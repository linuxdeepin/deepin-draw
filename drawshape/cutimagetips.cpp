#include "cutimagetips.h"

#include <QButtonGroup>
#include <QPushButton>
#include <QHBoxLayout>

CutImageTips::CutImageTips(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->setExclusive(true);

    QPushButton* scaledABtn = new QPushButton(this);
    scaledABtn->setText("4:3");
    scaledABtn->setFixedSize(30, 24);
    btnGroup->addButton(scaledABtn);
    QPushButton* scaledBBtn = new QPushButton(this);
    scaledBBtn->setText("8:5");
    scaledBBtn->setFixedSize(30, 24);
    btnGroup->addButton(scaledBBtn);
    QPushButton* scaledCBtn = new QPushButton(this);
    scaledCBtn->setText("16:9");
    scaledCBtn->setFixedSize(30, 24);
    btnGroup->addButton(scaledCBtn);
    QPushButton* scaledDBtn = new QPushButton(this);
    scaledDBtn->setText("1:1");
    scaledDBtn->setFixedSize(30, 24);
    btnGroup->addButton(scaledDBtn);
    QPushButton* scaledEBtn = new QPushButton(this);
    scaledEBtn->setText(tr("Free"));
    scaledEBtn->setFixedSize(30, 24);
    btnGroup->addButton(scaledEBtn);

    QPushButton* cancelBtn = new QPushButton(this);
    cancelBtn->setText(tr("Cancel"));

    QPushButton* okBtn = new QPushButton(this);
    okBtn->setText(tr("Cut"));

    QHBoxLayout* tipsLayout = new QHBoxLayout;
    tipsLayout->setMargin(0);
    tipsLayout->setSpacing(0);
    tipsLayout->addWidget(scaledABtn);
    tipsLayout->addWidget(scaledBBtn);
    tipsLayout->addWidget(scaledCBtn);
    tipsLayout->addWidget(scaledDBtn);
    tipsLayout->addWidget(scaledEBtn);
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
