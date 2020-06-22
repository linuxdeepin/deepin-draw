/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DBUSDRAW_ADAPTOR_H
#define DBUSDRAW_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
//#include "schedulestructs.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * @bref: dbusdraw_adaptor 提供给外部程序调用的方法
*/
class dbusdraw_adaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.Draw")
    Q_CLASSINFO("D-Bus Introspection", ""
                "  <interface name=\"com.deepin.Draw\">\n"

                "    <method name=\"openFiles\">\n"
                "      <arg direction=\"in\" type=\"none\" name=\"openFiles\"/>\n"
                "    </method>\n"

                "    <method name=\"openImages\">\n"
                "      <arg direction=\"in\" type=\"none\" name=\"openImages\"/>\n"
                "    </method>\n"

                "    <method name=\"openFile\">\n"
                "      <arg direction=\"in\" type=\"s\" name=\"openFile\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"

                "  </interface>\n")
public:
    dbusdraw_adaptor(QObject *parent);
    virtual ~dbusdraw_adaptor();

public Q_SLOTS: // METHODS
    void openFiles(QList<QVariant> filePaths);
    void openImages(QList<QVariant> images);
    bool openFile(QString filePath);

Q_SIGNALS: // SIGNALS
};

#endif // DBUSDRAW_ADAPTOR_H
