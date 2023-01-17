/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: ZhangYong <zhangyong@uniontech.com>
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

#ifndef COMDYNEW_H
#define COMDYNEW_H
#include <QMap>
#include <QString>
#include <QDebug>
typedef void *(*CREATE_FUNC)();
class ClassObjectFactory
{
public:
    template<class T>
    static T *creatObject(const QString &name)
    {
        auto p = creatClassObject(name);
        if (p != nullptr)
            return static_cast<T *>(p);
        return nullptr;
    }

    static void registerClass(const QString &name, CREATE_FUNC func);
    static void *creatClassObject(const QString &name);

private:
    static QMap<QString, CREATE_FUNC> *s_mapNameClass;
};

class ClassRegisterHelper
{
public:
    ClassRegisterHelper(const QString &name, CREATE_FUNC func)
    {
        qWarning() << "ClassRegisterHelper name =========== " << name;
        ClassObjectFactory::registerClass(name, func);
    }
};

#define REGISTCLASS_DECLARE(classname) \
    class classname##RegisterNameSpace{\
        static void* creatInstance(){\
            return new classname;\
        }\
        static ClassRegisterHelper s_reg1;\
    };
#define REGISTCLASS_DEFINE(classname) \
    ClassRegisterHelper classname##RegisterNameSpace::s_reg1 = ClassRegisterHelper(#classname, creatInstance);

#define REGISTCLASS(classname) \
    REGISTCLASS_DECLARE(classname) \
    REGISTCLASS_DEFINE(classname)
#endif // COMDYNEW_H
