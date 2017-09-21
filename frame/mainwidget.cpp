#include "mainwidget.h"

#include <QLabel>
#include <QDebug>

const int MARGIN = 25;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_canvas = new CanvasWidget(this);
    m_seperatorLine = new QLabel(this);
    m_seperatorLine->setMinimumWidth(this->width());
    m_seperatorLine->setFixedHeight(1);
    m_seperatorLine->setStyleSheet("border: 1px solid rgba(0, 0, 0, 30);");

    m_hLayout = new QHBoxLayout(this);
    m_hLayout->setContentsMargins(0, MARGIN, 0, MARGIN);
    m_hLayout->setSpacing(0);
    m_hLayout->addSpacing(MARGIN);
    m_hLayout->addWidget(m_canvas);
    m_hLayout->addSpacing(MARGIN);

    m_vLayout = new QVBoxLayout(this);
    m_vLayout->setMargin(0);
    m_vLayout->setSpacing(0);
    m_vLayout->addWidget(m_seperatorLine);
    m_vLayout->addSpacing(25);
    m_vLayout->addLayout(m_hLayout);
    m_vLayout->addSpacing(25);

    this->setLayout(m_vLayout);

    connect(this, &MainWidget::drawShapeChanged,
                    m_canvas, &CanvasWidget::drawShapeChanged);
    connect(this, &MainWidget::fillShapeSelectedActive,
            m_canvas, &CanvasWidget::fillShapeSelectedActive);
    connect(this, &MainWidget::rotateImage,
            m_canvas, &CanvasWidget::rotateImage);
    connect(this, &MainWidget::mirroredImage,
            m_canvas, &CanvasWidget::mirroredImage);
    connect(this, &MainWidget::saveImage,
            m_canvas, &CanvasWidget::saveImage);
}

void MainWidget::setImageInCanvas(QString imageFileName)
{
    m_canvas->setImage(imageFileName);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "MainWidget  resizeEvent:"
                     << this->width() << this->height();
    m_seperatorLine->setMinimumWidth(this->width());

    updateGeometry();
    QWidget::resizeEvent(event);
}

MainWidget::~MainWidget()
{
}
