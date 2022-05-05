#include "colorstylewidget.h"
#include "colorsettingbutton.h"


ColorStyleWidget::ColorStyleWidget(QWidget *parent): AttributeWgt(-1, parent)
{
    initUi();
}

void ColorStyleWidget::setVar(const QVariant &var)
{
    if (m_bEnableAttriVisible) {
        QVariantList l = var.toList();
        bool bApply = true;
        QColor c(Qt::transparent);
        if (1 == l.size()) {
            c = var.value<QColor>();
        } else if (2 == l.size()) {
            bApply = l.at(0).toBool();
            c = l.at(1).value<QColor>();
        }
        m_fillColor->setVar(c);
        m_fillColorEdit->setText(c.name());
        m_bEnableAtrri->setChecked(bApply);
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

void ColorStyleWidget::setEnableAttriVisible(bool bVisible)
{
    m_bEnableAtrri->setVisible(bVisible);
    m_bEnableAttriVisible = bVisible;
}

bool ColorStyleWidget::isEnableAttriVisible()
{
    return m_bEnableAttriVisible;
}

bool ColorStyleWidget::isAttriApply()
{
    return m_bEnableAtrri->isChecked();
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
    QHBoxLayout *hFillColor = new QHBoxLayout(this);
    hFillColor->setMargin(0);
    hFillColor->setContentsMargins(0, 0, 0, 0);
    hFillColor->addWidget(m_fillColor, 3);
    hFillColor->addWidget(m_fillColorEdit, 2);
    fillLayout->addWidget(m_bEnableAtrri);

    QHBoxLayout *titleLayout = new QHBoxLayout(this);
    m_titleLabel = new DLabel(tr("fill"));
    titleLayout->addWidget(m_titleLabel);
    m_bEnableAtrri = new DCheckBox;
    m_bEnableAtrri->setChecked(true);
    titleLayout->addWidget(m_bEnableAtrri);
    titleLayout->setAlignment(m_titleLabel, Qt::AlignLeft);
    titleLayout->setAlignment(m_bEnableAtrri, Qt::AlignRight);

    fillLayout->addLayout(titleLayout);
    fillLayout->addLayout(hFillColor);
    setLayout(fillLayout);
    m_fillColor->show();

    connect(m_bEnableAtrri, &DCheckBox::stateChanged, this, [ = ](int state) {
        Q_UNUSED(state)

        enableColorEdit(m_bEnableAtrri->isChecked());
        emit colorChanged(m_fillColor->getColor(), EChanged);
    });

    connect(m_fillColor, &ColorSettingButton::colorChanged, this, [ = ](const QColor & _t1, int _t2) {
        m_fillColorEdit->setText(_t1.name());
        emit colorChanged(_t1, _t2);
    });
}

void ColorStyleWidget::enableColorEdit(bool bEnable)
{
    m_fillColorEdit->setEnabled(bEnable);
    m_fillColor->setEnabled(bEnable);
}
