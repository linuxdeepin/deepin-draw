#ifndef CGRAPHICSITEMGROUP_H
#define CGRAPHICSITEMGROUP_H

#include <QGraphicsItemGroup>
class CGraphicsItemGroup : public QGraphicsItemGroup
{
public:
    CGraphicsItemGroup();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};
#endif // CGRAPHICSITEMGROUP_H
