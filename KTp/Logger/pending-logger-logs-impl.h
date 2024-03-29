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

#ifndef PENDINGLOGGERLOGSIMPL_H
#define PENDINGLOGGERLOGSIMPL_H

#include "pending-logger-logs.h"

class PendingLoggerLogsImpl : public KTp::PendingLoggerLogs
{
    Q_OBJECT

  public:
    explicit PendingLoggerLogsImpl(const Tp::AccountPtr &account,
                                   const KTp::LogEntity &entity,
                                   const QDate &date,
                                   QObject *parent = 0);
    virtual ~PendingLoggerLogsImpl();

  private Q_SLOTS:
    void operationFinished(KTp::PendingLoggerOperation *op);

  private:
    QList<KTp::PendingLoggerOperation*> mRunningOps;
};

#endif // PENDINGLOGGERLOGSIMPL_H
