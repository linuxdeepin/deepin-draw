#include "canvaswidget.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include <cmath>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QLabel(parent)
    , m_scaleValue(1)
{
//   setStyleSheet("border: 1px solid rgba(0, 0, 0, 30); background-color: green;");
    m_shapesWidget = new ShapesWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
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
//    connect(this, &CanvasWidget::saveImageAction,
//            m_shapesWidget, &ShapesWidget::saveImage);
    connect(this, &CanvasWidget::generateSaveImage,
            m_shapesWidget, &ShapesWidget::saveImage);
    connect(this, &CanvasWidget::autoCrop, m_shapesWidget, &ShapesWidget::autoCrop);

    connect(m_shapesWidget, &ShapesWidget::updateMiddleWidgets,
            this, &CanvasWidget::updateMiddleWidget);
    connect(m_shapesWidget, &ShapesWidget::adjustArtBoardSize,
            this, &CanvasWidget::adjustArtBoardSize);
    connect(m_shapesWidget, &ShapesWidget::cutImageFinished,
            this, &CanvasWidget::cutImageFinished);

}

void CanvasWidget::openImage(QString path)
{
    m_shapesWidget->loadImage(QStringList() << path);
}

CanvasWidget::~CanvasWidget()
{
}
