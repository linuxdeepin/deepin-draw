#ifndef PICKCOLORWIDGET_H
#define PICKCOLORWIDGET_H

#include <QWidget>

#include "editlabel.h"
#include "pushbutton.h"
#include "colorlabel.h"
#include "colorslider.h"

class PickColorWidget : public QWidget
{
    Q_OBJECT
public:
    PickColorWidget(QWidget* parent);
    ~PickColorWidget();

    void setRgbValue(QColor color);
    void updateColor();

signals:
    void pickedColor(QColor color);

private:
    EditLabel* m_redEditLabel;
    EditLabel* m_greenEditLabel;
    EditLabel* m_blueEditLabel;
    PushButton* m_picker;
    ColorLabel* m_colorLabel;
    ColorSlider* m_colorSlider;
};

#endif // PICKCOLORWIDGET_H
