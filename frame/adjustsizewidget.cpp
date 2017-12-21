#include "adjustsizewidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"
#include "utils/dintvalidator.h"

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

    int canvasWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
    int canvasHeight = ConfigSettings::instance()->value("artboard", "height").toInt();

    if (canvasWidth == 0 || canvasHeight == 0)
    {
        QSize desktopSize = qApp->desktop()->size();
        canvasWidth = desktopSize.width();
        canvasHeight = desktopSize.height();
    }

    setCanvasSize(QSize(canvasWidth, canvasHeight));

    connect(m_widthLEdit, &FontsizeLineEdit::editingFinished, this, [=]{
        int canvasWidth = m_widthLEdit->text().toInt();
        canvasWidth = std::min(500000, std::max(20, canvasWidth));
        m_widthLEdit->setText(QString("%1").arg(canvasWidth));
        ConfigSettings::instance()->setValue("artboard", "width", canvasWidth);
    });
    connect(m_widthLEdit, &FontsizeLineEdit::addSize, this, [=]{
        int canvasWidth = m_widthLEdit->text().toInt();
        canvasWidth = std::min(500000, std::max(20, canvasWidth + 1));
        m_widthLEdit->setText(QString("%1").arg(canvasWidth));
    });
    connect(m_widthLEdit, &FontsizeLineEdit::reduceSize, this, [=]{
        int canvasWidth = m_widthLEdit->text().toInt();
        canvasWidth = std::min(500000, std::max(20, canvasWidth - 1));
        m_widthLEdit->setText(QString("%1").arg(canvasWidth));
    });

    connect(m_heightLEdit, &FontsizeLineEdit::editingFinished, this, [=]{
        int canvasHeight = m_heightLEdit->text().toInt();
        canvasHeight = std::min(500000, std::max(20, canvasHeight));
        m_heightLEdit->setText(QString("%1").arg(canvasHeight));
        ConfigSettings::instance()->setValue("artboard", "height", canvasHeight);
    });
    connect(m_heightLEdit, &FontsizeLineEdit::addSize, this, [=]{
        int canvasHeight = m_heightLEdit->text().toInt();
        canvasHeight = std::min(500000, std::max(20, canvasHeight + 1));
        m_heightLEdit->setText(QString("%1").arg(canvasHeight));
    });
    connect(m_heightLEdit, &FontsizeLineEdit::reduceSize, this, [=]{
        int canvasHeight = m_heightLEdit->text().toInt();
        canvasHeight = std::min(500000, std::max(20, canvasHeight - 1));
        m_heightLEdit->setText(QString("%1").arg(canvasHeight));
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

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged,
            this, [=](const QString &group, const QString &key){
        Q_UNUSED(key);
        if (group == "artboard")
        {
            int canvasWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
            int canvasHeight = ConfigSettings::instance()->value("artboard", "height").toInt();

            if (canvasWidth == 0|| canvasHeight == 0)
            {
                QSize desktopSize = qApp->desktop()->size();
                canvasWidth = desktopSize.width();
                canvasHeight = desktopSize.height();
            }
            setCanvasSize(QSize(canvasWidth, canvasHeight));
        }
    });
    connect(cutTransAreaBtn, &ToolButton::clicked, this, &AdjustsizeWidget::autoCrop);
}

void AdjustsizeWidget::setCanvasSize(QSize size)
{
    m_widthLEdit->setText(QString("%1").arg(size.width()));
    m_heightLEdit->setText(QString("%1").arg(size.height()));
}

void AdjustsizeWidget::updateCanvasSize(QSize size)
{
    setCanvasSize(size);
}

AdjustsizeWidget::~AdjustsizeWidget()
{
}
