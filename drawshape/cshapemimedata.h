#ifndef CSHAPEMIMEDATA_H
#define CSHAPEMIMEDATA_H

#include <QGraphicsItem>
#include <QMimeData>

class CGraphicsItem;

class CShapeMimeData : public QMimeData
{
public:
    CShapeMimeData(QList<QGraphicsItem *> items);
    ~CShapeMimeData();
    QList<CGraphicsItem *> itemList() const;

private:
    QList<CGraphicsItem *> m_itemList;
};

#endif // CSHAPEMIMEDATA_H
