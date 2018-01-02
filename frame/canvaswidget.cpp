#include "canvaswidget.h"

#include "utils/configsettings.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include <cmath>

const int MARGIN = 25;

CanvasWidget::CanvasWidget(QWidget *parent)
    : QLabel(parent)
    , m_scaleValue(1)
{
    setObjectName("CanvasBorder");
//    setStyleSheet("QLabel#CanvasBorder{border: 1px solid rgba(0, 0, 0, 80);}");
    setMouseTracking(true);
    m_shapesWidget = new ShapesWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(25);
    layout->setSpacing(0);
    layout->addWidget(m_shapesWidget);
    setLayout(layout);

    connect(this, &CanvasWidget::drawShapeChanged, m_shapesWidget,
            &ShapesWidget::setCurrentShape);
    connect(this, &CanvasWidget::fillShapeSelectedActive, m_shapesWidget,
            &ShapesWidget::setFillShapeSelectedActive);
    connect(this, &CanvasWidget::rotateImage,
            m_shapesWidget,  &ShapesWidget::handleImageRotate);
    connect(this, &CanvasWidget::mirroredImage,
            m_shapesWidget, &ShapesWidget::mirroredImage);
    connect(this, &CanvasWidget::generateSaveImage,
            m_shapesWidget, &ShapesWidget::saveImage);
    connect(this, &CanvasWidget::printImage,
            m_shapesWidget, &ShapesWidget::printImage);
    connect(this, &CanvasWidget::autoCrop, m_shapesWidget,
            &ShapesWidget::autoCrop);
    connect(this, &CanvasWidget::pressToShapeWidget, m_shapesWidget,
            &ShapesWidget::pressFromParent);
//    connect(this, &CanvasWidget::releaseToShapeWidget, m_shapesWidget,
//            &ShapesWidget::releaseFromParent);

    connect(m_shapesWidget, &ShapesWidget::updateMiddleWidgets,
            this, &CanvasWidget::updateMiddleWidget);
    connect(m_shapesWidget, &ShapesWidget::adjustArtBoardSize,
            this,  [=](QSize size){
        ConfigSettings::instance()->setValue("artboard", "width", size.width());
        ConfigSettings::instance()->setValue("artboard", "height", size.height());
        emit adjustArtBoardSize(size);
    });
    connect(m_shapesWidget, &ShapesWidget::cutImageFinished,
            this, &CanvasWidget::cutImageFinished);
    connect(m_shapesWidget, &ShapesWidget::shapePressed,
            this, &CanvasWidget::shapePressed);
}

void CanvasWidget::paintEvent(QPaintEvent *)
{
    QPoint endPos = this->rect().bottomRight();
    int outSideSpacing = 20;
    QPoint pointA = QPoint(endPos.x() - outSideSpacing, endPos.y() - outSideSpacing);
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

void CanvasWidget::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    qDebug() << "CanvasWidget mousePressEvent trigger!";
    emit pressToShapeWidget(ev);
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    emit releaseToShapeWidget(ev);
}

void CanvasWidget::openImage(const QString &path)
{
    m_shapesWidget->loadImage(QStringList() << path);
}

void CanvasWidget::setShapes(QList<Toolshape> shapes)
{
    m_shapesWidget->setShapes(shapes);
}

CanvasWidget::~CanvasWidget()
{
}
