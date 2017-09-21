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
   setStyleSheet("border: 1px solid rgba(0, 0, 0, 30);");

    m_shapesWidget = new ShapesWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_shapesWidget);
    setLayout(layout);

    connect(this, &CanvasWidget::drawShapeChanged, m_shapesWidget,
            &ShapesWidget::setCurrentShape);

//    connect(this, &CanvasWidget::rotateImage,
//            m_view, &ImageView::rotateImage);

//    connect(this, &CanvasWidget::mirroredImage,
//            m_view, &ImageView::mirroredImage);

//    connect(this, &CanvasWidget::saveImage,
//            m_view, &ImageView::saveImage);
}

bool CanvasWidget::overWindowSize()
{
    QSize windowSize = qApp->desktop()->geometry().size();
    QSize imageSize = QPixmap(m_currentFile).size();

    if (imageSize.width() > windowSize.width() ||
            imageSize.height() > windowSize.height())
    {
        m_scaledSize = fitWindowScaledSize(windowSize, imageSize);
        return true;
    } else
    {
        m_scaledSize = imageSize;
        return false;
    }
}

QSize CanvasWidget::fitWindowScaledSize(QSize windowSize, QSize imgSize)
{
    QSize tmpImgSize = QPixmap(m_currentFile).scaled(windowSize/2,
                                                     Qt::KeepAspectRatio).size();
    qreal wS = qreal(tmpImgSize.width())/qreal(windowSize.width()/2);
    qreal hS = qreal(tmpImgSize.height())/qreal(windowSize.height()/2);
    m_scaleValue = std::min(wS, hS);

    qDebug() << wS << hS << m_scaleValue;
    return tmpImgSize;
}

void CanvasWidget::setImage(QString filename)
{
    m_currentFile = filename;
}

CanvasWidget::~CanvasWidget()
{
}
