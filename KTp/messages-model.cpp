/*
    Copyright (C) 2011  Lasath Fernando <kde@lasath.org>
    Copyright (C) 2013  Lasath Fernando <davidedmundson@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "messages-model.h"

#include <KDebug>
#include <KLocalizedString>

#include <TelepathyQt/ReceivedMessage>
#include <TelepathyQt/TextChannel>
#include <TelepathyQt/Account>

#include <KTp/message-processor.h>
#include <KTp/message-context.h>
//#include <KTp/Logger/scrollback-manager.h>

class MessagePrivate
{
  public:
    MessagePrivate(const KTp::Message &message);

    KTp::Message message;
    MessagesModel::DeliveryStatus deliveryStatus;
    QDateTime deliveryReportReceiveTime;
};

MessagePrivate::MessagePrivate(const KTp::Message &message) :
        message(message),
        deliveryStatus(MessagesModel::DeliveryStatusUnknown)
{
}

class MessagesModel::MessagesModelPrivate
{
  public:
    Tp::TextChannelPtr textChannel;
    Tp::AccountPtr account;
    //ScrollbackManager *logManager;
    QList<MessagePrivate> messages;
    // For fast lookup of original messages upon receipt of a message delivery report.
    QHash<QString /*messageToken*/, QPersistentModelIndex> messagesByMessageToken;
    bool visible;
    bool logsLoaded;
};

MessagesModel::MessagesModel(const Tp::AccountPtr &account, QObject *parent) :
        QAbstractListModel(parent),
        d(new MessagesModelPrivate)
{
    kDebug();

    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[TimeRole] = "time";
    roles[TypeRole] = "type";
    roles[SenderIdRole] = "senderId";
    roles[SenderAliasRole] = "senderAlias";
    roles[SenderAvatarRole] = "senderAvatar";
    roles[DeliveryStatusRole] = "deliveryStatus";
    roles[DeliveryReportReceiveTimeRole] = "deliveryReportReceiveTime";
    setRoleNames(roles);

    d->account = account;
    d->visible = false;

    //d->logManager = new ScrollbackManager(this);
    d->logsLoaded = false;
    //connect(d->logManager, SIGNAL(fetched(QList<KTp::Message>)), SLOT(onHistoryFetched(QList<KTp::Message>)));

    //Load configuration for number of message to show
    KConfig config(QLatin1String("ktelepathyrc"));
    KConfigGroup tabConfig = config.group("Behavior");
    //d->logManager->setScrollbackLength(tabConfig.readEntry<int>("scrollbackLength", 10));
}

Tp::TextChannelPtr MessagesModel::textChannel() const
{
    return d->textChannel;
}

bool MessagesModel::verifyPendingOperation(Tp::PendingOperation *op)
{
    bool operationSucceeded = true;

    if (op->isError()) {
        kWarning() << op->errorName() << "+" << op->errorMessage();
        operationSucceeded = false;
    }

    return operationSucceeded;
}

void MessagesModel::setupChannelSignals(const Tp::TextChannelPtr &channel)
{
    connect(channel.data(),
            SIGNAL(messageReceived(Tp::ReceivedMessage)),
            SLOT(onMessageReceived(Tp::ReceivedMessage)));
    connect(channel.data(),
            SIGNAL(messageSent(Tp::Message,Tp::MessageSendingFlags,QString)),
            SLOT(onMessageSent(Tp::Message,Tp::MessageSendingFlags,QString)));
    connect(channel.data(),
            SIGNAL(pendingMessageRemoved(Tp::ReceivedMessage)),
            SLOT(onPendingMessageRemoved()));
}

