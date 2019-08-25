#ifndef CGRAPHICRECTITEM_H
#define CGRAPHICRECTITEM_H
#include <QtGlobal>
#include <QGraphicsRectItem>
class CGraphicsRectItem : public QGraphicsRectItem
{
public:
    explicit CGraphicsRectItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsRectItem(const QRectF &rect, QGraphicsItem *parent = nullptr);
    explicit CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = nullptr);
    ~CGraphicsRectItem()  override;

private:
    void setProperty();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

#endif // CGRAPHICRECTITEM_H
