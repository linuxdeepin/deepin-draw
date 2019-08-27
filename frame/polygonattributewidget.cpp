#include "polygonattributewidget.h"

#include <DLabel>
#include <DFontSizeManager>
#include <DSlider>
#include <DLineEdit>


#include <QHBoxLayout>
#include <QButtonGroup>

#include "widgets/csidewidthwidget.h"
#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "utils/configsettings.h"
#include "utils/global.h"


const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

//DWIDGET_USE_NAMESPACE

PolygonAttributeWidget::PolygonAttributeWidget(QWidget *parent)
    : DWidget(parent)
{
//    DFontSizeManager::instance()->bind(this, DFontSizeManager::T1);

    BigColorButton *fillBtn = new BigColorButton(this);
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

    connect(this, &PolygonAttributeWidget::resetColorBtns, this, [ = ] {
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


    DLabel *sideNumLabel = new DLabel(this);
    sideNumLabel->setObjectName("AnchorNumLabel");
    sideNumLabel->setText(tr("侧边数"));

    DSlider *sideNumSlider = new DSlider(this);
    sideNumSlider->setSingleStep(1);
    sideNumSlider->setMinimum(4);
    sideNumSlider->setMaximum(10);
    sideNumSlider->setMinimumWidth(200);
    sideNumSlider->setMaximumHeight(24);
    sideNumSlider->setOrientation(Qt::Horizontal);

    DLineEdit *sideNumEdit = new DLineEdit(this);
    sideNumEdit->setMinimumWidth(50);
    sideNumEdit->setMaximumWidth(50);
    sideNumEdit->setText(QString::number(sideNumSlider->value()));


    connect(sideNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        sideNumEdit->setText(QString::number(value));
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
    layout->addWidget(sideNumLabel);
    layout->addWidget(sideNumSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(sideNumEdit);

    layout->addStretch();
    setLayout(layout);

}



PolygonAttributeWidget::~PolygonAttributeWidget()
{

}
