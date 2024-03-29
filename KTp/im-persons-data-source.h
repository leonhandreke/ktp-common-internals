/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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


#ifndef IM_PERSONS_DATA_SOURCE_H
#define IM_PERSONS_DATA_SOURCE_H

#include <KPeople/BasePersonsDataSource>

#include <TelepathyQt/Types>

#include "KTp/ktp-export.h"

class KTP_EXPORT IMPersonsDataSource : public KPeople::BasePersonsDataSource
{
public:
    IMPersonsDataSource(QObject *parent, const QVariantList &data);
    virtual ~IMPersonsDataSource();
    virtual QString sourcePluginId() const;

    virtual KPeople::AllContactsMonitor* createAllContactsMonitor();
};

#endif // IM_PERSONS_DATA_SOURCE_H
