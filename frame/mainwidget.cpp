#include "mainwidget.h"

#include <QLabel>
#include <QDebug>

#include "utils/global.h"
#include "utils/configsettings.h"
#include "utils/calculaterect.h"
#include "widgets/graphicsgloweffect.h"

DWIDGET_USE_NAMESPACE

const int ARTBOARD_MARGIN = 25;
const int TITLEBAR_HEIGHT = 40;
const int MARGIN = 25;

MainWidget::MainWidget(QWidget *parent)
    : QLabel(parent)
{
    DRAW_THEME_INIT_WIDGET("MainWidget");
    this->setObjectName("MainWidget");
    m_shapesWidget = new ShapesWidget(this);
    m_artboardMPoints = initFourPoints(m_artboardMPoints);

    auto effect = new GraphicsGlowEffect();
    effect->setBlurRadius(16);
    effect->setColor(QColor("#ececef"));
    m_shapesWidget->setGraphicsEffect(effect);

    m_seperatorLine = new QLabel(this);
    m_seperatorLine->setMinimumWidth(this->width());
    m_seperatorLine->setFixedHeight(1);
    m_seperatorLine->setStyleSheet("border: 1px solid rgba(0, 0, 0, 12);");

    m_resizeLabel = new ResizeLabel(this);
    m_resizeLabel->setFixedSize(this->size());
    m_resizeLabel->hide();

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
            [=](bool drawing, FourPoints mainPoints, QSize addSize){
        mainPoints[0] = QPointF(mainPoints[0].x() + m_horizontalMargin,
                mainPoints[0].y() + m_verticalMargin);
        mainPoints[3] = QPointF(mainPoints[3].x() + m_horizontalMargin,
                mainPoints[3].y() + m_verticalMargin);
        m_resizeLabel->paintResizeLabel(drawing, mainPoints);
        emit resizeArtboard(!drawing, addSize);
        update();
    });

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            [=](const QString &group,  const QString &key){
        Q_UNUSED(key);
        if (group == "artboard")
        {
            QTimer::singleShot(500, [=]{
                updateLayout();
                updateGeometry();
            });

        }
    });
}

void MainWidget::updateLayout()
{
    QSize artboardSize = getArtboardSize(this->cursor().pos());
    m_horizontalMargin = 0, m_verticalMargin = 0;
    int artboardWindowWidth = 0, artboardWindowHeight = 0;
    qDebug() << "updateLayout:" <<  qApp->activeWindow()
             << window()->size() << this->size() << artboardSize;
    if (artboardSize.width() <= window()->width() - ARTBOARD_MARGIN*2)
    {
        m_horizontalMargin = (window()->width() -  artboardSize.width())/2;
    } else
    {
        m_horizontalMargin = ARTBOARD_MARGIN;
    }

    if (artboardSize.height() <= window()->height() - TITLEBAR_HEIGHT - ARTBOARD_MARGIN*2)
    {
        m_verticalMargin = (window()->height() -artboardSize.height() - TITLEBAR_HEIGHT)/2;
    } else {
        m_verticalMargin = ARTBOARD_MARGIN;
    }

    artboardWindowWidth = window()->width() - m_horizontalMargin*2;
    artboardWindowHeight = window()->height() - TITLEBAR_HEIGHT
            - m_verticalMargin*2;
    qDebug() << "before window size:" << QSize(artboardWindowWidth,
                                               artboardWindowHeight);
    QSize winSize = getCanvasSize(artboardSize, QSize(artboardWindowWidth,
                                                      artboardWindowHeight));
    artboardWindowWidth = winSize.width();
    artboardWindowHeight = winSize.height();
    m_horizontalMargin = (window()->width() - artboardWindowWidth)/2;
    m_verticalMargin = (window()->height() - artboardWindowHeight - TITLEBAR_HEIGHT)/2;

    ConfigSettings::instance()->setValue("canvas", "width", artboardWindowWidth);
    ConfigSettings::instance()->setValue("canvas", "height", artboardWindowHeight);
    m_shapesWidget->setFixedSize(artboardWindowWidth, artboardWindowHeight);
    m_shapesWidget->updateCanvasSize();
    qDebug() << "UpdateNewLayout:" << artboardWindowWidth << artboardWindowHeight;
    m_resizeLabel->setFixedSize(this->size());
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "MainWidget  resizeEvent:"
                     << this->width() << this->height();
    m_seperatorLine->setMinimumWidth(this->width());

    updateLayout();

    QLabel::resizeEvent(event);
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    m_shapesWidget->pressFromParent(event);
    QLabel::mousePressEvent(event);
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPoint endPos = this->rect().bottomRight();
    int outSideSpacing = 10;
    QPoint pointA = QPoint(endPos.x() - m_horizontalMargin + outSideSpacing,
                           endPos.y() - m_verticalMargin + outSideSpacing);
    int tipsWidth = 8, tipContentWidth = 2;
    QPoint pointB = QPoint(pointA.x(), pointA.y() - tipsWidth);
    QPoint pointC = QPoint(pointB.x() - tipContentWidth, pointB.y());
    QPoint pointD = QPoint(pointC.x(), pointC.y() + (tipsWidth - tipContentWidth));
    QPoint pointE = QPoint(pointD.x() - (tipsWidth - tipContentWidth), pointD.y());
    QPoint pointF = QPoint(pointE.x(), pointE.y() + tipContentWidth);

    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor(0, 0, 0, 80));
    QPainterPath path;
    path.moveTo(pointE);
    path.lineTo(pointF);
    path.lineTo(pointA);
    path.lineTo(pointB);
    path.lineTo(pointC);
    path.lineTo(pointD);
    path.lineTo(pointE);
    painter.setPen(pen);

    painter.fillPath(path, QBrush(QColor(255, 255, 255, 255)));
    painter.drawPath(path);
}

void MainWidget::openImage(const QString &path)
{
    QTimer::singleShot(500, [=]{
         m_shapesWidget->loadImage(QStringList() << path);
    });

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
