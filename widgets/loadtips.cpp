#include "loadtips.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>

#include "utils/global.h"

const QSize TIPS_SIZE = QSize(300, 300);

LoadTips::LoadTips(QWidget *parent)
    : QDialog(parent),
      m_counts(0)
{
    DRAW_THEME_INIT_WIDGET("LoadTips");
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);
    QLabel* w = new QLabel(this);
    w->setObjectName("TipsLabel");
    w->setFixedSize(TIPS_SIZE);
    m_waterProg = new DWaterProgress(this);
    m_waterProg->setFixedSize(QSize(200, 200));

    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName("MessageLabel");
    m_messageLabel->setText(tr("Importing pictures, please wait..."));

     QVBoxLayout* vLayout = new QVBoxLayout(w);
     vLayout->setMargin(0);
     vLayout->setSpacing(0);
     vLayout->addStretch();
     vLayout->addWidget(m_waterProg, 0, Qt::AlignCenter);
     vLayout->addSpacing(20);
     vLayout->addWidget(m_messageLabel, 0, Qt::AlignCenter);
     vLayout->addStretch();

     connect(this, &LoadTips::progressValueChanged, this, [=](int value){
        m_counts = value;
        m_waterProg->setValue(m_counts);
     });
     connect(this, &LoadTips::finishedPainting, this, [=]{
         this->hide();
     });
}

void LoadTips::showTips()
{
    this->show();
    QPoint gp = this->mapToGlobal(QPoint(0, 0));
    move((window()->width() - this->width()) / 2 + gp.x(),
         (window()->height() - this->height()) / 2 + gp.y());


    m_waterProg->setValue(m_counts);
    m_waterProg->start();

}

LoadTips::~LoadTips()
{
}
