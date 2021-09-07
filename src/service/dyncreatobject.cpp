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

#include "dyncreatobject.h"
QMap<QString, CREATE_FUNC> *ClassObjectFactory::s_mapNameClass = /*QMap<QString, CREATE_FUNC>()*/nullptr;
void *ClassObjectFactory::creatClassObject(const QString &name)
{
    if (s_mapNameClass == nullptr) {
        s_mapNameClass = new QMap<QString, CREATE_FUNC>;
    }
    auto it = s_mapNameClass->find(name);
    if (it != s_mapNameClass->end()) {
        CREATE_FUNC f = it.value();
        return f();
    }
    return nullptr;
}

void ClassObjectFactory::registerClass(const QString &name, CREATE_FUNC func)
{
    if (s_mapNameClass == nullptr) {
        s_mapNameClass = new QMap<QString, CREATE_FUNC>;
    }
    (*s_mapNameClass)[name] = func;
}

