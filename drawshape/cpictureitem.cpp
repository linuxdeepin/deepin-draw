#include "cpictureitem.h"
#include "globaldefine.h"

CPictureItem::CPictureItem(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{

}


CPictureItem::CPictureItem(const QPixmap &pixmap, QGraphicsPixmapItem *parent )
    : QGraphicsPixmapItem(parent)
{

}

CPictureItem::~CPictureItem()
{

}

int  CPictureItem::type() const
{
    return PictureType;
}
