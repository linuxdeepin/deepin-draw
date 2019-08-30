#include "cgraphicsrectitem.h"
#include "csizehandlerect.h"
#include <QPainter>
#include <QPixmap>

CGraphicsRectItem::CGraphicsRectItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    initRect();
}

CGraphicsRectItem::CGraphicsRectItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    initRect();
}

CGraphicsRectItem::CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    initRect();
}

CGraphicsRectItem::~CGraphicsRectItem()
{

}

int CGraphicsRectItem::type() const
{
    return RectType;
}

void CGraphicsRectItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    updateGeometry();
}

void CGraphicsRectItem::initRect()
{
    // handles
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Rotation; ++i) {
        CSizeHandleRect *shr = nullptr;
        if (i == CSizeHandleRect::Rotation) {
            QPixmap rotaImage(":/theme/resources/icon_rotate.svg");
            shr   = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), this, rotaImage);
        } else {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), this);
        }
        m_handles.push_back(shr);

    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRect(rect());
}

void CGraphicsRectItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    QPointF local = mapFromScene(point);
    QRectF rect = this->rect();

    switch (dir) {
    case CSizeHandleRect::Right:
        if (local.x() - rect.left() > 0.1 ) {
            rect.setRight(local.x());
        }
        break;
    case CSizeHandleRect::RightTop:
        if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
            rect.setTopRight(local);
        }
        break;
    case CSizeHandleRect::RightBottom:
        if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
            rect.setBottomRight(local);
        }
        break;
    case CSizeHandleRect::LeftBottom:
        if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
            rect.setBottomLeft(local);
        }
        break;
    case CSizeHandleRect::Bottom:
        if (local.y() - rect.top() > 0.1 ) {
            rect.setBottom(local.y());
        }
        break;
    case CSizeHandleRect::LeftTop:
        if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
            rect.setTopLeft(local);
        }
        break;
    case CSizeHandleRect::Left:
        if (local.x() - rect.right() < 0.1 ) {
            rect.setLeft(local.x());
        }
        break;
    case CSizeHandleRect::Top:
        if (local.y() - rect.bottom() < 0.1 ) {
            rect.setTop(local.y());
        }
        break;
    default:
        break;
    }

    rect = rect.normalized();
    prepareGeometryChange();

    this->setRect(rect);
    updateGeometry();

}


void CGraphicsRectItem::updateGeometry()
{
    const QRectF &geom = this->rect();

    const int w = SELECTION_HANDLE_SIZE;
    const int h = SELECTION_HANDLE_SIZE;

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;
        switch (hndl->dir()) {
        case CSizeHandleRect::LeftTop:
            hndl->move(geom.x() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Top:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::RightTop:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Right:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::RightBottom:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Bottom:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::LeftBottom:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Left:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::Rotation:
            hndl->move(geom.x() + geom.width() / 2 - hndl->rect().width() / 2, geom.y() - 25 - hndl->rect().height() / 2);
            break;
        default:
            break;
        }
    }
}

QRectF CGraphicsRectItem::rect() const
{
    return QRectF(m_topLeftPoint, m_bottomRightPoint);
}

QRectF CGraphicsRectItem::boundingRect() const
{
    QRectF rect = this->rect();
    QRectF bounding = QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
                             rect.width() + pen().width(), rect.height() + pen().width());
    return bounding;
}

//void CGraphicsRectItem::setState(SelectionHandleState st)
//{
//    const Handles::iterator hend =  m_handles.end();
//    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
//        (*it)->setState(st);
//    }
//}
