#include "mainwidget.h"

#include <QVBoxLayout>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent) {

    m_topToolbar = new TopToolbar(this);
    m_mainWidget = new QWidget(this);
    m_mainWidget->setStyleSheet("background: green;");
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget(m_topToolbar, 0, Qt::AlignTop);
    vLayout->addWidget(m_mainWidget, 0, Qt::AlignCenter);
    setLayout(vLayout);
}


MainWidget::~MainWidget() {
}
