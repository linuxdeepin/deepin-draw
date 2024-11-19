// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CSIZEHANDLERECT
#define CSIZEHANDLERECT

#if (QT_VERSION_MAJOR == 5)
#include <QGraphicsSvgItem>
#elif (QT_VERSION_MAJOR == 6)
#include <QtSvgWidgets/QGraphicsSvgItem>
#endif

#include <QGraphicsRectItem>
#include <QList>
#include <QSvgRenderer>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class PageView;
class CGraphicsItem;

enum { SELECTION_HANDLE_SIZE = 15, SELECTION_MARGIN = 10 };
enum ESelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };

/**
 * @brief The CSizeHandleRect class 外接矩形类
 */
// TODO: 后续做qt5兼容处理，目前加了有问题
class CSizeHandleRect : public QGraphicsItem
{
public:
    enum EDirection { LeftTop, Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left, Rotation, InRect, None};

    CSizeHandleRect(QGraphicsItem *parent, EDirection d);
    CSizeHandleRect(QGraphicsItem *parent, EDirection d, const QString &filename);

    EDirection dir() const
    {
        return m_dir;
    }
    PageView *curView()const;
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

    //void initCursor();

private:
    const EDirection m_dir;
    ESelectionHandleState m_state;
    bool  m_bVisible;   //是否显示  true 显示  false不显示
    QSvgRenderer m_lightRenderer;
    QSvgRenderer m_darkRenderer;
    bool m_isRotation;
    bool m_onlyLogicAblity = false;

private:
    //void initCursor();
};
#endif // CSIZEHANDLERECT

