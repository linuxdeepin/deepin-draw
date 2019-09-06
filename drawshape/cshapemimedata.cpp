#include "cshapemimedata.h"
#include "cgraphicsitem.h"

CShapeMimeData::CShapeMimeData(QList<QGraphicsItem *> items)
{
    foreach (QGraphicsItem *item, items ) {
        CGraphicsItem *copy = static_cast<CGraphicsItem *>(item)->duplicate();
        if (copy) {
            m_itemList.append(copy);
        }
    }
}
CShapeMimeData::~CShapeMimeData()
{
    foreach (QGraphicsItem *item, m_itemList ) {
        delete item;
        item = nullptr;
    }
    m_itemList.clear();
}

QList<CGraphicsItem *> CShapeMimeData::itemList() const
{
    return m_itemList;
}



