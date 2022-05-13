#include <DPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include "adjustmentatrriwidget.h"
#include "boxlayoutwidget.h"

AdjustmentAtrriWidget::AdjustmentAtrriWidget(QWidget *parent) : AttributeWgt(EImageAdaptScene, parent)
{
    initUi();
}

DToolButton *AdjustmentAtrriWidget::button()
{
    return  m_adjustmentBtn;
}

void AdjustmentAtrriWidget::setVar(const QVariant &var)
{
    bool bEnable = var.toBool();
    m_titleLabel->setEnabled(bEnable);
    m_adjustmentBtn->setEnabled(bEnable);
}

void AdjustmentAtrriWidget::initUi()
{
    m_adjustmentBtn = new DToolButton(nullptr);
    m_adjustmentBtn->setObjectName("PicFlipAdjustmentBtn");
    m_adjustmentBtn->setMaximumSize(QSize(38, 38));
    m_adjustmentBtn->setIcon(QIcon::fromTheme("ddc_flip_adjustment_normal"));
    m_adjustmentBtn->setIconSize(QSize(48, 48));
    m_adjustmentBtn->setToolTip(tr("Auto fit"));
    m_adjustmentBtn->setFocusPolicy(Qt::NoFocus);

    m_titleLabel = new QLabel(tr("Auto fit"), this);
    m_titleLabel->setDisabled(true);

    QVBoxLayout *l = new QVBoxLayout;
    setLayout(l);

    l->addWidget(m_titleLabel);
    BoxLayoutWidget *w = new BoxLayoutWidget(this);
    w->addWidget(m_adjustmentBtn);
    l->addWidget(w);
}
