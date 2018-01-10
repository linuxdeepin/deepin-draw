#include "adjustsizewidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"
#include "utils/dintvalidator.h"
#include "utils/baseutils.h"

AdjustsizeWidget::AdjustsizeWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel* casWidthLabel = new QLabel(this);
    casWidthLabel->setObjectName("CasWidthLabel");
    casWidthLabel->setText(tr("Width"));

    m_widthLEdit = new FontsizeLineEdit(this);
    m_widthLEdit->setObjectName("WidthLineEdit");
    m_widthLEdit->setFixedWidth(80);

    QLabel* unitWLabel = new QLabel(this);
    unitWLabel->setText("px");

    QLabel* casHeightLabel = new QLabel(this);
    casHeightLabel->setObjectName("CasHeightLabel");
    casHeightLabel->setText(tr("Height"));

    m_heightLEdit = new FontsizeLineEdit(this);
    m_heightLEdit->setObjectName("HeightLineEdit");
    m_heightLEdit->setFixedWidth(80);

    QSize artboardSize = initArtboardSize();
    setCanvasSize(artboardSize);

    connect(m_widthLEdit, &FontsizeLineEdit::editingFinished, this, [=]{
        int canvasWidth = m_widthLEdit->text().toInt();
        canvasWidth = std::min(500000, std::max(20, canvasWidth));
        int canvasHeight = m_heightLEdit->text().toInt();
        setCanvasSize(QSize(canvasWidth, canvasHeight));
    });
    connect(m_widthLEdit, &FontsizeLineEdit::addSize, this, [=]{
        int canvasWidth = m_widthLEdit->text().toInt();
        canvasWidth = std::min(500000, std::max(20, canvasWidth + 1));
        int canvasHeight = m_heightLEdit->text().toInt();
        setCanvasSize(QSize(canvasWidth, canvasHeight));
    });
    connect(m_widthLEdit, &FontsizeLineEdit::reduceSize, this, [=]{
        int canvasWidth = m_widthLEdit->text().toInt();
        canvasWidth = std::min(500000, std::max(20, canvasWidth - 1));
        int canvasHeight = m_heightLEdit->text().toInt();
        setCanvasSize(QSize(canvasWidth, canvasHeight));
    });
    connect(m_heightLEdit, &FontsizeLineEdit::editingFinished, this, [=]{
        int canvasHeight = m_heightLEdit->text().toInt();
        canvasHeight = std::min(500000, std::max(20, canvasHeight));
        int canvasWidth = m_widthLEdit->text().toInt();
        setCanvasSize(QSize(canvasWidth, canvasHeight));
    });
    connect(m_heightLEdit, &FontsizeLineEdit::addSize, this, [=]{
        int canvasHeight = m_heightLEdit->text().toInt();
        canvasHeight = std::min(500000, std::max(20, canvasHeight + 1));
        int canvasWidth = m_widthLEdit->text().toInt();
        setCanvasSize(QSize(canvasWidth, canvasHeight));
    });
    connect(m_heightLEdit, &FontsizeLineEdit::reduceSize, this, [=]{
        int canvasHeight = m_heightLEdit->text().toInt();
        canvasHeight = std::min(500000, std::max(20, canvasHeight - 1));
        int canvasWidth = m_heightLEdit->text().toInt();
        setCanvasSize(QSize(canvasWidth, canvasHeight));
    });

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

    connect(cutTransAreaBtn, &ToolButton::clicked, this, &AdjustsizeWidget::autoCrop);
}

void AdjustsizeWidget::setCanvasSize(QSize size)
{
    m_widthLEdit->setText(QString("%1").arg(size.width()));
    m_heightLEdit->setText(QString("%1").arg(size.height()));
    ConfigSettings::instance()->setValue("artboard", "width",
                                         size.width());
    ConfigSettings::instance()->setValue("artboard", "height",
                                         size.height());
}

void AdjustsizeWidget::updateCanvasSize(QSize size)
{
    setCanvasSize(size);
}

void AdjustsizeWidget::resizeCanvasSize(bool resized, QSize addSize)
{

//    int originWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
//    int originHeight = ConfigSettings::instance()->value("artboard", "height").toInt();
//    originWidth += addSize.width();
//    originHeight += addSize.height();
    if (resized)
    {
        setCanvasSize(addSize);
        qDebug() << "ResizedSize:" << addSize;
    }
    else {
        m_widthLEdit->setText(QString("%1").arg(addSize.width()));
        m_heightLEdit->setText(QString("%1").arg(addSize.height()));
    }
}

AdjustsizeWidget::~AdjustsizeWidget()
{
}
