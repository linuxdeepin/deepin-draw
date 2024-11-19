// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "csizehandlerect.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "cdrawtoolmanagersigleton.h"
#include "cdrawscene.h"
#include "cselecttool.h"
#include "cgraphicsitem.h"
#include "application.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QDebug>
#include <QString>

//TODO： 这一块没做qt5兼容，先备注下，后续会补充上
CSizeHandleRect::CSizeHandleRect(QGraphicsItem *parent, EDirection d)
    : QGraphicsItem(parent) // 修改为 QGraphicsItem
    , m_dir(d)
    , m_state(SelectionHandleOff)
    , m_bVisible(true)
    , m_lightRenderer(QString(":/theme/light/images/size/node.svg"))
    , m_darkRenderer(QString(":/theme/dark/images/size/node-dark.svg"))
    , m_isRotation(false)
{
    setParentItem(parent);
    setCacheMode(NoCache);
    // setSharedRenderer(&m_lightRenderer); // 如果不再使用 QGraphicsSvgItem，这行可以移除
    //hide();
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);

    setCursor(getCursor());
}

CSizeHandleRect::CSizeHandleRect(QGraphicsItem *parent, EDirection d, const QString &filename)
    : QGraphicsItem(parent), m_dir(d), m_state(SelectionHandleOff), m_bVisible(true), m_isRotation(true)
{
    setParentItem(parent);
    setCacheMode(NoCache);
    //hide();

    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);

    setCursor(getCursor());
}

PageView *CSizeHandleRect::curView() const
{
    PageView *parentView = nullptr;
    if (scene() != nullptr) {
        if (!scene()->views().isEmpty()) {
            parentView = dynamic_cast<PageView *>(scene()->views().first());
        }
    }
    return parentView;
}

void CSizeHandleRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (!CGraphicsItem::paintInteractBorderLine)
        return;

    //如果仅存在功能那么什么都不用绘制了
    if (m_onlyLogicAblity)
        return;

    if (isFatherDragging())
        return;

    //在Qt6中，QGraphicsSvgItem的renderer()方法已经被移除。在Qt5中，renderer()方法是可用的，因此需要分别处理。
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!m_isRotation) {
        if (renderer() != &m_lightRenderer) {
            setSharedRenderer(&m_lightRenderer);
        }
    }
    #else
    // Qt6 处理逻辑
    if (!m_isRotation) {
        //// 在Qt6中直接使用m_lightRenderer进行绘制
        m_lightRenderer.render(painter, this->boundingRect());
    }
    #endif

    painter->setClipping(false);
    QRectF rect = this->boundingRect();

    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    this->renderer()->render(painter, rect);
    #else
    // 在Qt6中使用QSvgRenderer直接绘制
    m_lightRenderer.render(painter, rect);
    #endif

    painter->setClipping(true);
}

bool CSizeHandleRect::isFatherDragging()
{
    CGraphicsItem *pParentItem = dynamic_cast<CGraphicsItem *>(parentItem());
    if (pParentItem != nullptr && pParentItem->scene() != nullptr) {
        int model = pParentItem->drawScene()->page()->currentTool();
        int operatingTp = pParentItem->operatingType();
        if ((operatingTp == CSelectTool::EDragMove || operatingTp == CSelectTool::ECopyMove) && model == selection) {
            return true;
        }
    }
    return false;
}

void CSizeHandleRect::setState(ESelectionHandleState st)
{
    if (st != m_state) {
        switch (st) {
        case SelectionHandleOff:
            hide();
            break;
        case SelectionHandleInactive:
        case SelectionHandleActive:
            if (m_bVisible) {
                show();
            }
            break;
        }
        m_state = st;
    }
}

bool CSizeHandleRect::hitTest(const QPointF &point)
{
    bool bRet = false;
    if (m_bVisible) {
        QPointF pt = mapFromScene(point);
        bRet = this->boundingRect().contains(pt);
    }
    return bRet;
}

void CSizeHandleRect::move(qreal x, qreal y)
{
    setPos(x, y);
}

QRectF CSizeHandleRect::boundingRect() const
{
    if (curView() == nullptr)
        return QRectF();

    qreal scale = curView()->getScale();
    QRectF rect;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Qt5 处理逻辑
    rect = QGraphicsSvgItem::boundingRect();
#else
    // Qt6 中使用 QSvgRenderer::viewBoxF() 方法。这将返回整个SVG的视图框矩形
    // TODO: 这里以后可能需要留意下，感觉可能会出ui BUG，目前没发现问题
    rect = m_lightRenderer.viewBoxF();
#endif

    rect.setWidth(rect.width() / scale);
    rect.setHeight(rect.height() / scale);
    return rect;
}

void CSizeHandleRect::setVisible(bool flag)
{
    m_bVisible = flag;
    if (parentItem() != nullptr /* && parentItem()->isSelected()*/) {
        m_bVisible ? show() : hide();
    }
}

