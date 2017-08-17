#include "cutimagetips.h"

#include <QButtonGroup>
#include <QHBoxLayout>

#include "utils/global.h"

const QSize RATIONLABEL_SIZE = QSize(160, 24);

CutImageTips::CutImageTips(QWidget *parent)
    : QDialog(parent)
{
    DRAW_THEME_INIT_WIDGET("CutImageTips");
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 24);
    QLabel* rationLabel = new QLabel(this);
    rationLabel->setObjectName("RationLabel");
    rationLabel->setFixedSize(RATIONLABEL_SIZE);

    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->setExclusive(true);

    RationButton* scaledABtn = new RationButton(this);
    scaledABtn->setText("4:3");
    btnGroup->addButton(scaledABtn);
    connect(scaledABtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration4_3);
    });

    RationButton* scaledBBtn = new RationButton(this);
    scaledBBtn->setText("8:5");
    btnGroup->addButton(scaledBBtn);
    connect(scaledBBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration8_5);
    });

    RationButton* scaledCBtn = new RationButton(this);
    scaledCBtn->setText("16:9");
    btnGroup->addButton(scaledCBtn);
    connect(scaledCBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration16_9);
    });

    RationButton* scaledDBtn = new RationButton(this);
    scaledDBtn->setText("1:1");
    scaledDBtn->setChecked(true);
    btnGroup->addButton(scaledDBtn);
    connect(scaledDBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration1_1);
    });

    RationButton* scaledEBtn = new RationButton(this);
    scaledEBtn->setText(tr("Free"));
    btnGroup->addButton(scaledEBtn);
    connect(scaledEBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::FreeRation);
    });

    RationButton* cancelBtn = new RationButton(this);
    cancelBtn->setFixedSize(60, 24);
    cancelBtn->setText(tr("Cancel"));
    connect(cancelBtn, &RationButton::clicked, this, [=]{
        emit canceled();
    });

    RationButton* okBtn = new RationButton(this);
    okBtn->setFixedSize(60, 24);
    okBtn->setText(tr("Cut"));
    okBtn->setChecked(true);
    connect(okBtn, &RationButton::clicked, this, [=]{
        emit cutAction();
    });

    QHBoxLayout* rationLayout = new QHBoxLayout;
    rationLayout->setMargin(0);
    rationLayout->setSpacing(0);
    rationLayout->addWidget(scaledABtn);
    rationLayout->addWidget(scaledBBtn);
    rationLayout->addWidget(scaledCBtn);
    rationLayout->addWidget(scaledDBtn);
    rationLayout->addWidget(scaledEBtn);
    rationLabel->setLayout(rationLayout);

    QHBoxLayout* tipsLayout = new QHBoxLayout;
    tipsLayout->setMargin(0);
    tipsLayout->setSpacing(0);
    tipsLayout->addWidget(rationLabel);
    tipsLayout->addSpacing(10);
    tipsLayout->addWidget(cancelBtn);
    tipsLayout->addSpacing(5);
    tipsLayout->addWidget(okBtn);
    setLayout(tipsLayout);
}

void CutImageTips::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
}

void CutImageTips::showTips(QPoint pos)
{
    this->show();
    QPoint tipPos = QPoint(pos.x() - this->width(), pos.y());
    this->move(tipPos.x(), tipPos.y());
}

CutImageTips::~CutImageTips()
{

}
