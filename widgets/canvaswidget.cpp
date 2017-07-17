#include "canvaswidget.h"

#include <QHBoxLayout>

const int MARGIN = 20;

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent) {
    m_canvasLabel = new QLabel(this);
    m_canvasLabel->setMinimumSize(400, 400);
    m_canvasLabel->setStyleSheet("border: 2px solid blue;");
    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    mLayout->addSpacing(0);
    mLayout->addWidget(m_canvasLabel);

    setLayout(mLayout);
}

CanvasWidget::~CanvasWidget() {

}
