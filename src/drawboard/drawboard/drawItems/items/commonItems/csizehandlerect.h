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
#ifndef CSIZEHANDLERECT
#define CSIZEHANDLERECT

#include "../../../utils/globaldefine.h"
#include "toolsceneevent.h"

#include <QList>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>

class PageItem;
class PageView;

class DRAWLIB_EXPORT HandleNode : public QGraphicsSvgItem
{
    Q_OBJECT
public:
    enum EInnerType {
        None,
        Resize_LT,
        Resize_T,
        Resize_RT,
        Resize_R,
        Resize_RB,
        Resize_B,
        Resize_LB,
        Resize_L,
        Rotation,
        UserCustom = 1000
    };

    HandleNode(int d, PageItem *parent = nullptr);
    HandleNode(int d, const QString &iconPath, PageItem *parent = nullptr);

    int nodeType() const;

    void setIgnoreScal(bool b);
    bool isIgnoreScal() const;

    PageView *pageView()const;
    PageItem *parentPageItem()const;

    void   setIconVisible(bool flag);
    bool   isIconVisible() const;

    static void    getTransBlockFlag(EInnerType dir, bool &blockX, bool &blockY);
    static void    getTransNegtiveFlag(EInnerType dir, bool &negtiveX, bool &negtiveY);
    static QPointF transCenter(EInnerType dir, PageItem *pItem);

    QRectF validRect() const;

    virtual void pressBegin(int tool, ToolSceneEvent *event);
    virtual void pressMove(int tool, ToolSceneEvent *event);
    virtual void pressRelease(int tool, ToolSceneEvent *event);

    void update();
    QCursor innerCursor(int tp) const;

    virtual QCursor cursor()const;

protected:
    bool contains(const QPointF &point) const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange doChange, const QVariant &value) override;

    virtual void parentItemChanged(int doChange, const QVariant &value);
    virtual void setNodePos();
    virtual void moveCenterTo(const QPointF &pos);
    virtual bool isVisbleCondition()const;


    PRIVATECLASS(HandleNode)

    friend class PageItem;
};

typedef QList<HandleNode *> Handles;




#endif // CSIZEHANDLERECT

