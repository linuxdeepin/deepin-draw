#ifndef CPICTUREITEM_H
#define CPICTUREITEM_H

#include <QGraphicsPixmapItem>


class CPictureItem : public QGraphicsPixmapItem
{
public:
    explicit CPictureItem(QGraphicsPixmapItem *parent = nullptr);
    explicit CPictureItem(const QPixmap &pixmap, QGraphicsPixmapItem *parent = nullptr);
    virtual ~CPictureItem()  Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;

    //QGraphicsPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr)

//    explicit CPictureItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
//    explicit CPictureItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
//    virtual ~CPictureItem()  Q_DECL_OVERRIDE;
//    virtual int  type() const Q_DECL_OVERRIDE;
//    /**
//     * @brief setRect 创建矩形时，用于设置矩形大小
//     * @param rect
//     */
//    void setRect(const QRectF &rect);
//    virtual QRectF rect() const Q_DECL_OVERRIDE;
//    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
//    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;

//protected:
//    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
//    virtual void updateGeometry() Q_DECL_OVERRIDE;

//private:
//    /**
//     * @brief initRect 初始化矩形的属性和边框小方块
//     */
//    void initRect();

//private:
//    QPointF m_topLeftPoint; //左上角的点
//    QPointF m_bottomRightPoint; //右下角的点
//    ESelectionHandleState m_state; //边框选中状态
};

#endif // CPICTUREITEM_H
