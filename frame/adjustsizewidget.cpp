#include "adjustsizewidget.h"

#include <QLabel>
#include <QHBoxLayout>

#include "widgets/toolbutton.h"

AdjustsizeWidget::AdjustsizeWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel* casWidthLabel = new QLabel(this);
    casWidthLabel->setObjectName("CasWidthLabel");
    casWidthLabel->setText(tr("Canvas width"));

    m_widthLEdit = new QLineEdit(this);
    m_widthLEdit->setObjectName("WidthLineEdit");
    m_widthLEdit->setFixedWidth(80);

    m_widthLEdit->setStyleSheet("background-color: red;");
    QLabel* unitWLabel = new QLabel(this);
    unitWLabel->setText("px");

    QLabel* casHeightLabel = new QLabel(this);
    casHeightLabel->setObjectName("CasHeightLabel");
    casHeightLabel->setText(tr("Canvas height"));

    m_heightLEdit = new QLineEdit(this);
    m_heightLEdit->setObjectName("HeightLineEdit");
    m_heightLEdit->setFixedWidth(80);
    QLabel* unitHLabel = new QLabel(this);
    unitHLabel->setText("px");

    ToolButton* cutTransAreaBtn = new ToolButton(this);
    cutTransAreaBtn->setFixedWidth(100);
    cutTransAreaBtn->setObjectName("CutAreaLineEdit");
    cutTransAreaBtn->setText(tr("Autocrop"));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(casWidthLabel);
    layout->addWidget(m_widthLEdit);
    layout->addWidget(unitWLabel);
    layout->addSpacing(6);
    layout->addWidget(casHeightLabel);
    layout->addWidget(m_heightLEdit);
    layout->addWidget(unitHLabel);
    layout->addWidget(cutTransAreaBtn);
    setLayout(layout);
}

void AdjustsizeWidget::setCanvasSize(int width, int height)
{
    m_widthLEdit->setText(QString("%1").arg(width));
    m_heightLEdit->setText(QString("%1").arg(height));
}

AdjustsizeWidget::~AdjustsizeWidget()
{
}
