/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "global.h"
#include <DStandardPaths>

DCORE_USE_NAMESPACE;


GlobalShortcut *GlobalShortcut::m_globalSc = nullptr;

GlobalShortcut *GlobalShortcut::instance()
{
    if (!m_globalSc) {
        m_globalSc = new GlobalShortcut();
    }

    return m_globalSc;
}

GlobalShortcut::GlobalShortcut(QObject *parent)
{
    Q_UNUSED(parent);
}

GlobalShortcut::~GlobalShortcut()
{
    m_shiftSc = false;
    m_altSc = false;
}


QString Global::configPath()
{
    QString userConfigPath = DStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return userConfigPath;
}

QString Global::cacheDir()
{
    auto userCachePath = DStandardPaths::standardLocations(QStandardPaths::CacheLocation).value(0);
    return userCachePath;
}
