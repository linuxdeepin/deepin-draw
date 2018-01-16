/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DBUSDRAWSERVICE_H
#define DBUSDRAWSERVICE_H

#include <QObject>
#include <QDBusAbstractAdaptor>

#include "frame/mainwindow.h"

class DBusDrawService: public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.Draw")
    Q_CLASSINFO("D-Bus Introspection", ""
"<interface name=\"com.deepin.Draw\">\n"
    "<method name=\"activeWindow\"/>\n"
    "<method name=\"loadImage\">\n"
    "   <arg direction=\"in\" type=\"s\"/>\n"
    "</method>\n"
    "<method name=\"openImage\">\n"
    "   <arg direction=\"in\" type=\"s\"/>\n"
    "</method>\n"
 "</interface>\n")
public:
    explicit DBusDrawService(MainWindow* parent);
    ~DBusDrawService();

    inline MainWindow *parent() const
    { return static_cast<MainWindow *>(QObject::parent()); }

public Q_SLOTS:
    Q_SCRIPTABLE void openImage(const QString &path);
    Q_SCRIPTABLE void loadImage(const QString &path);
    Q_SCRIPTABLE void activeWindow();
};
#endif // DBUSDRAWSERVICE_H
