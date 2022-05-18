#include "hboxlayoutwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <DArrowLineExpand>
#include <DWidget>
#include <DSplitter>
#include <QComboBox>

#include "boxlayoutwidget.h"
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
HBoxLayoutWidget::HBoxLayoutWidget(QWidget *parent): AttributeWgt(-1, parent)
{
    initUi();
}

void HBoxLayoutWidget::setTitle(const QString &title)
{
    m_label->setText(title);
}

void HBoxLayoutWidget::addWidget(QWidget *widget, int stretch)
{
    m_lay->addWidget(widget, stretch);
}

void HBoxLayoutWidget::initUi()
{
    m_label = new DLabel(this);
    m_label->setText("arrow");
    QVBoxLayout *fillLayout = new QVBoxLayout(this);
    fillLayout->setMargin(0);
    fillLayout->setContentsMargins(0, 0, 0, 0);
    m_lay = new QHBoxLayout;
    m_lay->setContentsMargins(0, 10, 10, 0);
    fillLayout->addWidget(m_label);
    fillLayout->addLayout(m_lay);
    setLayout(fillLayout);
}

