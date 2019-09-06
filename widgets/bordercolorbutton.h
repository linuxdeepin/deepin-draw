#ifndef BORDERCOLORBUTTON_H
#define BORDERCOLORBUTTON_H

#include <DPushButton>
#include <QWidget>

DWIDGET_USE_NAMESPACE

class BorderColorButton : public DPushButton
{
    Q_OBJECT
public:
    BorderColorButton(QWidget *parent = nullptr);
    ~BorderColorButton();

    void setColor(QColor color);
    void setColorIndex(int index);
    void updateCheckedStatus();
    void updateConfigColor();
    void resetChecked();

signals:
    void btnCheckStateChanged(bool checked);

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

#endif // BORDERCOLORBUTTON_H
