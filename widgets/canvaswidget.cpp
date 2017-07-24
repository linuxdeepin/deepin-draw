#include "canvaswidget.h"

#include <QHBoxLayout>

const int MARGIN = 20;

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent) {
    m_canvasLabel = new QLabel(this);
    m_canvasLabel->setMinimumSize(400, 400);
    m_canvasLabel->setStyleSheet("border: 1px solid rgba(0, 0, 0, 130);");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    mLayout->addSpacing(0);
    mLayout->addWidget(m_canvasLabel);

    setLayout(mLayout);
}

void CanvasWidget::setImage(QString filename) {
    m_currentFile = filename;
    m_canvasLabel->setPixmap(QPixmap(m_currentFile));
}

CanvasWidget::~CanvasWidget() {

}
