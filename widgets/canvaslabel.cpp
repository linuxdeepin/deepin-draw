#include "canvaslabel.h"

#include <QPainter>

CanvasLabel::CanvasLabel(QWidget *parent)
    : QLabel(parent) {

}

void CanvasLabel::setCanvasPixmap(QString imageFile) {
    m_currentFile = imageFile;
    m_currentPixmap = QPixmap(m_currentFile);
    repaint();
}

void CanvasLabel::setCanvasPixmap(QPixmap pixmap) {
    m_currentPixmap = pixmap;
    repaint();
}

void CanvasLabel::paintEvent(QPaintEvent *e) {
    QLabel::paintEvent(e);
    if (m_currentPixmap.isNull()) {
        return ;
    } else {
        setFixedSize(m_currentPixmap.size());
    }

    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_currentPixmap);
}

CanvasLabel::~CanvasLabel() {}
