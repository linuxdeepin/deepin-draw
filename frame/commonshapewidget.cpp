#include "commonshapewidget.h"

#include <DLabel>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QDebug>

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "utils/configsettings.h"
#include "utils/global.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/testwidget.h"
#include "drawshape/cdrawparamsigleton.h"


const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

CommonshapeWidget::CommonshapeWidget(QWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CommonshapeWidget::~CommonshapeWidget()
{
}

void CommonshapeWidget::initUI()
{
    DLabel *fillLabel = new DLabel(this);
    fillLabel->setText(tr("填充"));

    m_fillBtn = new BigColorButton("common", this);
    m_strokeBtn = new BorderColorButton(this);


    DLabel *strokeLabel = new DLabel(this);
    strokeLabel->setObjectName("StrokeLabel");
    strokeLabel->setText(tr("描边"));
    SeperatorLine *sepLine = new SeperatorLine(this);
    DLabel *lwLabel = new DLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("描边粗细"));

    m_sideWidthWidget = new CSideWidthWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addWidget(fillLabel);
    layout->addWidget(m_strokeBtn);
    layout->addWidget(strokeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(sepLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addStretch();
    setLayout(layout);
}

void CommonshapeWidget::initConnection()
{
    connect(m_fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_strokeBtn->resetChecked();
//        QPoint btnPos = mapToGlobal(fillBtn->pos());
//        QPoint pos(btnPos.x() + strokeBtn->width() / 2,
//                   btnPos.y() + strokeBtn->height() + 5);

        emit showColorPanel(DrawStatus::Fill, cursor().pos(), show);


    });
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke,  cursor().pos(), show);
    });

    connect(this, &CommonshapeWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
        m_strokeBtn->resetChecked();
    });

    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] (int width) {
        CDrawParamSigleton::GetInstance()->setLineWidth(width);
    });
}

void CommonshapeWidget::updateCommonShapWidget()
{

}