void MessagesModel::setTextChannel(const Tp::TextChannelPtr &channel)
{
    Q_ASSERT(channel != d->textChannel);
    kDebug();
    setupChannelSignals(channel);

    if (d->textChannel) {
        removeChannelSignals(d->textChannel);
    }

    d->textChannel = channel;

    //d->logManager->setTextChannel(d->account, d->textChannel);

    //Load messages unless they have already been loaded
    if(!d->logsLoaded) {
        //d->logManager->fetchScrollback();
    }

    QList<Tp::ReceivedMessage> messageQueue = channel->messageQueue();
    Q_FOREACH(const Tp::ReceivedMessage &message, messageQueue) {
        bool messageAlreadyInModel = false;
        Q_FOREACH(const MessagePrivate &current, d->messages) {
            //FIXME: docs say messageToken can return an empty string. What to do if that happens?
            //Tp::Message has an == operator. maybe I can use that?
            if (current.message.token() == message.messageToken()) {
                messageAlreadyInModel = true;
                break;
            }
        }
        if (!messageAlreadyInModel) {
            onMessageReceived(message);
        }
    }
}

void MessagesModel::onHistoryFetched(const QList<KTp::Message> &messages)
{
    kDebug() << "found" << messages.count() << "messages in history";
    if (!messages.isEmpty()) {
        //Add all messages before the ones already present in the channel
        beginInsertRows(QModelIndex(), 0, messages.count() - 1);
        for(int i=messages.size()-1;i>=0;i--) {
            d->messages.prepend(messages[i]);
        }
        endInsertRows();
    }
    d->logsLoaded = true;
}

void MessagesModel::onMessageReceived(const Tp::ReceivedMessage &message)
{
    int unreadCount = d->textChannel->messageQueue().size();
    kDebug() << "unreadMessagesCount =" << unreadCount;
    kDebug() << "text =" << message.text();
    kDebug() << "messageType = " << message.messageType();
    kDebug() << "messageToken =" << message.messageToken();

    if (message.isDeliveryReport()) {
        d->textChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);

        Tp::ReceivedMessage::DeliveryDetails deliveryDetails = message.deliveryDetails();
        if(!deliveryDetails.hasOriginalToken()) {
            kDebug() << "Delivery report without original message token received.";
            // Matching the delivery report to the original message is impossible without the token.
            return;
        }
        kDebug() << "originalMessageToken =" << deliveryDetails.originalToken();

        QPersistentModelIndex originalMessageIndex = d->messagesByMessageToken.value(
                    deliveryDetails.originalToken());
        if (!originalMessageIndex.isValid() || originalMessageIndex.row() >= d->messages.count()) {
            // The original message for this delivery report was not found.
            return;
        }

        MessagePrivate &originalMessage = d->messages[originalMessageIndex.row()];
        kDebug() << "Got delivery status" << deliveryDetails.status()
                 << "for message with text" << originalMessage.message.mainMessagePart();
        originalMessage.deliveryReportReceiveTime = message.received();
        switch(deliveryDetails.status()) {
        case Tp::DeliveryStatusPermanentlyFailed:
        case Tp::DeliveryStatusTemporarilyFailed:
            originalMessage.deliveryStatus = DeliveryStatusFailed;
            if (deliveryDetails.hasDebugMessage()) {
                kDebug() << "Delivery failure debug message:" << deliveryDetails.debugMessage();
            }
            break;
        case Tp::DeliveryStatusDelivered:
            originalMessage.deliveryStatus = DeliveryStatusDelivered;
            break;
        case Tp::DeliveryStatusRead:
            originalMessage.deliveryStatus = DeliveryStatusRead;
            break;
        default:
            originalMessage.deliveryStatus = DeliveryStatusUnknown;
            break;
        }
        Q_EMIT dataChanged(originalMessageIndex, originalMessageIndex);
    } else {
        int length = rowCount();
        beginInsertRows(QModelIndex(), length, length);

        KTp::Message newMessage = KTp::MessageProcessor::instance()->processIncomingMessage(
                    message, d->account, d->textChannel);
        d->messages.append(newMessage);

        endInsertRows();
        Q_EMIT(messageReceived(newMessage));
        if (d->visible) {
            acknowledgeAllMessages();
        } else {
            Q_EMIT unreadCountChanged(unreadCount);
        }
    }
}

