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
#ifndef CDRAWPARAMSIGLETON_H
#define CDRAWPARAMSIGLETON_H
#include "globaldefine.h"
#include "sitemdata.h"
#include "cattributeitemwidget.h"

#include <QtGlobal>
#include <QColor>
#include <QPen>
#include <QFont>
class Page;
class PageScene;

class PageContext: public QObject
{
    Q_OBJECT
public:
    PageContext(const QString &file = "", QObject *parent = nullptr);

    QString key() const;

    Page *page()const;

    QString name()const;
    void    setName(const QString &name);

    QString file()const;
    void setFile(const QString &file);
    bool save(const QString &file = "");
    bool isDirty()const;
    void setDirty(bool dirty);

    DrawAttribution::SAttrisList currentAttris() const;
    void setDefaultAttri(int type, const QVariant &var);
    QVariant defaultAttri(int type) const;

    bool isEmpty()const;
    PageScene *scene() const;
    void addSceneItem(const CGraphicsUnit &var, bool record = false, bool releaseUnit = true, bool select = false);
    void addImage(const QImage &img, const QPointF &pos = QPointF(), const QRectF &rect = QRectF(), bool record = false, bool select = false);
    void addText(const QString &text, bool record = false, bool select = false);

    QRectF pageRect() const;
    void   setPageRect(const QRectF &rect);

    QImage renderToImage(const QColor &bgColor = Qt::transparent, const QSize &desImageSize = QSize()) const;

    void update();

    /**
     * @brief adaptImgPosAndRect　自适应图片位置和Rect
     * @param imgName　图片名
     * @param img　图片
     * @param pos　图片位置
     * @param rect 图片Rect
     */
    void adaptImgPosAndRect(const QString &imgName, const QImage &img, QPointF &pos, QRectF &rect)const;
signals:
    void dirtyChanged(bool isDirty);

private:
    QMap<int, QVariant> _attriValues;

    QString _key;
    QString _file;
    QString _name;

    PageScene *_scene = nullptr;
    Page *_page = nullptr;

    int _dirty: 1;
};


#endif // CDRAWPARAMSIGLETON_H
