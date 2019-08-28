#include "linewidget.h"

#include <QHBoxLayout>
#include <QDebug>


#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/toolbutton.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/cpushbutton.h"
#include "utils/configsettings.h"
#include "utils/global.h"

const int BTN_SPACNT = 10;

LineWidget::LineWidget(QWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

LineWidget::~LineWidget()
{
}


void LineWidget::initUI()
{
    QLabel *strokeLabel = new QLabel(this);
    strokeLabel->setObjectName("StrokeLabel");
    strokeLabel->setText(tr("颜色"));

    m_strokeButton = new BorderColorButton(this);

    SeperatorLine *sep1Line = new SeperatorLine(this);

    QLabel *lwLabel = new QLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("描边粗细"));

    m_sideWidthWidget = new CSideWidthWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->setSpacing(BTN_SPACNT);
    layout->addWidget(m_strokeButton);
    layout->addWidget(strokeLabel);

    layout->addWidget(sep1Line, 0, Qt::AlignCenter);

    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addStretch();
    setLayout(layout);
}

void LineWidget::initConnection()
{
    connect(m_strokeButton, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        showColorPanel(DrawStatus::Stroke, cursor().pos(), show);
    });
}




