/*
 * Copyright (C) 2013  Daniel Vrátil <dvratil@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef PENDINGTPLOGGERSEARCH_H
#define PENDINGTPLOGGERSEARCH_H

#include <KTp/Logger/pending-logger-search.h>

namespace Tpl {
class PendingOperation;
}

class PendingTpLoggerSearch : public KTp::PendingLoggerSearch
{
    Q_OBJECT

  public:
    explicit PendingTpLoggerSearch(const QString& term, QObject* parent = 0);
    virtual ~PendingTpLoggerSearch();

  private Q_SLOTS:
    void searchFinished(Tpl::PendingOperation *op);
};

#endif // PENDINGTPLOGGERSEARCH_H
