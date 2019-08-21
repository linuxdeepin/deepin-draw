#include "ailoringwidget.h"

#include <DLabel>
#include <DLineEdit>

#include <QHBoxLayout>
#include <QButtonGroup>

#include "widgets/toolbutton.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

AiloringWidget::AiloringWidget(QWidget *parent)
    : DWidget(parent)
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

    QList<ToolButton *> lwBtnList;
    QButtonGroup *scaleBtnGroup = new QButtonGroup(this);
    scaleBtnGroup->setExclusive(true);
    ToolButton *scaleBtn1 = new ToolButton("1:1", this);
    scaleBtn1->setFixedSize(50, 23);
    ToolButton *scaleBtn2 = new ToolButton("2:3", this);
    scaleBtn2->setFixedSize(50, 23);
    ToolButton *scaleBtn3 = new ToolButton("8:5", this);
    scaleBtn3->setFixedSize(50, 23);
    ToolButton *scaleBtn4 = new ToolButton("16:9", this);
    scaleBtn4->setFixedSize(50, 23);

    scaleBtnGroup->addButton(scaleBtn1);
    scaleBtnGroup->addButton(scaleBtn2);
    scaleBtnGroup->addButton(scaleBtn3);
    scaleBtnGroup->addButton(scaleBtn4);

    DLabel *freeLabel = new DLabel(this);
    freeLabel->setText(tr("自由"));

    DLabel *originalLabel = new DLabel(this);
    originalLabel->setText(tr("原始"));

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
    layout->addWidget(scaleBtn1);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(scaleBtn2);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(scaleBtn3);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(scaleBtn4);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(freeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(originalLabel);

    layout->addStretch();
    setLayout(layout);
}

AiloringWidget::~AiloringWidget()
{

}
