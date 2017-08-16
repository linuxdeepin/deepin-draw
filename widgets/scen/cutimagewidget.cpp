#include "cutimagewidget.h"

#include <QButtonGroup>
#include <QPushButton>
#include <QHBoxLayout>

CutImageWidget::CutImageWidget(QWidget *parent)
    : QWidget(parent)
{
    m_settingTips = new QLabel(this);
    m_settingTips->hide();
    m_settingTips->move(x(), y() + height());

    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->setExclusive(true);

    QPushButton* scaledABtn = new QPushButton(this);
    scaledABtn->setText("4:3");
    btnGroup->addButton(scaledABtn);
    QPushButton* scaledBBtn = new QPushButton(this);
    scaledBBtn->setText("8:5");
    btnGroup->addButton(scaledBBtn);
    QPushButton* scaledCBtn = new QPushButton(this);
    scaledCBtn->setText("16:9");
    btnGroup->addButton(scaledCBtn);
    QPushButton* scaledDBtn = new QPushButton(this);
    scaledDBtn->setText("1:1");
    btnGroup->addButton(scaledDBtn);
    QPushButton* scaledEBtn = new QPushButton(this);
    scaledEBtn->setText(tr("Free"));
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
    tipsLayout->addStretch();
    tipsLayout->addWidget(cancelBtn);
    tipsLayout->addSpacing(5);
    tipsLayout->addWidget(okBtn);
    m_settingTips->setLayout(tipsLayout);
}

void CutImageWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
}

void CutImageWidget::showTips()
{
    m_settingTips->show();
    m_settingTips->move(x(), y() + height());
}

CutImageWidget::~CutImageWidget()
{

}
