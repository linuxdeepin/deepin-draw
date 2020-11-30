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
#ifndef CSIZEHANDLERECT
#define CSIZEHANDLERECT

#include <QGraphicsRectItem>
#include <QList>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class CGraphicsView;
class CGraphicsItem;

enum { SELECTION_HANDLE_SIZE = 15, SELECTION_MARGIN = 10 };
enum ESelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };

/**
 * @brief The CSizeHandleRect class 外接矩形类
 */
class CSizeHandleRect : public QGraphicsSvgItem
{
public:
    enum EDirection { LeftTop, Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left, Rotation, InRect, None};

    CSizeHandleRect(QGraphicsItem *parent, EDirection d);
    CSizeHandleRect(QGraphicsItem *parent, EDirection d, const QString &filename);

    EDirection dir() const
    {
        return m_dir;
    }
    CGraphicsView *curView()const;
    void setState(ESelectionHandleState st);
    bool hitTest(const QPointF &point);
    void move(qreal x, qreal y);
    QRectF boundingRect() const override;
    void setVisible(bool flag);


    void setJustExitLogicAbility(bool b);

    QCursor getCursor();


    static void    getTransBlockFlag(EDirection dir, bool &blockX, bool &blockY);
    static void    getTransNegtiveFlag(EDirection dir, bool &negtiveX, bool &negtiveY);
    static QPointF transCenter(EDirection dir, CGraphicsItem *pItem);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    bool isFatherDragging();  //如果为true不会进行绘制

private:
    const EDirection m_dir;
    ESelectionHandleState m_state;
    bool  m_bVisible;   //是否显示  true 显示  false不显示
    QSvgRenderer m_lightRenderer;
    QSvgRenderer m_darkRenderer;
    bool m_isRotation;
    bool m_onlyLogicAblity = false;

private:
//    void initCursor();
};


#endif // CSIZEHANDLERECT

