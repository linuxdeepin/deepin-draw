#ifndef BORDERCOLORBUTTON_H
#define BORDERCOLORBUTTON_H

#include <QPushButton>
#include <QWidget>

class BorderColorButton : public QPushButton
{
    Q_OBJECT
public:
    BorderColorButton(QWidget* parent = 0);
    ~BorderColorButton();

    void setColor(QColor color);
    void setColorIndex(int index);
    void updateCheckedStatus();
    void updateConfigColor(const QString &group,  const QString &key);
    void resetChecked();

signals:
    void btnCheckStateChanged(bool checked);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent* );

private:
    QColor m_color;
    bool m_isHover;
    bool m_isChecked;
};

#endif // BORDERCOLORBUTTON_H
