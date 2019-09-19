#ifndef TEXTCOLORBUTTON_H
#define TEXTCOLORBUTTON_H

#include <DPushButton>
#include <QPainter>
#include <QPaintEvent>

DWIDGET_USE_NAMESPACE

class TextColorButton : public DPushButton
{
    Q_OBJECT
public:
    TextColorButton(DWidget *parent = nullptr);
    ~TextColorButton();

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

#endif // TEXTCOLORBUTTON_H
