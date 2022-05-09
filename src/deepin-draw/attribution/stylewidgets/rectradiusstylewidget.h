#ifndef ROUNDEDANGLESTRYLEWIDGET_H
#define ROUNDEDANGLESTRYLEWIDGET_H
#include "attributewidget.h"
#include <DLabel>
#include <DWidget>
#include <DSpinBox>
#include <DSlider>
DWIDGET_USE_NAMESPACE

class CSpinBox;
class NumberSlider;

class BackgroundWidget : public QWidget
{
public:
    using QWidget::QWidget;
    void setBackgroudColor(QColor c);
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QColor m_bkColor = Qt::transparent;
};

class RectRadiusStyleWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit RectRadiusStyleWidget(QWidget *parent = nullptr);
    void setVar(const QVariant &var) override;
private:
    void initUi();
    void initConnect();
    void emitValueChange(int left, int right, int leftBottom, int rightBottom);
    void setSpinBoxValue(CSpinBox *s, int value);
signals:
    void valueChanged(QVariant value, EChangedPhase phase);
private:
    QButtonGroup *m_checkgroup = nullptr;
    CSpinBox    *m_left = nullptr;
    CSpinBox    *m_right = nullptr;
    CSpinBox    *m_leftBottom = nullptr;
    CSpinBox    *m_rightBottom = nullptr;
    DSlider    *m_radiusSlider = nullptr;
    CSpinBox        *m_radius = nullptr;
    DIconButton *m_sameRadiusButton = nullptr;
    DIconButton *m_diffRadiusButton = nullptr;
    BackgroundWidget     *m_diffRadiusWidget = nullptr;
    DWidget     *m_sameRadiusWidget = nullptr;
};

#endif // ROUNDEDANGLESTRYLEWIDGET_H
