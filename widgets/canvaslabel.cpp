#include "canvaslabel.h"

#include <QPainter>

CanvasLabel::CanvasLabel(QWidget *parent)
    : QLabel(parent) {
}

void CanvasLabel::setCanvasPixmap(QString imageFile) {
    m_currentFile = imageFile;
    m_currentPixmap = QPixmap(m_currentFile);
    if (!m_currentPixmap.isNull()) {
        update();
    } else {
        qWarning() << "m_currentPixmap is null";
    }
}

void CanvasLabel::setCanvasPixmap(QPixmap pixmap) {
    m_currentPixmap = pixmap;
    if (!m_currentPixmap.isNull()) {
        update();
    } else {
        qWarning() << "m_currentPixmap is null";
    }
}

void CanvasLabel::initShapesWidget(QString shape, bool needInited) {
    if (needInited) {
        m_shapesWidget = new ShapesWidget(this);
    }
    m_shapesWidget->setCurrentShape(shape);
    m_shapesWidget->resize(this->width(), this->height());
    m_shapesWidget->move(0, 0);
    m_shapesWidget->show();
    qDebug() << "CanvasLabel initShapesWidget.." << m_shapesWidget->geometry();
}

void CanvasLabel::paintEvent(QPaintEvent *e) {
    QLabel::paintEvent(e);
    if (m_currentPixmap.isNull()) {
        return ;
    } else {
        setFixedSize(m_currentPixmap.size());
//        if (m_shapesWidgetExist) {
//            m_shapesWidget->resize(this->width(), this->height());
//        }
    }

    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_currentPixmap);
}

CanvasLabel::~CanvasLabel() {}
