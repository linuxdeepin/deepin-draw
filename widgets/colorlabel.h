#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QPaintEvent>

class ColorLabel : public QLabel {
    Q_OBJECT
public:
    ColorLabel(QWidget* parent = 0);
    ~ColorLabel();

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);
    void setHue(int hue);

protected:
    void paintEvent(QPaintEvent *);

private:
    //calculate the color's rgb value in pos.
    QColor calColorAtPoint(QPointF pos);

    int m_hue =  0;
};

#endif // COLORLABEL_H
