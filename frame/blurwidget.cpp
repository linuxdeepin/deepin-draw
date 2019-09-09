#include "blurwidget.h"

#include <DLabel>
#include <DSlider>

#include <QHBoxLayout>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"
#include "widgets/ccheckbutton.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

BlurWidget::BlurWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

BlurWidget::~BlurWidget()
{
}

void BlurWidget::initUI()
{
    DLabel *penLabel = new DLabel(this);
    penLabel->setObjectName("WidthLabel");
    penLabel->setText(tr("类型"));


    QMap<CCheckButton::EButtonSattus, QString> pictureMap;

    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/attribute/fuzzy tool_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/attribute/fuzzy tool_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/attribute/fuzzy tool_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/attribute/fuzzy tool_checked.svg");
    m_blurBtn = new CCheckButton(pictureMap, this);
    m_actionButtons.append(m_blurBtn);


    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/attribute/smudge tool_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/attribute/smudge tool_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/attribute/smudge tool_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/attribute/smudge tool_checked.svg");
    m_masicBtn = new CCheckButton(pictureMap, this);
    m_actionButtons.append(m_masicBtn);

    DSlider *lineWidthSlider = new DSlider(Qt::Horizontal, this);

    lineWidthSlider->setMinimum(20);
    lineWidthSlider->setMaximum(160);
    lineWidthSlider->setFixedWidth(200);
    lineWidthSlider->setMaximumHeight(24);
    lineWidthSlider->setOrientation(Qt::Horizontal);


    DLabel *lineWidthLabel = new DLabel(this);
    lineWidthLabel->setObjectName("WidthLabel");
    lineWidthLabel->setText(QString("%1px").arg(lineWidthSlider->value()));

    connect(lineWidthSlider, &DSlider::valueChanged, this, [ = ](int value) {
        lineWidthLabel->setText(QString("%1px").arg(value));
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(penLabel);
    layout->addWidget(m_blurBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_masicBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lineWidthSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lineWidthLabel);
    layout->addStretch();
    setLayout(layout);
}

void BlurWidget::initConnection()
{
    connect(m_blurBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_blurBtn);

    });

    connect(m_masicBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_masicBtn);

    });
}

void BlurWidget::clearOtherSelections(CCheckButton *clickedButton)
{
    foreach (CCheckButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}
