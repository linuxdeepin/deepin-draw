#include "csidewidthwidget.h"

#include "cpushbutton.h"
#include "drawshape/cdrawparamsigleton.h"

CSideWidthWidget::CSideWidthWidget(QWidget *parent)
    : DFrame(parent)
{
    initUI();
    initConnection();
}


void CSideWidthWidget::initUI()
{
    m_layout = new QHBoxLayout (this);
    m_layout->setMargin(0);

    QMap<CPushButton::CButtonSattus, QString> pictureMap;

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/tickness01_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/draw/tickness01_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/draw/tickness01_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/tickness01_checked.svg");

//    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/line_border_1_normal.svg");
//    pictureMap[CPushButton::Hover]  = QString(":/theme/light/images/draw/line_border_1_hover.svg");
//    pictureMap[CPushButton::Press]  = QString(":/theme/light/images/draw/line_border_1_active.svg");
//    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/line_border_1_active.svg");

    m_finerButton = new CPushButton(pictureMap, this);
    m_buttonMap.insert(m_finerButton, Finer);

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/tickness02_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/draw/tickness02_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/draw/tickness02_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/tickness02_checked.svg");
    m_fineButton = new CPushButton(pictureMap, this);

    m_fineButton->minimumSize();

    m_buttonMap.insert(m_fineButton, Fine);

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/tickness03_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/draw/tickness03_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/draw/tickness03_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/tickness03_checked.svg");
    m_mediumButton = new CPushButton(pictureMap, this);
    m_buttonMap.insert(m_mediumButton, Medium);

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/tickness04_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/draw/tickness04_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/draw/tickness04_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/tickness04_checked.svg");
    m_boldButton = new CPushButton(pictureMap, this);
    m_buttonMap.insert(m_boldButton, Bold);

    m_layout->addWidget(m_finerButton);
    m_layout->addWidget(m_fineButton);
    m_layout->addWidget(m_mediumButton);
    m_layout->addWidget(m_boldButton);

    setLayout(m_layout);
}

void CSideWidthWidget::initConnection()
{
    connect(m_finerButton, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_finerButton);
        emit sinalSideWidthChange(m_buttonMap.value(m_finerButton));
    });

    connect(m_fineButton, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_fineButton);
        emit sinalSideWidthChange(m_buttonMap.value(m_fineButton));
    });

    connect(m_mediumButton, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_mediumButton);
        emit sinalSideWidthChange(m_buttonMap.value(m_mediumButton));
    });

    connect(m_boldButton, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_boldButton);
        emit sinalSideWidthChange(/*m_buttonMap.value(m_boldButton)*/8);
    });

}


void CSideWidthWidget::clearOtherSelections(CPushButton *clickedButton)
{
    foreach (CPushButton *button, m_buttonMap.keys()) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };

    CDrawParamSigleton::GetInstance()->setLineWidth(8);
}

void CSideWidthWidget::slotSideWidthChange(int sideWidth)
{
    QMapIterator<CPushButton *, CLineWidth> i(m_buttonMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == sideWidth) {
            if (!i.key()->isChecked()) {
                i.key()->setChecked(true);
            }
        } else {
            i.key()->setChecked(false);
        }
    }
}


