#ifndef COLORSLIDER_H
#define COLORSLIDER_H

#include <QSlider>
#include <QPaintEvent>

class ColorSlider : public QSlider {
    Q_OBJECT
public:
    ColorSlider(QWidget* parent = 0);
    ~ColorSlider();

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);

protected:
    void paintEvent(QPaintEvent *ev);

private:
    int m_value;
};

#endif // COLORSLIDER_H
