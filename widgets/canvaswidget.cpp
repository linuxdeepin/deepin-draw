#include "canvaswidget.h"

#include <QHBoxLayout>

const int MARGIN = 20;

CanvasWidget::CanvasWidget(QWidget *parent)
    : QScrollArea(parent) {
    m_canvasLabel = new QLabel(this);
    m_canvasLabel->setMinimumSize(400, 400);
    m_canvasLabel->setStyleSheet("border: 1px solid rgba(0, 0, 0, 130);");

    this->setWidget(m_canvasLabel);
}

void CanvasWidget::setImage(QString filename) {
    m_currentFile = filename;
    QPixmap currentImage(m_currentFile);
    if (!currentImage.isNull()) {
        m_canvasLabel->setPixmap(QPixmap(m_currentFile));
        m_canvasLabel->setFixedSize(currentImage.size());
    }
}

CanvasWidget::~CanvasWidget() {

}
