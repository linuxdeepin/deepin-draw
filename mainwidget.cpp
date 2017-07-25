#include "mainwidget.h"

#include <QLabel>
#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
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

    connect(this, &MainWidget::zoomOutAction, m_canvas, &CanvasWidget::zoomOutImage);
    connect(this, &MainWidget::zoomInAction, m_canvas, &CanvasWidget::zoomInImage);
    connect(this, &MainWidget::prepareInitShapesWidget,
            m_canvas, &CanvasWidget::requestInitShapeWidget);
}

void MainWidget::setImageInCanvas(QString imageFileName) {
    m_canvas->setImage(imageFileName);
}

void MainWidget::resizeEvent(QResizeEvent *event) {
    qDebug() << "MainWidget sssssss" << this->width() << this->height();
    m_seperatorLine->setMinimumWidth(this->width());
    m_canvas->setMinimumSize(this->width(), this->height() - 1);
    this->updateGeometry();
}

MainWidget::~MainWidget() {
}
