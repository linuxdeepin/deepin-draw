#include "cgraphicscutitem.h"
#include "csizehandlerect.h"



#include <QPushButton>
#include <QPainter>
#include <QGraphicsProxyWidget>


const int CORNER_WITH = 20;
const QString PenColorName = "#B5B5B5";


CGraphicsCutItem::CGraphicsCutItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    initPenAndBrush();
    initRect();
}

CGraphicsCutItem::CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    initPenAndBrush();
    initRect();
}

CGraphicsCutItem::CGraphicsCutItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    initPenAndBrush();
    initRect();
}

CGraphicsCutItem::~CGraphicsCutItem()
{

}

int CGraphicsCutItem::type() const
{
    return CutType;
}

void CGraphicsCutItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    updateGeometry();
}

void CGraphicsCutItem::initRect()
{

//    m_cancelBtn = new QPushButton();
//    m_cancelBtn->setText("Cancel");
//    m_cancelBtn->setFixedSize(40, 20);

//    m_proxy = new QGraphicsProxyWidget(this);
//    m_proxy->setWidget(m_cancelBtn);

    m_okBtn = new CButtonRect(this, CButtonRect::OKButton);
    m_cancelBtn = new CButtonRect(this, CButtonRect::CancelButton);


    // handles
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Left; ++i) {
        CSizeHandleRect *shr = nullptr;
        shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        m_handles.push_back(shr);
    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsCutItem::initPenAndBrush()
{
    QPen pen;
    pen.setColor(QColor(PenColorName));
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);

    setPen(pen);
    setBrush(Qt::NoBrush);
}

void CGraphicsCutItem::updateGeometry()
{
    const QRectF &geom = this->boundingRect();

    m_cancelBtn->move(geom.x() + geom.width() - 120, geom.y() + geom.height() - 20);
    m_okBtn->move(geom.x() + geom.width() - 50, geom.y() + geom.height() - 20);

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;
        qreal w = hndl->boundingRect().width();
        qreal h = hndl->boundingRect().height();
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
        default:
            break;
        }
    }
}

QRectF CGraphicsCutItem::rect() const
{
    return QRectF(m_topLeftPoint, m_bottomRightPoint);
}

QRectF CGraphicsCutItem::boundingRect() const
{
    QRectF rect = this->rect();
    QRectF bounding = QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
                             rect.width() + pen().width(), rect.height() + pen().width());
    return bounding;
}

void CGraphicsCutItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    QPointF local = mapFromScene(point);
    QRectF rect = this->rect();
    QPointF centerPoint = rect.center();


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
        if (rect.right() - local.x() > 0.1 ) {
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
    centerPoint = mapToScene(rect.center());
    rect.setRect(centerPoint.rx() - rect.width() * 0.5, centerPoint.ry() - rect.height() * 0.5, rect.width(), rect.height());
    prepareGeometryChange();
    setTransformOriginPoint(centerPoint);
    this->setPos(0, 0);

    this->setRect(rect);
    updateGeometry();
}

void CGraphicsCutItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(dir)
    Q_UNUSED(point)
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
}

void CGraphicsCutItem::showControlRects(bool flag)
{
    setState(flag ? SelectionHandleActive : SelectionHandleOff);
}

CButtonRect::EButtonType CGraphicsCutItem::collideTest(const QPointF &point) const
{
    if (m_okBtn->hitTest(point)) {
        return m_okBtn->buttonType();
    }

    if (m_cancelBtn->hitTest(point)) {
        return m_cancelBtn->buttonType();
    }

    return  CButtonRect::NoneButton;
}

QVariant CGraphicsCutItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    Q_UNUSED(change);
    return value;
}

void CGraphicsCutItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(PenColorName));
    painter->setPen(pen);

    QPainterPath path;
    ///画4个角
    pen.setColor(Qt::white);
    pen.setWidth(5);
    painter->setPen(pen);
    drawFourConner(painter, path, pen.width());

    ///画三等分矩形的直线
    pen.setColor(QColor(PenColorName));
    painter->setPen(pen);
    pen.setWidth(1);
    painter->setPen(pen);
    drawTrisectorRect(painter, path);

    ///画矩形
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect());
}

void CGraphicsCutItem::drawTrisectorRect(QPainter *painter, QPainterPath &path)
{
    path.moveTo(rect().x(), rect().y() + rect().height() / 3);
    path.lineTo(rect().x() + rect().width(), rect().y() + rect().height() / 3);

    path.moveTo(rect().x(), rect().y() + rect().height() / 3 * 2);
    path.lineTo(rect().x() + rect().width(), rect().y() + rect().height() / 3 * 2);

    path.moveTo(rect().x() + rect().width() / 3, rect().y() );
    path.lineTo(rect().x() + rect().width() / 3, rect().y() + rect().height() );

    path.moveTo(rect().x() + rect().width() / 3 * 2, rect().y() );
    path.lineTo(rect().x() + rect().width() / 3 * 2, rect().y() + rect().height() );

    painter->drawPath(path);
}

void CGraphicsCutItem::drawFourConner(QPainter *painter, QPainterPath &path, const int penWidth)
{
    //左上角
    path.moveTo(rect().x() + penWidth / 2, rect().y() + CORNER_WITH);
    path.lineTo(rect().x() + penWidth / 2, rect().y()  + penWidth / 2);
    path.lineTo(rect().x() + CORNER_WITH, rect().y()  + penWidth / 2);
    //右上角
    path.moveTo(rect().x() + rect().width() - CORNER_WITH, rect().y() + penWidth / 2);
    path.lineTo(rect().x() + rect().width() - penWidth / 2, rect().y()  + penWidth / 2);
    path.lineTo(rect().x() + rect().width() - penWidth / 2, rect().y() + CORNER_WITH);
    //右下角
    path.moveTo(rect().x() + rect().width() - penWidth / 2, rect().y() + rect().height() - CORNER_WITH);
    path.lineTo(rect().x() + rect().width() - penWidth / 2, rect().y()  + rect().height() - penWidth / 2);
    path.lineTo(rect().x() + rect().width() - CORNER_WITH, rect().y() + rect().height() - penWidth / 2);

    //左下角
    path.moveTo(rect().x() + CORNER_WITH + penWidth / 2, rect().y() + rect().height() - penWidth / 2);
    path.lineTo(rect().x() + penWidth / 2, rect().y()  + rect().height() - penWidth / 2);
    path.lineTo(rect().x() + penWidth / 2, rect().y() + rect().height() - CORNER_WITH);

    painter->drawPath(path);
}
