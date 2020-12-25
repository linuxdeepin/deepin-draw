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
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <DStandardPaths>
#include <QPainter>
#include <QBrush>

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

//QString Global::cacheDir()
//{
//    auto userCachePath = DStandardPaths::standardLocations(QStandardPaths::CacheLocation).value(0);
//    return userCachePath;
//}

//QPixmap NSBlur::blurScene(const QGraphicsScene *pScene,
//                          const QPainterPath &blurPathInScene,
//                          const QList<QGraphicsItem *> list,
//                          EListUseTo useTo,
//                          int radius, int tp)
//{
//    if (pScene == nullptr || blurPathInScene.isEmpty())
//        return QPixmap();

//    QGraphicsScene *pScen = const_cast<QGraphicsScene *>(pScene);

//    QPixmap result;
//    QRect rect = blurPathInScene.boundingRect().toRect();
//    result = QPixmap(rect.width(), rect.height());
//    result.fill(QColor(255, 255, 255, 0));
//    QPainter painterd(&result);
//    painterd.setRenderHint(QPainter::Antialiasing);
//    painterd.setRenderHint(QPainter::SmoothPixmapTransform);

//    QBrush br = pScene->backgroundBrush();

//    pScen->setBackgroundBrush(Qt::transparent);


//    auto shouldFilter = list;
//    if (useTo == EToBeOnlyConsider) {
//        auto shouldFilters = pScen->items();
//        for (auto p : list) {
//            shouldFilters.removeOne(p);
//        }
//    }

//    for (auto p : shouldFilter) {
//        p->setFlag(QGraphicsItem::ItemHasNoContents, true);
//    }

//    const_cast<QGraphicsScene *>(pScene)->render(&painterd, QRectF(0, 0, result.width(), result.height()),
//                                                 rect);

//    painterd.end();

//    const_cast<QGraphicsScene *>(pScene)->setBackgroundBrush(br);

//    for (auto p : shouldFilter) {
//        p->setFlag(QGraphicsItem::ItemHasNoContents, false);
//    }

//    result = blurPixmap(result, radius, tp);

//    return result;
//}

QPixmap NSBlur::blurPixmap(const QPixmap &pix, int radius, int tp, const QRect &clipRect)
{
    QPixmap tmpPixmap = pix;

    if (clipRect.isValid()) {
        tmpPixmap = tmpPixmap.copy(clipRect);
    }

    int imgWidth = tmpPixmap.width();
    int imgHeigth = tmpPixmap.height();
    if (!tmpPixmap.isNull()) {
        tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (tp == 0) {
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth);
        }
    }
    return tmpPixmap;
}
