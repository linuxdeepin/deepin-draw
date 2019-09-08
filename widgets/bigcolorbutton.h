#ifndef BIGCOLORBUTTON_H
#define BIGCOLORBUTTON_H

#include <DPushButton>
#include <QPainter>
#include <QPaintEvent>

DWIDGET_USE_NAMESPACE

class BigColorButton : public DPushButton
{
    Q_OBJECT
public:
    BigColorButton(DWidget *parent = nullptr);
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
