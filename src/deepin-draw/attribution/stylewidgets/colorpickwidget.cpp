#include "colorpickwidget.h"
#include "attributewidget.h"
#include "ccolorpanel.h"

ColorPickWidget::ColorPickWidget(QWidget *parent): DArrowRectangle(ArrowTop, parent)
{
    setWindowFlags(Qt::Popup);
    setFocusPolicy(Qt::NoFocus);
    setRadius(20);

    m_colorSettingWgt = new ColorSettingWgt(this);
    m_colorSettingWgt->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    l->setMargin(20);
    l->addWidget(m_colorSettingWgt);
    m_colorSettingWgt->show();
    setLayout(l);
    //setContent(m_colorSettingWgt);
    setRadiusArrowStyleEnable(true);
    connect(m_colorSettingWgt, &ColorSettingWgt::colorChanged, this, [ = ](const QColor & color, int phase) {
        emit colorChanged(color, phase);
    });

}

void ColorPickWidget::setVar(const QVariant &var)
{
    m_colorSettingWgt->setVar(var);
}

QWidget *ColorPickWidget::caller()
{
    return m_caller;
}

void ColorPickWidget::setCaller(QWidget *pCaller)
{
    m_caller = pCaller;
}

void ColorPickWidget::setColor(const QColor &color, int phase)
{
    m_colorSettingWgt->setColor(color, phase);
}

void ColorPickWidget::paintEvent(QPaintEvent *e)
{
    resize(sizeHint());
    DArrowRectangle::paintEvent(e);
}


