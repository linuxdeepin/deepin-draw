#ifndef CGRAPHICSROTATEANGLEITEM_H
#define CGRAPHICSROTATEANGLEITEM_H

#include <QGraphicsRectItem>

class CGraphicsRotateAngleItem : public QGraphicsRectItem
{
public:
    explicit CGraphicsRotateAngleItem(qreal rotateAngle, QGraphicsItem *parent = nullptr);
    explicit CGraphicsRotateAngleItem(const QRectF &rect, qreal rotateAngle, QGraphicsItem *parent = nullptr);

    void updateRotateAngle(qreal rotateAngle);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

private:
    qreal m_rotateAngle;
    int m_width;
    int m_height;
};

#endif // CGRAPHICSROTATEANGLEITEM_H
