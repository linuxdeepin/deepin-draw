#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QCursor>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:
    ColorLabel(QWidget* parent = 0);
    ~ColorLabel();

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);
    void setHue(int hue);

    void pickColor(QPoint pos, bool picked = false);
    QColor getPickedColor();

signals:
    void clicked();
    void pickedColor(QColor color);

protected:
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent* e);
    void leaveEvent(QEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

private:
    //calculate the color's rgb value in pos.
    QColor calColorAtPoint(QPointF pos);
    QCursor m_lastCursor;
    int m_hue =  0;
    bool m_picking;
    bool m_pressed;
    QColor m_pickedColor;
    QPoint m_clickedPos;
    QPoint m_tipPoint;
};

#endif // COLORLABEL_H
