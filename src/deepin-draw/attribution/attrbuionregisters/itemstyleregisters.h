#ifndef COMMONSTYLEATTRIBUTIONREGISTERS_H
#define COMMONSTYLEATTRIBUTIONREGISTERS_H

#include <QStyleOptionMenuItem>
#include <QModelIndex>
#include "iattributionregister.h"

class DrawBoard;
class ColorStyleWidget;
class SideWidthWidget;
class RectRadiusStyleWidget;
class CSpinBox;
class QComboBox;
class QToolButton;

class SliderSpinBoxWidget;
class ComboBoxSettingWgt;
class CheckBoxSettingWgt;
class HBoxLayoutWidget;
class CutAttributionWidget;

#define OVERRIDE_IATTRIEGETSTER_FUNCTION      public: \
    using IAttributionRegister::IAttributionRegister; \
    virtual void registe() override;

class AdjustmentAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
};


class StarAnchorAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
    inline SliderSpinBoxWidget *attriWidget() {return m_starAnchorAttri;}
private:
    SliderSpinBoxWidget *m_starAnchorAttri = nullptr;
};

class StartAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
    inline SliderSpinBoxWidget *attriWidget() {return m_starRadioAttri;}
private:
    SliderSpinBoxWidget *m_starRadioAttri = nullptr;
};

class PolygonSidesAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
    inline SliderSpinBoxWidget *attriWidget() {return m_polygonSidesAttri;}
private:
    SliderSpinBoxWidget *m_polygonSidesAttri = nullptr;
};

class LineAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
    inline HBoxLayoutWidget *attriWidget() {return m_streakStyle;}
private:
    HBoxLayoutWidget *m_streakStyle = nullptr;
    QComboBox *m_comboxstart = nullptr;
    QComboBox *m_comboxend = nullptr;
};

class EraserAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
private:
    SliderSpinBoxWidget *m_eraserAttri = nullptr;
};

class BlurAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
public:
    inline HBoxLayoutWidget *attriWidget() {return m_blurStyle;}
private:
    HBoxLayoutWidget *m_blurStyle = nullptr;
    QToolButton *m_blurEffect = nullptr;           //模糊
    QToolButton *m_masicoEffect = nullptr;           //高斯模糊
    SliderSpinBoxWidget *m_blurAttri = nullptr;
};

class CutAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
private:
    CutAttributionWidget *m_cutAttri = nullptr;     //裁剪属性
};


class PenAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
    inline ColorStyleWidget *attriWidget() {return m_PenBrushStyle;}
    inline ComboBoxSettingWgt *penStyleSettingWgt() {return m_penStyle;}
private:
    ColorStyleWidget *m_penAttri = nullptr;
    SliderSpinBoxWidget *m_sliderPenWidth = nullptr;
    ComboBoxSettingWgt  *m_penStyle = nullptr;
    ColorStyleWidget    *m_PenBrushStyle = nullptr;
    CheckBoxSettingWgt  *m_enablePenStyle = nullptr;
};

class RectAttriRegister : public IAttributionRegister
{
    Q_OBJECT
    OVERRIDE_IATTRIEGETSTER_FUNCTION
    inline RectRadiusStyleWidget *attriWidget() {return m_rectRadius;}
private:
    RectRadiusStyleWidget *m_rectRadius = nullptr;
};
#endif // COMMONSTYLEATTRIBUTIONREGISTERS_H