void MessagesModel::onMessageSent(const Tp::Message &message, Tp::MessageSendingFlags flags, const QString &messageToken)
{
    Q_UNUSED(flags);

    int length = rowCount();
    beginInsertRows(QModelIndex(), length, length);
    kDebug() << "text =" << message.text();

    const KTp::Message &newMessage = KTp::MessageProcessor::instance()->processIncomingMessage(
                message, d->account, d->textChannel);
    d->messages.append(newMessage);

    if (!messageToken.isEmpty()) {
        // Insert the message into the lookup table for delivery reports.
        const QPersistentModelIndex &modelIndex(createIndex(length, 0));
        d->messagesByMessageToken.insert(messageToken, modelIndex);
    }

    endInsertRows();
}

void MessagesModel::onPendingMessageRemoved()
{
    Q_EMIT unreadCountChanged(unreadCount());
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (index.isValid()) {
        const MessagePrivate m = d->messages[index.row()];

        switch (role) {
        case TextRole:
            result = m.message.finalizedMessage();
            break;
        case TypeRole:
            if (m.message.type() == Tp::ChannelTextMessageTypeAction) {
                result = MessageTypeAction;
            } else {
                if (m.message.direction() == KTp::Message::LocalToRemote) {
                    result = MessageTypeOutgoing;
                } else {
                    result = MessageTypeIncoming;
                }
            }
            break;
        case TimeRole:
            result = m.message.time();
            break;
        case SenderIdRole:
            result = m.message.senderId();
            break;
        case SenderAliasRole:
            result = m.message.senderAlias();
            break;
        case SenderAvatarRole:
            if (m.message.sender()) {
                result = QVariant::fromValue(m.message.sender()->avatarPixmap());
            }
            break;
        case DeliveryStatusRole:
            result = m.deliveryStatus;
            break;
        case DeliveryReportReceiveTimeRole:
            result = m.deliveryReportReceiveTime;
            break;
        };
    } else {
        kError() << "Attempting to access data at invalid index (" << index << ")";
    }

    return result;
}

int MessagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->messages.size();
}

void MessagesModel::sendNewMessage(const QString &message)
{
    if (message.isEmpty()) {
        kWarning() << "Attempting to send empty string";
    } else {
        Tp::PendingOperation *op;
        QString modifiedMessage = message;
        if (d->textChannel->supportsMessageType(Tp::ChannelTextMessageTypeAction)
                && modifiedMessage.startsWith(QLatin1String("/me "))) {
            //remove "/me " from the start of the message
            modifiedMessage.remove(0,4);
            op = d->textChannel->send(modifiedMessage, Tp::ChannelTextMessageTypeAction);
        } else {
            op = d->textChannel->send(modifiedMessage);
        }
        connect(op,
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(verifyPendingOperation(Tp::PendingOperation*)));
    }
}

void MessagesModel::removeChannelSignals(const Tp::TextChannelPtr &channel)
{
    QObject::disconnect(channel.data(),
                        SIGNAL(messageReceived(Tp::ReceivedMessage)),
                        this,
                        SLOT(onMessageReceived(Tp::ReceivedMessage))
                       );
    QObject::disconnect(channel.data(),
                        SIGNAL(messageSent(Tp::Message,Tp::MessageSendingFlags,QString)),
                        this,
                        SLOT(onMessageSent(Tp::Message,Tp::MessageSendingFlags,QString))
                       );
}

int MessagesModel::unreadCount() const
{
    return d->textChannel->messageQueue().size();
}

void MessagesModel::acknowledgeAllMessages()
{
    QList<Tp::ReceivedMessage> queue = d->textChannel->messageQueue();

    kDebug() << "Conversation Visible, Acknowledging " << queue.size() << " messages.";

    d->textChannel->acknowledge(queue);
    Q_EMIT unreadCountChanged(queue.size());
}

void MessagesModel::setVisibleToUser(bool visible)
{
    kDebug() << visible;

    if (d->visible != visible) {
        d->visible = visible;
        Q_EMIT visibleToUserChanged(d->visible);
    }

    if (visible) {
        acknowledgeAllMessages();
    }
}

bool MessagesModel::isVisibleToUser() const
{
    return d->visible;
}

MessagesModel::~MessagesModel()
{
    kDebug();
    delete d;
}

bool MessagesModel::shouldStartOpened() const
{
    return d->textChannel->isRequested();
}
