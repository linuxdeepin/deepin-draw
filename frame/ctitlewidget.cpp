#include "ctitlewidget.h"

#include <QHBoxLayout>


CTitleWidget::CTitleWidget(DWidget *parent)
    : DWidget(parent)
{
    m_title = new DLabel(this);

    m_title->setText(tr("未命名画板"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_title);
    layout->addStretch();

    setLayout(layout);
}

CTitleWidget::~CTitleWidget()
{

}

void CTitleWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}
