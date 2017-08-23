#include "adjustsizewidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

#include "widgets/toolbutton.h"

AdjustsizeWidget::AdjustsizeWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel* casWidthLabel = new QLabel(this);
    casWidthLabel->setObjectName("CasWidthLabel");
    casWidthLabel->setText(tr("Canvas width"));

    QLineEdit* widthLEdit = new QLineEdit(this);
    widthLEdit->setObjectName("WidthLineEdit");
    widthLEdit->setFixedWidth(80);
    widthLEdit->setStyleSheet("background-color: red;");
    QLabel* unitWLabel = new QLabel(this);
    unitWLabel->setText("px");

    QLabel* casHeightLabel = new QLabel(this);
    casHeightLabel->setObjectName("CasHeightLabel");
    casHeightLabel->setText(tr("Canvas height"));
    QLineEdit* heightLEdit = new QLineEdit(this);
    heightLEdit->setObjectName("HeightLineEdit");
    heightLEdit->setFixedWidth(80);
    QLabel* unitHLabel = new QLabel(this);
    unitHLabel->setText("px");

    ToolButton* cutTransAreaBtn = new ToolButton(this);
    cutTransAreaBtn->setFixedWidth(100);
    cutTransAreaBtn->setObjectName("CutAreaLineEdit");
    cutTransAreaBtn->setText(tr("??????"));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(casWidthLabel);
    layout->addWidget(widthLEdit);
    layout->addWidget(unitWLabel);
    layout->addSpacing(6);
    layout->addWidget(casHeightLabel);
    layout->addWidget(heightLEdit);
    layout->addWidget(unitHLabel);
    layout->addWidget(cutTransAreaBtn);
    setLayout(layout);
}

AdjustsizeWidget::~AdjustsizeWidget()
{
}
