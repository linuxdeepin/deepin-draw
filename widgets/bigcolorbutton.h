#ifndef BIGCOLORBUTTON_H
#define BIGCOLORBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>

//#include "utils/baseutils.h"

class BigColorButton : public QPushButton
{
    Q_OBJECT
public:
    BigColorButton(QWidget *parent = nullptr);
    ~BigColorButton();

    void setColor(QColor color);
    void setColorIndex(int index);
    void updateConfigColor();
    void resetChecked();

signals:
    void btnCheckStateChanged(bool show);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent * );

private:
    QColor m_color;
    bool m_isHover;
    bool m_isChecked;


};

#endif // BIGCOLORBUTTON_H
