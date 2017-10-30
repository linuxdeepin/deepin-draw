#include "mainwidget.h"

#include <QLabel>
#include <QDebug>

#include "utils/configsettings.h"

int HOR_MARGIN = 25;
int VER_MARGIN = 25;
const int ARTBOARD_MARGIN = 25;
const int TITLEBAR_HEIGHT = 40;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_canvas = new CanvasWidget(this);
    m_seperatorLine = new QLabel(this);
    m_seperatorLine->setMinimumWidth(this->width());
    m_seperatorLine->setFixedHeight(1);
    m_seperatorLine->setStyleSheet("border: 1px solid rgba(0, 0, 0, 30);");

    updateLayout();

    m_hLayout = new QHBoxLayout;
    m_hLayout->setMargin(0);
    m_hLayout->setSpacing(0);
    m_hLayout->addStretch();
    m_hLayout->addWidget(m_canvas);
    m_hLayout->addStretch();

    m_vLayout = new QVBoxLayout;
    m_vLayout->addWidget(m_seperatorLine);
    m_vLayout->addStretch();
    m_vLayout->addLayout(m_hLayout);
    m_vLayout->addStretch();

    this->setLayout(m_vLayout);

    connect(this, &MainWidget::drawShapeChanged,
                    m_canvas, &CanvasWidget::drawShapeChanged);
    connect(this, &MainWidget::fillShapeSelectedActive,
            m_canvas, &CanvasWidget::fillShapeSelectedActive);
    connect(this, &MainWidget::rotateImage,
            m_canvas, &CanvasWidget::rotateImage);
    connect(this, &MainWidget::mirroredImage,
            m_canvas, &CanvasWidget::mirroredImage);
    connect(this, &MainWidget::saveImageAction,
            m_canvas, &CanvasWidget::saveImageAction);
    connect(this, &MainWidget::autoCrop,
            m_canvas, &CanvasWidget::autoCrop);

    connect(m_canvas, &CanvasWidget::updateMiddleWidget,
            this, &MainWidget::updateMiddleWidget);
    connect(m_canvas, &CanvasWidget::adjustArtBoardSize,
            this, &MainWidget::adjustArtBoardSize);
    connect(m_canvas, &CanvasWidget::cutImageFinished,
            this, &MainWidget::cutImageFinished);

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            [=](const QString &group,  const QString &key){
        Q_UNUSED(key);
        if (group == "artboard")
        {
            updateLayout();
            updateGeometry();
        }
    });
}

void MainWidget::updateLayout()
{
    int artboardActualWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
    int artboardActualHeight = ConfigSettings::instance()->value("artboard", "height").toInt();
    int artboardWindowWidth, artboardWindowHeight;

    if (artboardActualWidth == 0|| artboardActualHeight == 0)
    {
        QSize desktopSize = qApp->desktop()->size();
        artboardActualWidth = desktopSize.width();
        artboardActualHeight = desktopSize.height();
    }

    if (artboardActualWidth <= window()->width() - ARTBOARD_MARGIN*2)
    {
        HOR_MARGIN = (window()->width() -  artboardActualWidth)/2;
    } else
    {
        HOR_MARGIN = ARTBOARD_MARGIN;
    }
    if (artboardActualHeight <= window()->height() - ARTBOARD_MARGIN*2 - TITLEBAR_HEIGHT)
    {
        VER_MARGIN = (window()->height() - artboardActualHeight - TITLEBAR_HEIGHT)/2;
    } else {
        VER_MARGIN = ARTBOARD_MARGIN;
    }

    HOR_MARGIN = std::max(HOR_MARGIN, ARTBOARD_MARGIN);
    VER_MARGIN = std::max(VER_MARGIN, ARTBOARD_MARGIN);

    artboardWindowWidth = window()->width() - HOR_MARGIN*2;
    artboardWindowHeight = window()->height() - TITLEBAR_HEIGHT - VER_MARGIN*2;

    m_canvas->setFixedSize(artboardWindowWidth, artboardWindowHeight);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "MainWidget  resizeEvent:"
                     << this->width() << this->height();
    m_seperatorLine->setMinimumWidth(this->width());

    updateLayout();
//    updateGeometry();
    QWidget::resizeEvent(event);
}

MainWidget::~MainWidget()
{
}
