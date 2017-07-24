#include "canvaswidget.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

const int MARGIN = 20;

CanvasWidget::CanvasWidget(QWidget *parent)
    : QScrollArea(parent) {
    m_canvasLabel = new QLabel(this);
    m_canvasLabel->setMinimumSize(10, 10);
    m_canvasLabel->setStyleSheet("border: 1px solid rgba(0, 0, 0, 130);");

    this->setWidget(m_canvasLabel);
    setAlignment(Qt::AlignCenter);
}

bool CanvasWidget::overWindowSize() {
    QSize windowSize = qApp->desktop()->geometry().size();
    QSize imageSize = QPixmap(m_currentFile).size();
    if (imageSize.width() > windowSize.width() || imageSize.height() > windowSize.height()) {
        m_scaledSize = fitWindowScaledSize(windowSize, imageSize);
        return true;
    } else {
        return false;
    }
}

QSize CanvasWidget::fitWindowScaledSize(QSize windowSize, QSize imgSize) {
        QSize tmpImgSize = QPixmap(m_currentFile).scaled(windowSize/2, Qt::KeepAspectRatio).size();
        qreal wS = tmpImgSize.width()/(windowSize.width()/2);
        qreal hS = tmpImgSize.height()/(windowSize.height()/2);
        m_scaleValue = std::min(wS, hS);
        return tmpImgSize;
}

void CanvasWidget::setImage(QString filename) {
    m_currentFile = filename;
    QPixmap currentImage(m_currentFile);
    if (!currentImage.isNull()) {
        if (!overWindowSize()) {
            m_canvasLabel->setPixmap(QPixmap(m_currentFile));
            m_canvasLabel->setFixedSize(currentImage.size());
        } else {
            QPixmap tmpImage = QPixmap(m_currentFile).scaled(m_scaledSize, Qt::KeepAspectRatio);
            m_canvasLabel->setPixmap(QPixmap(tmpImage));
            m_canvasLabel->setFixedSize(tmpImage.size());
        }


    }
}

CanvasWidget::~CanvasWidget() {

}
