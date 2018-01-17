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
#include "application.h"
#include "dbusdrawservice.h"

#include <QDebug>

DBusDrawService::DBusDrawService(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
{
}

DBusDrawService::~DBusDrawService()
{
}

void DBusDrawService::openImage(const QString &path)
{
    qDebug() << "Go to open image: " << path;
    parent()->openImage(path);
}

void DBusDrawService::loadImage(const QString &path)
{
    qDebug() << "Go to load image: " << path;
    parent()->loadImage(path);
}

void DBusDrawService::activeWindow()
{
    parent()->activeWindow();
}
