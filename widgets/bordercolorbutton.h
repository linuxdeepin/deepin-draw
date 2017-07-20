#ifndef BORDERCOLORBUTTON_H
#define BORDERCOLORBUTTON_H

#include <QPushButton>
#include <QWidget>

class BorderColorButton : public QPushButton {
    Q_OBJECT
public:
    BorderColorButton(QWidget* parent = 0);
    ~BorderColorButton();

    void setColor(QColor color);
    void setColorIndex();
    void setCheckedStatus(bool checked);
    void updateConfigColor(const QString &shape,  const QString &key, int index);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QColor m_color;
    bool m_isHover;
    bool m_isChecked;
};

#endif // BORDERCOLORBUTTON_H
