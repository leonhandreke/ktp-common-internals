/*
    Copyright (C) 2013  Dan Vrátil <dvratil@redhat.com>

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


#include "qml-plugins.h"

#include <QtDeclarative/QDeclarativeItem>

#include "contact-list.h"
#include "conversation.h"
#include "conversations-model.h"
#include "conversation-target.h"
#include "hide-window-component.h"
#include "messages-model.h"
#include "telepathy-text-observer.h"

#include "Models/accounts-filter-model.h"

void QmlPlugins::registerTypes(const char *uri)
{
    qmlRegisterType<ContactList> (uri, 0, 1, "ContactList");
    qmlRegisterUncreatableType<AccountsFilterModel> (uri, 0, 1, "AccountsFilterModel",
        QLatin1String("Filter cannot be created. Access through ContactList.filter"));

    qmlRegisterType<TelepathyTextObserver> (uri, 0, 1, "TelepathyTextObserver");
    qmlRegisterType<Conversation>(uri, 0, 1, "Conversation");
    qmlRegisterType<HideWindowComponent>(uri, 0, 1, "HideWindowComponent");

    //needed for MessageType enum
    qmlRegisterUncreatableType<MessagesModel>(uri, 0, 1, "MessagesModel",
        QLatin1String("MessagesModel can not be instanitized directly. Use a TelepathyTextObserver instead"));
}

Q_EXPORT_PLUGIN2(conversation, QmlPlugins);

