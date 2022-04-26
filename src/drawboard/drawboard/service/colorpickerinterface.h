/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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

#ifndef COLOR_PICKER_INTERFACE_H
#define COLOR_PICKER_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.Picker
 */
class ColorPickerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "com.deepin.Picker";
    }

public:
    ColorPickerInterface(const QString &service, const QString &path,
                         const QDBusConnection &connection, QObject *parent = nullptr);

    ~ColorPickerInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> StartPick(const QString &appid)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(appid);
        return asyncCallWithArgumentList(QStringLiteral("StartPick"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void colorPicked(const QString &appid, const QString &color);
};

namespace com {
namespace deepin {
typedef ::ColorPickerInterface Picker;
}
}
#endif
