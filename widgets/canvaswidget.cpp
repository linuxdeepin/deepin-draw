#include "canvaswidget.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include <cmath>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QScrollArea(parent), m_scaleValue(1) {
    m_canvasLabel = new CanvasLabel(this);
    m_canvasLabel->setMinimumSize(10, 10);
    m_canvasLabel->setStyleSheet("border: 1px solid rgba(0, 0, 0, 130);");

    this->setWidget(m_canvasLabel);
    setAlignment(Qt::AlignCenter);

    connect(this, &CanvasWidget::requestInitShapeWidget,
                    m_canvasLabel, &CanvasLabel::initShapesWidget);
    connect(this, &CanvasWidget::changeShapeColor,
            m_canvasLabel, &CanvasLabel::setShapeColor);
    connect(this, &CanvasWidget::changeShapeLineWidth,
            m_canvasLabel, &CanvasLabel::setShapeLineWidth);
    connect(this, &CanvasWidget::changeLineShape,
            m_canvasLabel, &CanvasLabel::setLineShape);
    connect(this, &CanvasWidget::changeTextFontsize,
            m_canvasLabel, &CanvasLabel::setTextFontsize);
    connect(this, &CanvasWidget::changedBlurLinewidth,
            m_canvasLabel, &CanvasLabel::setBlurLinewidth);
}

bool CanvasWidget::overWindowSize() {
    QSize windowSize = qApp->desktop()->geometry().size();
    QSize imageSize = QPixmap(m_currentFile).size();
    if (imageSize.width() > windowSize.width() || imageSize.height() > windowSize.height()) {
        m_scaledSize = fitWindowScaledSize(windowSize, imageSize);
        return true;
    } else {
        m_scaledSize = imageSize;
        return false;
    }
}

QSize CanvasWidget::fitWindowScaledSize(QSize windowSize, QSize imgSize) {
    QSize tmpImgSize = QPixmap(m_currentFile).scaled(windowSize/2, Qt::KeepAspectRatio).size();
    qreal wS = qreal(tmpImgSize.width())/qreal(windowSize.width()/2);
    qreal hS = qreal(tmpImgSize.height())/qreal(windowSize.height()/2);
    m_scaleValue = std::min(wS, hS);
    qDebug() << wS << hS << m_scaleValue;
    return tmpImgSize;
}

void CanvasWidget::setImage(QString filename) {
    m_currentFile = filename;
    QPixmap currentImage(m_currentFile);
    if (!currentImage.isNull()) {
        if (!overWindowSize()) {
            m_canvasLabel->setCanvasPixmap(m_currentFile);
        } else {
            QPixmap tmpImage = QPixmap(m_currentFile).scaled(m_scaledSize, Qt::KeepAspectRatio);
            if (!tmpImage.isNull()) {
                m_canvasLabel->setCanvasPixmap(tmpImage);
            } else {
                qWarning() << "The current image is null!";
            }
        }
    }
}

void CanvasWidget::zoomInImage() {
    m_scaleValue  = m_scaleValue*(1-0.1);
    m_scaleValue = std::max(m_scaleValue, 0.02);
    QPixmap tmpImage = QPixmap(m_currentFile).scaled(m_scaledSize*m_scaleValue, Qt::KeepAspectRatio);
    m_canvasLabel->setCanvasPixmap(tmpImage);
}

void CanvasWidget::zoomOutImage() {
    m_scaleValue = m_scaleValue*(1+0.1);
    m_scaleValue = std::min(m_scaleValue, qreal(20));
    QSize originSize = QPixmap(m_currentFile).size();
    QPixmap tmpImage = QPixmap(m_currentFile).scaled(m_scaledSize*m_scaleValue, Qt::KeepAspectRatio);
    m_canvasLabel->setCanvasPixmap(tmpImage);
    qDebug() << "zoomOutImage:" << m_canvasLabel->size() << tmpImage.size()
             << tmpImage.isNull() << originSize*m_scaleValue << originSize << m_scaleValue;
}

CanvasWidget::~CanvasWidget() {
}
