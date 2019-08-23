#include "blurwidget.h"

#include <DLabel>
#include <DSlider>

#include <QHBoxLayout>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"
#include "widgets/cpushbutton.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

BlurWidget::BlurWidget(QWidget *parent)
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


    QMap<CPushButton::CButtonSattus, QString> pictureMap;

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/attribute/fuzzy tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/attribute/fuzzy tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/attribute/fuzzy tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/attribute/fuzzy tool_checked.svg");
    m_blurBtn = new CPushButton(pictureMap, this);
    m_actionButtons.append(m_blurBtn);


    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/attribute/smudge tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/attribute/smudge tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/attribute/smudge tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/attribute/smudge tool_checked.svg");
    m_masicBtn = new CPushButton(pictureMap, this);
    m_actionButtons.append(m_masicBtn);

    DSlider *lineWidthSlider = new DSlider(Qt::Horizontal, this);

    lineWidthSlider->setMinimum(20);
    lineWidthSlider->setMaximum(160);
    lineWidthSlider->setMinimumWidth(200);
    lineWidthSlider->setMaximumWidth(200);
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
    connect(m_blurBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_blurBtn);

    });

    connect(m_masicBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_masicBtn);

    });
}

void BlurWidget::clearOtherSelections(CPushButton *clickedButton)
{
    foreach (CPushButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}
