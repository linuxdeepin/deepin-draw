#include "linewidget.h"

#include <QHBoxLayout>
#include <QDebug>


#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/toolbutton.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/ccheckbutton.h"
#include "utils/configsettings.h"
#include "utils/global.h"

const int BTN_SPACNT = 10;

LineWidget::LineWidget(DWidget *parent)
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
    DLabel *strokeLabel = new DLabel(this);
    strokeLabel->setObjectName("StrokeLabel");
    strokeLabel->setText(tr("颜色"));

    m_strokeBtn = new BorderColorButton(this);

    SeperatorLine *sep1Line = new SeperatorLine(this);

    DLabel *lwLabel = new DLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("描边粗细"));

    m_sideWidthWidget = new CSideWidthWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->setSpacing(BTN_SPACNT);
    layout->addWidget(m_strokeBtn);
    layout->addWidget(strokeLabel);

    layout->addWidget(sep1Line, 0, Qt::AlignCenter);

    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addStretch();
    setLayout(layout);
}

void LineWidget::initConnection()
{
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {

        QPoint btnPos = mapToGlobal(m_strokeBtn->pos());
        QPoint pos(btnPos.x() + m_strokeBtn->width() / 2,
                   btnPos.y() + m_strokeBtn->height());

        showColorPanel(DrawStatus::Stroke, pos, show);
    });

    connect(this, &LineWidget::resetColorBtns, this, [ = ] {
        m_strokeBtn->resetChecked();
    });

    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalLineAttributeChanged();
    });

}

void LineWidget::updateLineWidget()
{
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();
}




