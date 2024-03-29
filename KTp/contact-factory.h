/*
* Copyright (C) 2012 David Edmundson <kde@davidedmundson.co.uk>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KTP_CONTACT_FACTORY_H
#define KTP_CONTACT_FACTORY_H

#include <TelepathyQt/ContactFactory>
#include <TelepathyQt/Types>

#include <KTp/ktp-export.h>

namespace KTp {
class KTP_EXPORT ContactFactory : public Tp::ContactFactory
{
public:
    static Tp::ContactFactoryPtr create(const Tp::Features &features=Tp::Features());
protected:
    ContactFactory(const Tp::Features &features);
    virtual Tp::ContactPtr construct(Tp::ContactManager *manager, const Tp::ReferencedHandles &handle, const Tp::Features &features, const QVariantMap &attributes) const;
};
}


#endif // CONTACTFACTORY_H
