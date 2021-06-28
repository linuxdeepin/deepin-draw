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
#ifndef GLOBAL_H
#define GLOBAL_H

#include "widgets/drawthememanager.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QStyle>

#define DRAW_THEME_INIT_WIDGET(className, ...) do {\
        DrawThemeManager * manager = DrawThemeManager::instance(); \
        this->setStyleSheet(this->styleSheet() + manager->getQssForWidget(className)); \
        connect(manager, &DrawThemeManager::themeChanged, this, [this, manager] (QString) { \
            this->setStyleSheet(manager->getQssForWidget(className)); \
        });\
        QStringList list = QString(#__VA_ARGS__).replace(" ", "").split(",");\
        const QMetaObject *self = metaObject();\
        Q_FOREACH (const QString &str, list) {\
            if(str.isEmpty())\
                continue;\
            connect(this, self->property(self->indexOfProperty(str.toLatin1().data())).notifySignal(),\
                    manager, manager->metaObject()->method(manager->metaObject()->indexOfMethod("updateQss()")));\
        } \
    } while (0);

class GlobalShortcut : public QObject
{
    Q_OBJECT
public:
    explicit GlobalShortcut(QObject *parent = nullptr);
    ~GlobalShortcut();
    static GlobalShortcut *instance() ;

//    inline void setShiftScStatus(bool status)
//    {
//        m_shiftSc = status;
//    }
//    inline void setAltScStatus(bool status)
//    {
//        m_altSc = status;
//    }
//    bool shiftSc()
//    {
//        return m_shiftSc;
//    }
//    bool altSc()
//    {
//        return m_altSc;
//    }

private:
    bool m_shiftSc = false;
    bool m_altSc = false;
    static  GlobalShortcut *m_globalSc;
};

class Global
{
public:
    static QString configPath();
//    static QString cacheDir();
};

class QGraphicsScene;
class QGraphicsItem;
namespace NSBlur {
QPixmap blurPixmap(const QPixmap &pix, int radius = 10, int tp = 0, const QRect &clipRect = QRect());
QImage  blurImage(const QImage &pix, int radius = 10, int tp = 0, const QRect &clipRect = QRect());

enum EListUseTo {EToBeFilter, EToBeOnlyConsider};
//QPixmap blurScene(const QGraphicsScene *pScene,
//                  const QPainterPath &blurPathInScene,
//                  const QList<QGraphicsItem *> list = QList<QGraphicsItem *>(),
//                  EListUseTo useTo = EToBeFilter,
//                  int radius = 10,
//                  int tp = 0);
}


#endif // GLOBAL_H
