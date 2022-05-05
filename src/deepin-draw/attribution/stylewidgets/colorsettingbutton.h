#ifndef COLORSETTINGBUTTON_H
#define COLORSETTINGBUTTON_H
#include "attributewidget.h"
class ColorPickWidget;
class CColorSettingButton: public ColorSettingButton
{
    Q_OBJECT
public:
    using ColorSettingButton::ColorSettingButton;
    ColorPickWidget *colorPick();
protected:
    void  mousePressEvent(QMouseEvent *event) override;
private:
    bool        _connectedColorPicker = false;

};


#endif // COLORSETTINGBUTTON_H
