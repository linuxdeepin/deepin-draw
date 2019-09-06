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



const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

CommonshapeWidget::CommonshapeWidget(DWidget *parent)
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

    m_fillBtn = new BigColorButton(this);
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
        mapToGlobal(QPoint(1, 1));
//        emit showColorPanel(DrawStatus::Fill, cursor().pos(), show);
        emit showColorPanel(DrawStatus::Fill, getBtnPosition(m_fillBtn), show);


    });
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke, getBtnPosition(m_strokeBtn), show);
    });

    connect(this, &CommonshapeWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
        m_strokeBtn->resetChecked();
    });

    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalCommonShapeChanged();
    });
}

void CommonshapeWidget::updateCommonShapWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();
}

QPoint CommonshapeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
