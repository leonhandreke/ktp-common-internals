/*
* Copyright (C) 2011, 2012 Daniele E. Domenichelli <daniele.domenichelli@gmail.com>
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

#include "debug.h"

#include <KDE/KDebug>
#include <TelepathyQt/Debug>


namespace KTp
{

namespace {
int s_tpqtDebugArea;
QString libraryString;

static void tpDebugCallback(const QString &libraryName,
                            const QString &libraryVersion,
                            QtMsgType type,
                            const QString &msg)
{
    if (Q_UNLIKELY(libraryString.isEmpty())) {
        libraryString = QString::fromLatin1("%1:%2()").arg(libraryName, libraryVersion);
    }
    kDebugStream(type, s_tpqtDebugArea, __FILE__, __LINE__, libraryString.toLatin1()) << qPrintable(msg);
}
} // namespace
} // namespace KTp

void KTp::Debug::installCallback(bool debug, bool warning)
{
    s_tpqtDebugArea = KDebug::registerArea("Telepathy-Qt");

    // Redirect Tp debug and warnings to KDebug output
    Tp::setDebugCallback(&tpDebugCallback);

    // Enable telepathy-Qt4 debug
    Tp::enableDebug(debug);
    Tp::enableWarnings(warning);
}
