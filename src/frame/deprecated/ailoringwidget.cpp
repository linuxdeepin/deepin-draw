#include "ailoringwidget.h"

#include <DLabel>
#include <DLineEdit>

#include <QHBoxLayout>
#include <QButtonGroup>

#include "widgets/toolbutton.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int PUSHBUTTON_FONT_SIZE = 9;


AiloringWidget::AiloringWidget(QWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

AiloringWidget::~AiloringWidget()
{

}

void AiloringWidget::initUI()
{
    DLabel *sizeLabel = new DLabel(this);
    sizeLabel->setText(tr("尺寸"));

    DLineEdit *widthEdit = new DLineEdit(this);
    widthEdit->setText(QString::number(1920));
    widthEdit->setFixedWidth(50);

    DLabel *multiLabel = new DLabel(this);
    multiLabel->setText(tr("x"));

    DLineEdit *heightEdit = new DLineEdit(this);
    heightEdit->setText(QString::number(1080));
    heightEdit->setFixedWidth(50);

    DLabel *scaleLabel = new DLabel(this);
    scaleLabel->setText(tr("比例"));

    QFont pushBtnFont;
    pushBtnFont.setPointSize(PUSHBUTTON_FONT_SIZE);

    m_scaleBtn1_1 = new DPushButton(this);
    m_scaleBtn1_1->setText("1:1");
    m_scaleBtn1_1->setFont(pushBtnFont);

    m_scaleBtn2_3 = new DPushButton(this);
    m_scaleBtn2_3->setText("2:3");
    m_scaleBtn2_3->setFont(pushBtnFont);

    m_scaleBtn8_5 = new DPushButton(this);
    m_scaleBtn8_5->setText("8:5");
    m_scaleBtn8_5->setFont(pushBtnFont);

    m_scaleBtn16_9 = new DPushButton(this);
    m_scaleBtn16_9->setText("16:9");
    m_scaleBtn16_9->setFont(pushBtnFont);

    m_freeBtn = new DPushButton(this);
    m_freeBtn->setText(tr("自由"));
    m_freeBtn->setFont(pushBtnFont);

    m_originalBtn = new DPushButton(this);
    m_originalBtn->setText(tr("原始"));
    m_originalBtn->setFont(pushBtnFont);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(sizeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(widthEdit);
    layout->addWidget(multiLabel);
    layout->addWidget(heightEdit);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(scaleLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn1_1);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn2_3);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn8_5);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn16_9);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_freeBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_originalBtn);

    layout->addStretch();
    setLayout(layout);
}

void AiloringWidget::initConnection()
{

}
