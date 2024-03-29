/*
 * Copyright (C) 2011 David Edmundson <kde@davidedmundson.co.uk>
 * Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@gmail.com>
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


#include "contact-grid-widget.h"

#include <KDE/KIcon>
#include <KDE/KLineEdit>
#include <KDE/KDebug>

#include <QtGui/QApplication>
#include <QtGui/QTextOption>
#include <QtGui/QPainter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListView>

#include "types.h"
#include <KTp/Models/contacts-list-model.h>
#include <KTp/Models/contacts-filter-model.h>

namespace KTp {

class ContactGridDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(ContactGridDelegate)

public:
    ContactGridDelegate(QObject *parent);
    virtual ~ContactGridDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

}; // class ContactGridDelegate

} // namespace KTp


KTp::ContactGridDelegate::ContactGridDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

KTp::ContactGridDelegate::~ContactGridDelegate()
{
}

void KTp::ContactGridDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyle *style = QApplication::style();
    int textHeight = option.fontMetrics.height() * 2;

    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    QRect avatarRect = option.rect.adjusted(0, 0, 0, -textHeight);
    QRect textRect = option.rect.adjusted(0, option.rect.height() - textHeight, 0, -3);

    QPixmap avatar;
    avatar.load(index.data(KTp::ContactAvatarPathRole).toString());
    if (avatar.isNull()) {
        avatar = KIcon(QLatin1String("im-user-online")).pixmap(option.decorationSize);
    } else if (avatar.width() > option.decorationSize.width() || avatar.height() > option.decorationSize.height()) {
        //resize larger avatars if required
        avatar = avatar.scaled(option.decorationSize, Qt::KeepAspectRatio);
        //draw leaving paddings on smaller (or non square) avatars
    }
    style->drawItemPixmap(painter, avatarRect, Qt::AlignCenter, avatar);


    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    painter->drawText(textRect, index.data().toString(), textOption);

}

QSize KTp::ContactGridDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    int textHeight = option.fontMetrics.height() * 2;
    return QSize(option.decorationSize.width() + 4, option.decorationSize.height() + textHeight + 3);
}

// -----------------------------------------------------------------------------

class KTp::ContactGridWidget::Private
{
public:
    Private(KTp::ContactGridWidget *parent)
        : q(parent),
          layout(new QVBoxLayout(parent)),
          contactGridView(new QListView(parent)),
          contactFilterLineEdit(new KLineEdit(parent)),
          contactsModel(0),
          filterModel(0)
    {
    }

    ~Private()
    {
    }

    void _k_onSelectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
    void _k_onDoubleClicked(const QModelIndex &index);

    KTp::ContactGridWidget *q;
    QVBoxLayout *layout;
    QListView *contactGridView;
    KLineEdit *contactFilterLineEdit;
    KTp::ContactsListModel *contactsModel;
    KTp::ContactsFilterModel *filterModel;
};

void KTp::ContactGridWidget::Private::_k_onSelectionChanged(const QItemSelection &newSelection,
                                                            const QItemSelection &oldSelection)
{
    Q_UNUSED(oldSelection)
    kDebug() << newSelection << oldSelection;

    if (newSelection.isEmpty()) {
        Q_EMIT q->selectionChanged(Tp::AccountPtr(), KTp::ContactPtr());
        return;
    }

    Q_EMIT q->selectionChanged(q->selectedAccount(), q->selectedContact());
}

void KTp::ContactGridWidget::Private::_k_onDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    Q_EMIT q->contactDoubleClicked(index.data(KTp::AccountRole).value<Tp::AccountPtr>(),
                                   index.data(KTp::ContactRole).value<KTp::ContactPtr>());
}


// -----------------------------------------------------------------------------

KTp::ContactGridWidget::ContactGridWidget(KTp::ContactsListModel* model, QWidget *parent)
    : QWidget(parent),
      d(new Private(this))
{
    d->filterModel = new KTp::ContactsFilterModel(this);

    d->contactsModel = model;
    d->filterModel->setSourceModel(d->contactsModel);

    d->contactGridView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->contactGridView->setResizeMode(QListView::Adjust);
    d->contactGridView->setSpacing(5);
    d->contactGridView->setViewMode(QListView::IconMode);
    d->contactGridView->setIconSize(QSize(80, 80));

    d->contactFilterLineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->contactFilterLineEdit->setClearButtonShown(true);

    d->layout->setMargin(0);
    d->layout->addWidget(d->contactGridView);
    d->layout->addWidget(d->contactFilterLineEdit);
    setLayout(d->layout);

    d->contactGridView->setModel(d->filterModel);
    d->contactGridView->setItemDelegate(new ContactGridDelegate(d->contactGridView));

    connect(d->contactGridView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(_k_onSelectionChanged(QItemSelection,QItemSelection)));
    connect(d->contactGridView,
            SIGNAL(doubleClicked(QModelIndex)),
            SLOT(_k_onDoubleClicked(QModelIndex)));

    connect(d->contactFilterLineEdit,
            SIGNAL(textChanged(QString)),
            d->filterModel,
            SLOT(setDisplayNameFilterString(QString)));
}

KTp::ContactGridWidget::~ContactGridWidget()
{
    delete d;
}

QString KTp::ContactGridWidget::displayNameFilter() const
{
    return d->contactFilterLineEdit->text();
}

void KTp::ContactGridWidget::clearDisplayNameFilter()
{
    setDisplayNameFilter(QString());
}

void KTp::ContactGridWidget::setDisplayNameFilter(const QString& displayNameFilter)
{
    if (displayNameFilter != d->contactFilterLineEdit->text()) {
        d->contactFilterLineEdit->setText(displayNameFilter);
        Q_EMIT displayNameFilterChanged(displayNameFilter);
    }
}

QSize KTp::ContactGridWidget::iconSize() const
{
    return d->contactGridView->iconSize();
}

void KTp::ContactGridWidget::setIconSize(const QSize& iconSize)
{
    kDebug();
    if (iconSize != d->contactGridView->iconSize()) {
        d->contactGridView->setIconSize(iconSize);
        Q_EMIT iconSizeChanged(iconSize);
    }
}

bool KTp::ContactGridWidget::hasSelection() const
{
    return d->contactGridView->selectionModel()->hasSelection();
}

Tp::AccountPtr KTp::ContactGridWidget::selectedAccount() const
{
    return d->contactGridView->currentIndex().data(KTp::AccountRole).value<Tp::AccountPtr>();
}

KTp::ContactPtr KTp::ContactGridWidget::selectedContact() const
{
    return d->contactGridView->currentIndex().data(KTp::ContactRole).value<KTp::ContactPtr>();
}

KTp::ContactsFilterModel* KTp::ContactGridWidget::filter() const
{
    return d->filterModel;
}

KLineEdit* KTp::ContactGridWidget::contactFilterLineEdit() const
{
    return d->contactFilterLineEdit;
}

#include "contact-grid-widget.moc"
#include "moc_contact-grid-widget.cpp"
