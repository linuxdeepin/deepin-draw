#ifndef BIGCOLORBUTTON_H
#define BIGCOLORBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>

#include "utils/baseutils.h"

class BigColorButton : public QPushButton
{
    Q_OBJECT
public:
    BigColorButton(QWidget* parent = 0);
    ~BigColorButton();

    void setColor(QColor color);
    void setColorIndex(int index);
    void setCheckedStatus(bool checked);
    void updateConfigColor(const QString &group,  const QString &key);

signals:
    void btnCheckStateChanged(bool show);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QColor m_color;
    bool m_isHover;
    bool m_isChecked;
};

#endif // BIGCOLORBUTTON_H
