#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QPushButton>
#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QApplication>

#include "sliderlabel.h"
#include "editlabel.h"
#include "pushbutton.h"
#include "utils/baseutils.h"

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

    void updateColorButtonStatus();

    void setColor(QColor color);
    void setDrawStatus(DrawStatus status);

signals:
    void colorChanged(QColor color);
    void updateHeight();

private:
    SliderLabel* m_sliderLabel;
    EditLabel* m_editLabel;
    PushButton* m_colorfulBtn;

    QStringList m_colList;
    QList<ColorButton*> m_cButtonList;
    DrawStatus m_drawstatus;
    bool m_expand;
};

#endif // COLORPANEL_H