void CSizeHandleRect::setJustExitLogicAbility(bool b)
{
    m_onlyLogicAblity = b;
    update();
}

QCursor CSizeHandleRect::getCursor()
{
    static bool init = false;
    static QPixmap m_RotateCursor;
    static QPixmap m_LeftTopCursor;
    static QPixmap m_RightTopCursor;
    static QPixmap m_LeftRightCursor;
    static QPixmap m_UpDownCursor;
    
    // 仅初始化一次光标图像
    if (!init) {
        qreal radio = qApp->devicePixelRatio();

        // 不同光标类型的SVG路径列表
        QStringList srcList;
        srcList << ":/theme/light/images/mouse_style/rotate_mouse.svg" 
                << ":/theme/light/images/mouse_style/icon_drag_leftup.svg"
                << ":/theme/light/images/mouse_style/icon_drag_rightup.svg" 
                << ":/theme/light/images/mouse_style/icon_drag_left.svg"
                << ":/theme/light/images/mouse_style/icon_drag_up.svg";

        QList<QPixmap> memberCursors;
        QSvgRenderer render;

        // 加载每个SVG并渲染为QPixmap
        for (int i = 0; i < srcList.size(); ++i) {
            auto srcPath = srcList.at(i);
            if (render.load(srcPath)) {
                QPixmap pix(QSize(24, 24) * radio);
                pix.fill(QColor(0, 0, 0, 0)); // 透明背景
                QPainter painter(&pix);
                render.render(&painter, QRect(QPoint(0, 0), pix.size()));
                memberCursors << pix;
            }
        }

        // 如果所有光标都加载成功，则分配给静态变量
        if (memberCursors.size() >= 5) {
            m_RotateCursor  = memberCursors.at(0);
            m_LeftTopCursor = memberCursors.at(1);
            m_RightTopCursor = memberCursors.at(2);
            m_LeftRightCursor = memberCursors.at(3);
            m_UpDownCursor = memberCursors.at(4);
        }

        init = true;
    }

    QCursor cursorResult(Qt::ArrowCursor);

    // Qt5 使用 QMatrix，Qt6 使用 QTransform
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QMatrix matrix;
#else
    QTransform matrix;
#endif

    // 从父项获取旋转角度
    CGraphicsItem *parent = dynamic_cast<CGraphicsItem *>(parentItem());
    qreal rotaAngle = (parent == nullptr ? 0 : parent->drawRotation());
    matrix.rotate(rotaAngle);

    // 根据方向确定光标类型并应用变换
    switch (m_dir) {
    case Right:
    case Left:
        cursorResult = QCursor(m_LeftRightCursor.transformed(matrix, Qt::SmoothTransformation));
        break;
    case Top:
    case Bottom:
        cursorResult = QCursor(m_UpDownCursor.transformed(matrix, Qt::SmoothTransformation));
        break;
    case RightTop:
    case LeftBottom:
        cursorResult = QCursor(m_RightTopCursor.transformed(matrix, Qt::SmoothTransformation));
        break;
    case LeftTop:
    case RightBottom:
        cursorResult = QCursor(m_LeftTopCursor.transformed(matrix, Qt::SmoothTransformation));
        break;

    case Rotation: {
        // 变换旋转光标并设置热点
        QPixmap pixmap = m_RotateCursor.transformed(matrix, Qt::SmoothTransformation);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        cursorResult = QCursor(pixmap); // Qt5: 直接使用QPixmap
#else
        cursorResult = QCursor(pixmap, pixmap.width() / 2, pixmap.height() / 2); // Qt6: 指定热点
#endif
        break;
    }
    default:
        break;
    }

    return cursorResult;
}

void CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::EDirection dir, bool &blockX, bool &blockY)
{
    blockX = false;
    blockY = false;
    switch (dir) {
    case Left:
    case Right: {
        blockY = true;
        break;
    }
    case Top:
    case Bottom: {
        blockX = true;
        break;
    }
    case LeftTop:
    case RightTop:
    case RightBottom:
    case LeftBottom:
    case Rotation:
    default:
        blockX = false;
        blockY = false;
        break;
    }
}

void CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::EDirection dir, bool &negtiveX, bool &negtiveY)
{
    negtiveX = false;
    negtiveY = false;
    switch (dir) {
    case Left:
    case LeftBottom:
        negtiveX = true;
        break;
    case Top:
    case RightTop:
        negtiveY = true;
        break;
    case LeftTop:
        negtiveX = true;
        negtiveY = true;
        break;
    default:
        negtiveX = false;
        negtiveY = false;
        break;
    }
}

QPointF CSizeHandleRect::transCenter(CSizeHandleRect::EDirection dir, CGraphicsItem *pItem)
{
    return pItem->getCenter(dir);
}
