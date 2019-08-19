#include "shapemimedata.h"
#include <QGraphicsItem>
#include "basegraphicsitem.h"

ShapeMimeData::ShapeMimeData(QList<QGraphicsItem *> items)
{
    foreach (QGraphicsItem *item , items )
    {
       AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
       m_shapeItems.append(sp->duplicate());
    }
}
ShapeMimeData::~ShapeMimeData()
{
    foreach (QGraphicsItem *item , m_shapeItems )
    {
        delete item;
        item =nullptr;
    }
    m_shapeItems.clear();
}

QList<QGraphicsItem *> ShapeMimeData::shapeItems() const
{
    return m_shapeItems;
}
