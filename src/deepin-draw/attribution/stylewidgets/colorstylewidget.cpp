#include "colorstylewidget.h"
#include "colorsettingbutton.h"


ColorStyleWidget::ColorStyleWidget(QWidget *parent): AttributeWgt(-1, parent)
{
    initUi();
}

void ColorStyleWidget::setVar(const QVariant &var)
{
    QVariantList l = var.toList();
    QColor c = var.value<QColor>();

    m_fillColor->setVar(c);
    m_fillColorEdit->setText(c.name());
    if (!var.isValid()) {
        m_fillColorEdit->setText("#...");
    }
}

void ColorStyleWidget::setTitleText(QString text)
{
    m_titleLabel->setText(text);
}

void ColorStyleWidget::setColorFill(int fillStyle)
{
    m_fillColor->setColorFill(static_cast<CColorSettingButton::EColorFill>(fillStyle));
}

void ColorStyleWidget::setColorTextVisible(bool bVisible)
{
    m_fillColorEdit->setVisible(bVisible);
}

void ColorStyleWidget::addWidget(QWidget *w)
{
    m_lFillColor->addWidget(w);
    m_addWidgets << w;
}

void ColorStyleWidget::addTitleWidget(QWidget *w, Qt::Alignment alignment)
{
    m_titleLayout->addWidget(w);
    m_titleLayout->setAlignment(w, alignment);
}

void ColorStyleWidget::setContentEnable(bool enable)
{
    m_fillColor->setEnabled(enable);
    m_fillColorEdit->setEnabled(enable);
}

void ColorStyleWidget::initUi()
{
    m_fillColor = new CColorSettingButton(tr("color"), this, false);
    m_fillColor->setColorFill(CColorSettingButton::EFillArea);
    m_fillColorEdit = new DLineEdit(this);
    m_fillColorEdit->setClearButtonEnabled(false);
    QVBoxLayout *fillLayout = new QVBoxLayout(this);
    fillLayout->setMargin(0);
    fillLayout->setContentsMargins(0, 0, 0, 0);
    m_lFillColor = new QHBoxLayout(this);
    m_lFillColor->setMargin(0);
    m_lFillColor->setContentsMargins(0, 0, 0, 0);
    m_lFillColor->addWidget(m_fillColor, 3);
    m_lFillColor->addWidget(m_fillColorEdit, 2);

    m_titleLayout = new QHBoxLayout(this);
    m_titleLabel = new DLabel(tr("fill"));
    m_titleLayout->addWidget(m_titleLabel);
    m_titleLayout->setAlignment(m_titleLabel, Qt::AlignLeft);

    fillLayout->addLayout(m_titleLayout);
    fillLayout->addLayout(m_lFillColor);
    setLayout(fillLayout);
    m_fillColor->show();

    connect(m_fillColor, &ColorSettingButton::colorChanged, this, [ = ](const QColor & _t1, int _t2) {
        m_fillColorEdit->setText(_t1.name());
        emit colorChanged(_t1, _t2);
    });
}

void ColorStyleWidget::enableColorEdit(bool bEnable)
{
    m_fillColorEdit->setEnabled(bEnable);
    m_fillColor->setEnabled(bEnable);
    for (QWidget *w : m_addWidgets) {
        w->setEnabled(bEnable);
    }
}
