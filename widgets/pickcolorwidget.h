#ifndef PICKCOLORWIDGET_H
#define PICKCOLORWIDGET_H

#include <DWidget>

#include "editlabel.h"
#include "pushbutton.h"
#include "colorlabel.h"
#include "colorslider.h"

DWIDGET_USE_NAMESPACE

class PickColorWidget : public DWidget
{
    Q_OBJECT
public:
    PickColorWidget(DWidget *parent);
    ~PickColorWidget();

    void setRgbValue(QColor color, bool isPicked = false);
    void updateColor();
    void setPickedColor(bool picked);

signals:
    void pickedColor(QColor color);

private:
    EditLabel *m_redEditLabel;
    EditLabel *m_greenEditLabel;
    EditLabel *m_blueEditLabel;
    PushButton *m_picker;
    ColorLabel *m_colorLabel;
    ColorSlider *m_colorSlider;
};

#endif // PICKCOLORWIDGET_H
