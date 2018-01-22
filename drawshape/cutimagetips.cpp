#include "cutimagetips.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QDebug>

#include "utils/global.h"
#include "utils/configsettings.h"

const QSize RATIONLABEL_SIZE = QSize(150, 24);
const QSize RATION_MINI_SIZE = QSize(400, 24);
const int COORDINATE_Y = 5;
CutImageTips::CutImageTips(QWidget *parent)
    : QLabel(parent)
{
    DRAW_THEME_INIT_WIDGET("CutImageTips");
    setMinimumSize(RATION_MINI_SIZE);

    QString defaultRation = ConfigSettings::instance()->value("cut",
                                                              "ration").toString();
    qDebug() << "CutImageTips:" << defaultRation;

    QLabel* rationLabel = new QLabel(this);
    rationLabel->setObjectName("RationLabel");
    rationLabel->setFixedSize(RATIONLABEL_SIZE);

    m_rationBtnGroup = new QButtonGroup(this);
    m_rationBtnGroup->setExclusive(true);

    RationButton* scaledABtn = new RationButton(this);
    scaledABtn->setText("4:3");
    m_rationBtnGroup->addButton(scaledABtn);
    if (defaultRation == scaledABtn->text()) {
        scaledABtn->setChecked(true);
    }
    connect(scaledABtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration4_3);
    });

    RationButton* scaledBBtn = new RationButton(this);
    scaledBBtn->setText("8:5");
    m_rationBtnGroup->addButton(scaledBBtn);
    if (defaultRation == scaledBBtn->text())
        scaledBBtn->setChecked(true);
    connect(scaledBBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration8_5);
    });

    RationButton* scaledCBtn = new RationButton(this);
    scaledCBtn->setText("16:9");
    m_rationBtnGroup->addButton(scaledCBtn);
    if (defaultRation == scaledCBtn->text())
        scaledCBtn->setChecked(true);
    connect(scaledCBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration16_9);
    });

    RationButton* scaledDBtn = new RationButton(this);
    scaledDBtn->setText("1:1");
    m_rationBtnGroup->addButton(scaledDBtn);
    if (defaultRation == scaledDBtn->text())
        scaledDBtn->setChecked(true);
    connect(scaledDBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::Ration1_1);
    });

    RationButton* scaledEBtn = new RationButton(this);
    scaledEBtn->setText(tr("free"));
    scaledEBtn->setObjectName("Free");
    m_rationBtnGroup->addButton(scaledEBtn);
    if (defaultRation == scaledEBtn->text())
        scaledEBtn->setChecked(true);
    connect(scaledEBtn, &RationButton::clicked, this, [=]{
        emit cutRationChanged(CutRation::FreeRation);
    });

    m_cancelBtn = new RationButton(this);
    m_cancelBtn->setObjectName("CancelBtn");
    m_cancelBtn->setFixedSize(60, 24);
    m_cancelBtn->setText(tr("Cancel"));
    m_cancelBtn->setCheckable(false);
    connect(m_cancelBtn, &RationButton::clicked, this, [=]{
        emit canceled();
    });

    m_okBtn = new RationButton(this);
    m_okBtn->setObjectName("ClipBtn");
    m_okBtn->setFixedSize(60, 24);
    m_okBtn->setText(tr("Clip"));
    m_okBtn->setChecked(true);
    connect(m_okBtn, &RationButton::clicked, this, [=]{
        emit cutAction();
    });

    QHBoxLayout* rationLayout = new QHBoxLayout(rationLabel);
    rationLayout->setMargin(0);
    rationLayout->setSpacing(0);
    rationLayout->addWidget(scaledABtn);
    rationLayout->addWidget(scaledBBtn);
    rationLayout->addWidget(scaledCBtn);
    rationLayout->addWidget(scaledDBtn);
    rationLayout->addWidget(scaledEBtn);

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addWidget(rationLabel);
    mLayout->addStretch();
    mLayout->addSpacing(10);
    mLayout->addWidget(m_cancelBtn);
    mLayout->addSpacing(5);
    mLayout->addWidget(m_okBtn);
    setLayout(mLayout);

    connect(this, &CutImageTips::cutRationChanged, this, &CutImageTips::setCutRation);
}

void CutImageTips::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
}

void CutImageTips::keyPressEvent(QKeyEvent *e)
{

    if (e->key() == Qt::Key_Shift)
    {
        GlobalShortcut::instance()->setShiftScStatus(true);
    } else if (e->key() == Qt::Key_Alt)
    {
        qDebug() << "alt key pressed...";
        GlobalShortcut::instance()->setAltScStatus(true);
    } else if (e->key() == Qt::Key_Escape)
    {
        emit canceled();
    }
}

void CutImageTips::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift)
    {
        GlobalShortcut::instance()->setShiftScStatus(false);
    } else if (e->key() == Qt::Key_Alt)
    {
        GlobalShortcut::instance()->setAltScStatus(false);
    }

//    QFrame::keyReleaseEvent(e);
}

void CutImageTips::showTips(QPointF pos, int newWidth)
{
    m_cancelBtn->setChecked(false);
    m_okBtn->setChecked(true);
    QString defaultRation = ConfigSettings::instance()->value(
                "cut", "ration").toString();
    qDebug() << "defaultRation:" << defaultRation;
    foreach (QAbstractButton* rationBtn, m_rationBtnGroup->buttons()) {
        if (rationBtn->text() == defaultRation ||
                rationBtn->objectName().toLower() == defaultRation)
            rationBtn->setChecked(true);
    }

    setFixedWidth(newWidth);
    this->move(QPoint(pos.x(), pos.y() + COORDINATE_Y));
    this->show();
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
