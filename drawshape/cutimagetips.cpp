 #include "cutimagetips.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QDebug>

#include "utils/global.h"
#include "utils/configsettings.h"

const QSize RATIONLABEL_SIZE = QSize(160, 24);

CutImageTips::CutImageTips(QWidget *parent)
    : QDialog(parent)
{
    DRAW_THEME_INIT_WIDGET("CutImageTips");
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    QString defaultRation = ConfigSettings::instance()->value("cut", "ration").toString();
    qDebug() << "CutImageTips:" << defaultRation;

    QLabel* rationLabel = new QLabel(this);
    rationLabel->setObjectName("RationLabel");
    rationLabel->setFixedSize(RATIONLABEL_SIZE);

    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->setExclusive(true);

    RationButton* scaledABtn = new RationButton(this);
    scaledABtn->setText("4:3");
    btnGroup->addButton(scaledABtn);
    if (defaultRation == scaledABtn->text()) {
        scaledABtn->setChecked(true);
    }
    connect(scaledABtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration4_3);
    });

    RationButton* scaledBBtn = new RationButton(this);
    scaledBBtn->setText("8:5");
    btnGroup->addButton(scaledBBtn);
    if (defaultRation == scaledBBtn->text())
        scaledBBtn->setChecked(true);
    connect(scaledBBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration8_5);
    });

    RationButton* scaledCBtn = new RationButton(this);
    scaledCBtn->setText("16:9");
    btnGroup->addButton(scaledCBtn);
    if (defaultRation == scaledCBtn->text())
        scaledCBtn->setChecked(true);
    connect(scaledCBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration16_9);
    });

    RationButton* scaledDBtn = new RationButton(this);
    scaledDBtn->setText("1:1");
    btnGroup->addButton(scaledDBtn);
    if (defaultRation == scaledDBtn->text())
        scaledDBtn->setChecked(true);
    connect(scaledDBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration1_1);
    });

    RationButton* scaledEBtn = new RationButton(this);
    scaledEBtn->setText(tr("free"));
    btnGroup->addButton(scaledEBtn);
    if (defaultRation == scaledEBtn->text())
        scaledEBtn->setChecked(true);
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

    connect(this, &CutImageTips::cutRationChanged, this, &CutImageTips::setCutRation);
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

void CutImageTips::setCutRation(CutRation ration)
{
    switch (ration) {
    case CutRation::Ration1_1:
        ConfigSettings::instance()->setValue("cut", "ration", "1:1");
        break;
    case CutRation::Ration4_3:
        ConfigSettings::instance()->setValue("cut", "ration", "4:3");
        break;
    case CutRation::Ration8_5:
        ConfigSettings::instance()->setValue("cut", "ration", "8:5");
        break;
    case CutRation::Ration16_9:
        ConfigSettings::instance()->setValue("cut", "ration", "16:9");
        break;
    default:
        ConfigSettings::instance()->setValue("cut", "ration", "free");
        break;
    }
}

CutImageTips::~CutImageTips()
{
}
