#ifndef CBUTTONRECT_H
#define CBUTTONRECT_H

#include <QGraphicsRectItem>
#include <QTextOption>



class CButtonRect : public QGraphicsRectItem
{
public:

    enum EButtonType {OKButton, CancelButton, NoneButton};

    CButtonRect(QGraphicsItem *parent,  EButtonType type);
    ~CButtonRect() Q_DECL_OVERRIDE;
    CButtonRect::EButtonType buttonType() const;
    void move(qreal x, qreal y );
    bool hitTest(const QPointF &point);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
//    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
private:
    EButtonType m_buttonType;
    QString m_text;
    QColor m_backColor;

};

#endif // CBUTTONRECT_H
