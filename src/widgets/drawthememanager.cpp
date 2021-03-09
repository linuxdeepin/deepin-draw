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
#include "drawthememanager.h"

#include <QApplication>


#include "utils/baseutils.h"

namespace {
const QString THEME_GROUP = "APP";
const QString THEME_TEXT = "AppTheme";
}

DrawThemeManager *DrawThemeManager::m_drawTheme = NULL;
DrawThemeManager *DrawThemeManager::instance()
{
    if (m_drawTheme == NULL) {
        m_drawTheme = new DrawThemeManager;
    }

    return m_drawTheme;
}

DrawThemeManager::DrawThemeManager(QObject *parent) : QObject(parent)
{
}

//QString DrawThemeManager::getCurrentTheme()
//{
//    return m_currentTheme;
//}

//void DrawThemeManager::setCurrentTheme(const QString themeName)
//{
//    m_currentTheme = themeName;

//    emit themeChanged(m_currentTheme);
//}

//QString DrawThemeManager::getQssForWidget(QString className)
//{
//    return getFileContent(QString(":/theme/%1/%2.qss").arg(m_currentTheme).arg(className));
//}

//void DrawThemeManager::updateQss()
//{
//    DWidget *w = qobject_cast<DWidget *>(sender());
////    if (w) {
////        w->setStyleSheet(w->styleSheet());
////    }
//}
