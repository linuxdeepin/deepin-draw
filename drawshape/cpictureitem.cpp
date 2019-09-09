#include "cpictureitem.h"

#include <QPainter>
#include <QDebug>

CPictureItem::CPictureItem(const QPixmap &pixmap, CGraphicsItem *parent )
    : CGraphicsRectItem(parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
{

}


CPictureItem::CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent )
    : CGraphicsRectItem(rect, parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
{

}

CPictureItem::~CPictureItem()
{

}

int  CPictureItem::type() const
{
    return PictureType;
}

void CPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    Q_UNUSED(option)
    Q_UNUSED(widget)
    //获取原始图片大小
    QRectF pictureRect = QRectF(0, 0, m_pixmap.width(), m_pixmap.height());
    painter->drawPixmap(rect(), m_pixmap, pictureRect);

}

void CPictureItem::setAngle(const qreal &angle)
{
    m_angle = angle;
}

void CPictureItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

//进行翻转，先转化为qimage，翻转后转化为qpixmap
void CPictureItem::setMirror(bool hor, bool ver)
{
    //qDebug() << "entered the  setMirror function" << endl;
    QImage image = m_pixmap.toImage();
    QImage mirrorImage = image.mirrored(hor, ver);
    m_pixmap = QPixmap::fromImage(mirrorImage);
    update();
}


void CPictureItem::setRotation90(bool leftOrRight)
{
    //旋转图形 有BUG
    QPointF center = this->rect().center();
    this->setTransformOriginPoint(center);
    if (leftOrRight == true) {
        m_angle = m_angle - 90.0;
        this->setRotation(m_angle);
    } else {
        m_angle = m_angle + 90.0;
        this->setRotation(m_angle);
    }
}

CGraphicsItem *CPictureItem::duplicate() const
{
    CPictureItem *item = new CPictureItem(rect(), m_pixmap);

    item->setAngle(m_angle);

    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue() + 0.1);
    return item;
}


