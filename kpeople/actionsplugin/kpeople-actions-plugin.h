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


#ifndef IM_PLUGIN_H
#define IM_PLUGIN_H

#include <KPeople/AbstractPersonAction>

class KPeopleActionsPlugin : public KPeople::AbstractPersonAction
{
    Q_OBJECT
public:
    KPeopleActionsPlugin(QObject *parent, const QVariantList &args);
    virtual QList<QAction*> actionsForPerson(const KABC::Addressee &person,
                                             const KABC::AddresseeList &contacts,
                                             QObject *parent) const;

private Q_SLOTS:
    void onActionTriggered();
};

#endif // IM_PLUGIN_H
