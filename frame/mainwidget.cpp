#include "mainwidget.h"

#include <QLabel>
#include <QDebug>

#include "utils/configsettings.h"
#include "utils/calculaterect.h"
#include "widgets/graphicsgloweffect.h"

DWIDGET_USE_NAMESPACE

int HOR_MARGIN = 0;
int VER_MARGIN = 0;
const int ARTBOARD_MARGIN = 25;
const int TITLEBAR_HEIGHT = 40;
const int MARGIN = 25;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("MainWidget");
    m_shapesWidget = new ShapesWidget(this);
    m_artboardMPoints = initFourPoints(m_artboardMPoints);

    auto effect = new GraphicsGlowEffect();
    effect->setBlurRadius(16);
    effect->setColor(QColor("#ececef"));
    m_shapesWidget->setGraphicsEffect(effect);

    m_seperatorLine = new QLabel(this);
    m_seperatorLine->setMinimumWidth(this->width());
    m_seperatorLine->setFixedHeight(1);
    m_seperatorLine->setStyleSheet("border: 1px solid rgba(0, 0, 0, 30);");

    m_resizeLabel = new ResizeLabel(this);
    m_resizeLabel->setFixedSize(this->size());
    m_resizeLabel->hide();

    updateLayout();

    m_hLayout = new QHBoxLayout;
    m_hLayout->setMargin(0);
    m_hLayout->setSpacing(0);
    m_hLayout->addStretch();
    m_hLayout->addWidget(m_shapesWidget);
    m_hLayout->addStretch();

    m_vLayout = new QVBoxLayout;
    m_vLayout->setMargin(0);
    m_vLayout->setSpacing(0);
    m_vLayout->addWidget(m_seperatorLine);
    m_vLayout->addStretch();
    m_vLayout->addSpacing(MARGIN);
    m_vLayout->addLayout(m_hLayout);
    m_vLayout->addSpacing(MARGIN);
    m_vLayout->addStretch();

    this->setLayout(m_vLayout);

    connect(this, &MainWidget::drawShapeChanged,
                    m_shapesWidget, &ShapesWidget::setCurrentShape);
    connect(this, &MainWidget::fillShapeSelectedActive,
            m_shapesWidget, &ShapesWidget::setFillShapeSelectedActive);
    connect(this, &MainWidget::rotateImage,
            m_shapesWidget, &ShapesWidget::handleImageRotate);
    connect(this, &MainWidget::mirroredImage,
            m_shapesWidget, &ShapesWidget::mirroredImage);
    connect(this, &MainWidget::generateSaveImage,
            m_shapesWidget, &ShapesWidget::saveImage);
    connect(this, &MainWidget::printImage,
            m_shapesWidget, &ShapesWidget::printImage);
    connect(this, &MainWidget::autoCrop,
            m_shapesWidget, &ShapesWidget::autoCrop);
    connect(this, &MainWidget::pressToCanvas,
            m_shapesWidget, &ShapesWidget::pressFromParent);

    connect(m_shapesWidget, &ShapesWidget::updateMiddleWidgets,
            this, &MainWidget::updateMiddleWidget);
    connect(m_shapesWidget, &ShapesWidget::adjustArtBoardSize,
            this,  [=](QSize size){
        ConfigSettings::instance()->setValue("artboard", "width", size.width());
        ConfigSettings::instance()->setValue("artboard", "height", size.height());
        emit adjustArtBoardSize(size);
    });
    connect(m_shapesWidget, &ShapesWidget::cutImageFinished,
            this, &MainWidget::cutImageFinished);
    connect(m_shapesWidget, &ShapesWidget::shapePressed,
            this, &MainWidget::shapePressed);
    connect(m_shapesWidget, &ShapesWidget::drawArtboard, this,
            [=](bool drawing, FourPoints mainPoints){
        QSize artboardSize = m_shapesWidget->size();
        int startX = (this->width() - artboardSize.width())/2;
        int startY = (this->height() - artboardSize.height())/2;

        mainPoints[0] = QPointF(mainPoints[0].x(),
                mainPoints[0].y()  - qreal(startY));
        mainPoints[3] = QPointF(mainPoints[3].x(),
                mainPoints[3].y() - qreal(startY));
        m_resizeLabel->paintResizeLabel(drawing, mainPoints);
        update();
    });

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

    if (artboardActualWidth == 0 || artboardActualHeight == 0)
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

    qreal xRation = artboardWindowWidth/qreal(artboardActualWidth);
    qreal yRation = artboardWindowHeight/qreal(artboardActualHeight);

    qreal resultRation = std::min(xRation, yRation);
    artboardWindowWidth = resultRation*artboardActualWidth;
    artboardWindowHeight = resultRation*artboardActualHeight;

    ConfigSettings::instance()->setValue("canvas", "width", artboardWindowWidth);
    ConfigSettings::instance()->setValue("canvas", "height", artboardWindowHeight);
    m_shapesWidget->setFixedSize(artboardWindowWidth, artboardWindowHeight - MARGIN);
    m_resizeLabel->setFixedSize(this->size());
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "MainWidget  resizeEvent:"
                     << this->width() << this->height();
    m_seperatorLine->setMinimumWidth(this->width());

    updateLayout();
    QWidget::resizeEvent(event);
}

//void MainWidget::paintEvent(QPaintEvent *event)
//{
////    Q_UNUSED(event);
//    QWidget::paintEvent(event);
//    if (!m_drawArtboard)
//        return;

//    else {
//        QPainter painter(this);
//        painter.setRenderHints(QPainter::Antialiasing
//                               | QPainter::SmoothPixmapTransform);
//        painter.setBrush(Qt::transparent);
//        painter.setPen(QPen(QColor(0, 0, 0, 150)));
//        QRect rect = QRect(int(m_artboardMPoints[0].x() + 80), int(m_artboardMPoints[0].y() + 35),
//                int(m_artboardMPoints[3].x() - m_artboardMPoints[0].x()),
//                int(m_artboardMPoints[3].y() - m_artboardMPoints[0].y()));
//        painter.drawRect(rect);
//    }
//}

void MainWidget::openImage(const QString &path)
{
     m_shapesWidget->loadImage(QStringList() << path);
}

void MainWidget::updateCanvasSize(const QSize &size)
{
    m_shapesWidget->setFixedSize(size);
}

void MainWidget::initShapes(QList<Toolshape> shapes)
{
    m_shapesWidget->setShapes(shapes);
}

int MainWidget::shapeNum() const
{
    return m_shapesWidget->shapesNum();
}

MainWidget::~MainWidget()
{
}
