#ifndef SHAPEMIMEDATA_H
#define SHAPEMIMEDATA_H

#include <QMimeData>


class QGraphicsItem;

class ShapeMimeData : public QMimeData
{
    Q_OBJECT
public:
    ShapeMimeData( QList<QGraphicsItem * > items);
    ~ShapeMimeData();
    QList<QGraphicsItem *> shapeItems() const ;
private:
    QList<QGraphicsItem * > m_shapeItems;
};
#endif // SHAPEMIMEDATA_H
