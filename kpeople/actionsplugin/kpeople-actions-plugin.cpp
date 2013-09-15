/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "kpeople-actions-plugin.h"

#include <QAction>

#include <KIcon>
#include <KLocalizedString>
#include <KPluginFactory>
#include <kdemacros.h>

#include "KTp/contact.h"
#include "KTp/actions.h"
#include "KTp/im-persons-data-source.h"

#include <TelepathyQt/Account>
#include <TelepathyQt/ContactManager>


#include <kpeople/personpluginmanager.h>
#include <KPeople/PersonData>

enum IMActionType {
    TextChannel,
    AudioChannel,
    VideoChannel,
    FileTransfer,
    LogViewer
};

class IMAction : public QAction {
    Q_OBJECT
public:
    IMAction(const QString &text, const KIcon &icon, const KTp::ContactPtr &contact,
             const Tp::AccountPtr &account, IMActionType type, QObject *parent);
    IMAction(const QString &text, const KIcon &icon, const QUrl &uri,
             IMActionType type, QObject *parent);
    KTp::ContactPtr contact() const;
    Tp::AccountPtr account() const;
    IMActionType type() const;
    QUrl uri() const;
private:
    KTp::ContactPtr m_contact;
    Tp::AccountPtr m_account;
    QUrl m_uri;
    IMActionType m_type;
};

IMAction::IMAction(const QString &text, const KIcon &icon, const KTp::ContactPtr &contact,
                   const Tp::AccountPtr &account, IMActionType type, QObject *parent):
    QAction(icon, text, parent),
    m_contact(contact),
    m_account(account),
    m_type(type)
{
}

IMAction::IMAction(const QString &text, const KIcon &icon, const QUrl &uri,
                   IMActionType type, QObject *parent):
    QAction(icon, text, parent),
    m_uri(uri),
    m_type(type)
{
}

KTp::ContactPtr IMAction::contact() const
{
    return m_contact;
}

Tp::AccountPtr IMAction::account() const
{
    return m_account;
}

IMActionType IMAction::type() const
{
    return m_type;
}

QUrl IMAction::uri() const
{
    return m_uri;
}

KPeopleActionsPlugin::KPeopleActionsPlugin(QObject* parent, const QVariantList &args):
AbstractPersonPlugin(parent)
{
}

QList<QAction*> KPeopleActionsPlugin::actionsForPerson(const KPeople::PersonDataPtr &personData, QObject *parent)
{
    QList<QAction*> actions;

    IMPersonsDataSource *dataSource = dynamic_cast<IMPersonsDataSource*>(KPeople::PersonPluginManager::presencePlugin());
    if (!dataSource) {
        return actions;
    }

    QStringList imContactsIds = personData->imAccounts();

    for (int i = 0; i < imContactsIds.size(); i++) {
        const QString contactId = imContactsIds[i];
        const KTp::ContactPtr contact = dataSource->contactForContactId(contactId);
        if (!contact || !contact->manager()) {
            continue;
        }
        const Tp::AccountPtr account = dataSource->accountForContact(contact);

        if (!account) {
            continue;
        }

        if (true) { //no such query for text chat capability, added an "if true" because makes the code look consistent
            QAction *action = new IMAction(i18n("Start Chat Using %1...", account->displayName()),
                                KIcon(QLatin1String("text-x-generic")),
                                contact,
                                account,
                                TextChannel,
                                parent);
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }
        if (contact->audioCallCapability()) {
            QAction *action = new IMAction(i18n("Start Audio Call Using %1...", account->displayName()),
                                KIcon(QLatin1String("audio-headset")),
                                contact,
                                account,
                                AudioChannel,
                                parent);
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }
        if (contact->videoCallCapability()) {
            QAction *action = new IMAction(i18n("Start Video Call Using %1...", account->displayName()),
                                KIcon(QLatin1String("camera-web")),
                                contact,
                                account,
                                VideoChannel,
                                parent);
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }

        if (contact->fileTransferCapability()) {
            QAction *action = new IMAction(i18n("Send a File Using %1...", account->displayName()),
                                        KIcon(QLatin1String("mail-attachment")),
                                        contact,
                                        account,
                                        FileTransfer,
                                        parent);
            action->setDisabled(true); //FIXME: we need to prompt for file
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }
    }

    QAction *action = new IMAction(i18n("Open Log Viewer..."),
                                   KIcon(QLatin1String("documentation")),
                                   personData->uri(),
                                   LogViewer,
                                   parent);
    connect(action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
    actions << action;
    return actions;
}

void KPeopleActionsPlugin::onActionTriggered()
{
    IMAction *action = qobject_cast<IMAction*>(sender());
    KTp::ContactPtr contact = action->contact();
    Tp::AccountPtr account = action->account();
    IMActionType type = action->type();

    switch (type) {
        case TextChannel:
            KTp::Actions::startChat(account, contact);
            break;
        case AudioChannel:
            KTp::Actions::startAudioCall(account, contact);
            break;
        case VideoChannel:
            KTp::Actions::startAudioVideoCall(account, contact);
            break;
        case FileTransfer:
            //TODO: add filetransfer
            break;
        case LogViewer:
            KTp::Actions::openLogViewer(action->uri());
            break;
    }
}

#include "kpeople-actions-plugin.moc"
#include "moc_kpeople-actions-plugin.cpp"


K_PLUGIN_FACTORY( KPeopleActionsPluginFactory, registerPlugin<KPeopleActionsPlugin>(); )
K_EXPORT_PLUGIN( KPeopleActionsPluginFactory("ktp_kpeople_plugin") )
