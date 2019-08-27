#include "polygonalstarattributewidget.h"

#include <DLabel>
#include <DFontSizeManager>
#include <DSlider>
#include <DLineEdit>



#include <QHBoxLayout>
#include <QButtonGroup>

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "utils/configsettings.h"
#include "utils/global.h"
#include "widgets/csidewidthwidget.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

PolygonalStarAttributeWidget::PolygonalStarAttributeWidget(QWidget *parent)
    : DWidget(parent)
{
//    DFontSizeManager::instance()->bind(this, DFontSizeManager::T1);

    BigColorButton *fillBtn = new BigColorButton( this);
    DLabel *fillLabel = new DLabel(this);
    fillLabel->setObjectName("FillLabel");
    fillLabel->setText(tr("填充"));

    BorderColorButton *strokeBtn = new BorderColorButton(this);
    strokeBtn->setObjectName("FillStrokeButton");

    connect(fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        strokeBtn->resetChecked();
        emit showColorPanel(DrawStatus::Fill, cursor().pos(), show);

    });
    connect(strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke,  cursor().pos(), show);
    });

    connect(this, &PolygonalStarAttributeWidget::resetColorBtns, this, [ = ] {
        fillBtn->resetChecked();
        strokeBtn->resetChecked();
    });

    DLabel *strokeLabel = new DLabel(this);
    strokeLabel->setObjectName("StrokeLabel");
    strokeLabel->setText(tr("描边"));

    SeperatorLine *sepLine = new SeperatorLine(this);
    DLabel *lwLabel = new DLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("描边粗细"));

    CSideWidthWidget *sideWidthWidget = new CSideWidthWidget(this);


    DLabel *anchorNumLabel = new DLabel(this);
    anchorNumLabel->setObjectName("AnchorNumLabel");
    anchorNumLabel->setText(tr("锚点数"));

    DSlider *anchorNumSlider = new DSlider(this);
    anchorNumSlider->setOrientation(Qt::Horizontal);
    anchorNumSlider->setMinimum(3);
    anchorNumSlider->setMaximum(50);
    anchorNumSlider->setMinimumWidth(200);
    anchorNumSlider->setMaximumHeight(24);

    DLineEdit *anchorNumEdit = new DLineEdit(this);
    anchorNumEdit->setMinimumWidth(50);
    anchorNumEdit->setMaximumWidth(50);
    anchorNumEdit->setText(QString::number(anchorNumSlider->value()));


    connect(anchorNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        anchorNumEdit->setText(QString::number(value));
    });

    DLabel *radiusLabel = new DLabel(this);
    radiusLabel->setObjectName("RadiusLabel");
    radiusLabel->setText(tr("半径"));

    DSlider *radiusNumSlider = new DSlider(this);
    radiusNumSlider->setOrientation(Qt::Horizontal);
    radiusNumSlider->setMinimum(0);
    radiusNumSlider->setMaximum(100);
    radiusNumSlider->setMinimumWidth(200);
    radiusNumSlider->setMaximumHeight(24);


    DLineEdit *radiusNumEdit = new DLineEdit(this);
    radiusNumEdit->setMinimumWidth(60);
    radiusNumEdit->setMaximumWidth(60);
    radiusNumEdit->setText(QString("%1%").arg(radiusNumSlider->value()));


    connect(radiusNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        radiusNumEdit->setText(QString("%1%").arg(value));
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(fillBtn);
    layout->addWidget(fillLabel);
    layout->addWidget(strokeBtn);
    layout->addWidget(strokeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(sepLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lwLabel);
    layout->addWidget(sideWidthWidget);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(anchorNumLabel);
    layout->addWidget(anchorNumSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(anchorNumEdit);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(radiusLabel);
    layout->addWidget(radiusNumSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(radiusNumEdit);

    layout->addStretch();
    setLayout(layout);

}



PolygonalStarAttributeWidget::~PolygonalStarAttributeWidget()
{

}
