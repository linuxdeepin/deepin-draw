#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QPushButton>
#include <QWidget>
#include <QColor>
#include <QPaintEvent>

#include "sliderlabel.h"
#include "editlabel.h"
#include "pushbutton.h"

class ColorButton : public QPushButton {
    Q_OBJECT
public:
    ColorButton(const QColor &color, QWidget* parent = 0);
    ~ColorButton();

    void setDisableColor(bool disable);

signals:
    void colorButtonClicked(QColor color);

protected:
    void paintEvent(QPaintEvent *);

private:
    QColor m_color;
    bool      m_disable;
};

class ColorPanel : public QWidget {
    Q_OBJECT
public:
    ColorPanel(QWidget* parent = 0);
    ~ColorPanel();

signals:
    void colorChanged(QColor color);

private:
    SliderLabel* m_sliderLabel;
    EditLabel* m_editLabel;
    PushButton* m_colorfulBtn;
};

#endif // COLORPANEL_H
