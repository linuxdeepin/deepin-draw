#include "mainwidget.h"

#include <QLabel>
#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent) {
    m_canvas = new CanvasWidget(this);
    m_seperatorLine = new QLabel(this);
    m_seperatorLine->setMinimumWidth(this->width());
    m_seperatorLine->setFixedHeight(1);
    m_seperatorLine->setStyleSheet("border: 1px solid rgba(0, 0, 0, 30);");

    m_vLayout = new QVBoxLayout(this);
    m_vLayout->setMargin(0);
    m_vLayout->setSpacing(0);
    m_vLayout->addWidget(m_seperatorLine);
    m_vLayout->addStretch();
    m_vLayout->addWidget(m_canvas, 0, Qt::AlignHCenter);
    m_vLayout->addStretch();
    setLayout(m_vLayout);

    connect(this, &MainWidget::initShapesWidget,
                    m_canvas, &CanvasWidget::initShapeWidget);
    connect(this, &MainWidget::rotateImage,
            m_canvas, &CanvasWidget::rotateImage);
    connect(this, &MainWidget::mirroredImage,
            m_canvas, &CanvasWidget::mirroredImage);
}

void MainWidget::setImageInCanvas(QString imageFileName) {
    m_canvas->setImage(imageFileName);
}

void MainWidget::resizeEvent(QResizeEvent *event) {
    qDebug() << "MainWidget  resizeEvent:"
                     << this->width() << this->height();
    m_seperatorLine->setMinimumWidth(this->width());
    m_canvas->setMinimumSize(this->width(), this->height() - 1);
    this->updateGeometry();
    QWidget::resizeEvent(event);
}

MainWidget::~MainWidget() {
}
