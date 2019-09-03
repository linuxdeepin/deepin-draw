#include "cpenwidget.h"

#include <QHBoxLayout>
#include <QDebug>


#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/cpushbutton.h"
#include "drawshape/cdrawparamsigleton.h"


const int BTN_SPACNT = 10;

CPenWidget::CPenWidget(QWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CPenWidget::~CPenWidget()
{

}

void CPenWidget::initUI()
{
    QLabel *strokeLabel = new QLabel(this);
    strokeLabel->setObjectName("StrokeLabel");
    strokeLabel->setText(tr("颜色"));

    m_strokeBtn = new BorderColorButton(this);

    SeperatorLine *sep1Line = new SeperatorLine(this);

    QLabel *lineTypeLabel = new QLabel(this);
    lineTypeLabel->setObjectName("LineType");
    lineTypeLabel->setText(tr("类型"));

    QMap<CPushButton::CButtonSattus, QString> pictureMap;

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/attribute/line tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/attribute/line tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/attribute/line tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/attribute/line tool_checked.svg");
    m_straightline = new CPushButton(pictureMap, this);
    m_actionButtons.append(m_straightline);


    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/attribute/arrow tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/attribute/arrow tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/attribute/arrow tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/attribute/arrow tool_checked.svg");
    m_arrowline = new CPushButton(pictureMap, this);
    m_actionButtons.append(m_arrowline);

    QLabel *lwLabel = new QLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("描边粗细"));

    m_sideWidthWidget = new CSideWidthWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(lineTypeLabel);
    layout->addWidget(m_straightline);
    layout->addWidget(m_arrowline);
    layout->setSpacing(BTN_SPACNT);
    layout->addWidget(m_strokeBtn);
    layout->addWidget(strokeLabel);

    layout->addWidget(sep1Line, 0, Qt::AlignCenter);

    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addStretch();
    setLayout(layout);
}

void CPenWidget::initConnection()
{
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {

        QPoint btnPos = mapToGlobal(m_strokeBtn->pos());
        QPoint pos(btnPos.x() + m_strokeBtn->width() / 2,
                   btnPos.y() + m_strokeBtn->height());

        showColorPanel(DrawStatus::Stroke, pos, show);
    });

    connect(this, &CPenWidget::resetColorBtns, this, [ = ] {
        m_strokeBtn->resetChecked();
    });


    connect(m_straightline, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_straightline);
        CDrawParamSigleton::GetInstance()->setCurrentPenType(EPenType::straight);
        emit signalPenAttributeChanged();
    });

    connect(m_arrowline, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_arrowline);
        CDrawParamSigleton::GetInstance()->setCurrentPenType(EPenType::arrow);
        emit signalPenAttributeChanged();
    });

    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalPenAttributeChanged();
    });
}

void CPenWidget::clearOtherSelections(CPushButton *clickedButton)
{
    foreach (CPushButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}

void CPenWidget::updatePenWidget()
{
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();
    EPenType penType = CDrawParamSigleton::GetInstance()->getCurrentPenType();
    if (penType == EPenType::straight) {
        if (!m_straightline->isChecked()) {
            m_straightline->setChecked(true);
            m_arrowline->setChecked(false);
        }
    } else if (penType == EPenType::arrow) {
        if (!m_arrowline->isChecked()) {
            m_arrowline->setChecked(true);
            m_straightline->setChecked(false);
        }
    }
}
