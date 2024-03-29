/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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

#include "imdetailswidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KPluginFactory>
#include <KABC/Addressee>

#include <KPeople/PersonData>
#include <TelepathyQt/AccountManager>
#include <KTp/core.h>

K_PLUGIN_FACTORY( ImDetailsWidgetFactory, registerPlugin<ImDetailsWidget>(); )
K_EXPORT_PLUGIN( ImDetailsWidgetFactory("imdetailswidgetplugin", "ktp-common-internals"))

using namespace KPeople;

ImDetailsWidget::ImDetailsWidget(QObject* parent, const QVariantList& args)
{
    Q_UNUSED(parent);
    Q_UNUSED(args);
}

QString ImDetailsWidget::label() const
{
    return i18n("IM");
}

QWidget* ImDetailsWidget::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const
{
    Q_UNUSED(person);
    QWidget *root = new QWidget(parent);
    QGridLayout *layout = new QGridLayout(root);
    root->setLayout(layout);

    int row = 0;
    Q_FOREACH(const KABC::Addressee &contact, contacts) {
        const QString contactId = contact.custom(QLatin1String("telepathy"), QLatin1String("contactId"));
        const QString accountPath = contact.custom(QLatin1String("telepathy"), QLatin1String("accountPath")); //probably unused till we fix everything properly

        Tp::AccountPtr account = KTp::accountManager()->accountForObjectPath(accountPath);
        if (!account) {
            continue;
        }

        QLabel *iconLabel = new QLabel(root);
        const int iconSize = root->style()->pixelMetric(QStyle::PM_SmallIconSize);
        iconLabel->setPixmap(QIcon::fromTheme(account->iconName()).pixmap(iconSize, iconSize));
        layout->addWidget(iconLabel, row, 0);

        QLabel *label = new QLabel(contactId, root);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout->addWidget(label, row, 1);
        qDebug() << contactId;
        row++;
        //FUTURE - presence here + blocked + presence subscription
    }
    if (layout->count()) {
        return root;
    } else {
        return 0;
    }
}

