#include "styleattriwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <DArrowLineExpand>
#include <DWidget>

StyleAttriWidget::StyleAttriWidget(int attri, QWidget *parent): AttributeWgt(attri, parent)
{
    initUi();
}

QVBoxLayout *StyleAttriWidget::getLayout()
{
    return m_layout;//dynamic_cast<QVBoxLayout *>(layout());
}

void StyleAttriWidget::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setMargin(0);
    this->setLayout(mainLayout);

    DArrowLineExpand *expand = new DArrowLineExpand(this);
    expand->setTitle(tr("style"));
    DWidget *w = new DWidget(this);
    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setMargin(0);
    w->setLayout(m_layout);
    expand->setContent(w);
    //expand->show();
    mainLayout->addWidget(expand);
}
