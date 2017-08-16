#include "canvaswidget.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include <cmath>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent), m_scaleValue(1) {
    m_view = new ImageView(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_view);
    setLayout(layout);

    connect(this, &CanvasWidget::initShapeWidget,
                   m_view, &ImageView::initShapesWidget);

    connect(this, &CanvasWidget::shapeColorChanged,
            m_view, &ImageView::updateShapesColor);

    connect(this, &CanvasWidget::shapeLineWidthChanged,
           m_view, &ImageView::updateShapesLineWidth);

    connect(this, &CanvasWidget::lineShapeChanged,
            m_view, &ImageView::updateLineShapes);

    connect(this, &CanvasWidget::textFontsizeChanged,
            m_view, &ImageView::updateTextFontsize);

    connect(this, &CanvasWidget::blurLinewidthChanged,
            m_view, &ImageView::updateBlurLinewidth);

    connect(this, &CanvasWidget::rotateImage,
            m_view, &ImageView::rotateImage);

    connect(this, &CanvasWidget::mirroredImage,
            m_view, &ImageView::mirroredImage);

    connect(this, &CanvasWidget::cutImage, m_view, &ImageView::cutImage);
}

bool CanvasWidget::overWindowSize() {
    QSize windowSize = qApp->desktop()->geometry().size();
    QSize imageSize = QPixmap(m_currentFile).size();
    if (imageSize.width() > windowSize.width() ||
            imageSize.height() > windowSize.height()) {
        m_scaledSize = fitWindowScaledSize(windowSize, imageSize);
        return true;
    } else {
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

void CanvasWidget::setImage(QString filename) {
    m_currentFile = filename;
    m_view->setImage(filename);
}

CanvasWidget::~CanvasWidget() {
}
