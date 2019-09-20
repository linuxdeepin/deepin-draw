#include "calphacontrolwidget.h"

#include <QHBoxLayout>

CAlphaControlWidget::CAlphaControlWidget(DWidget *parent)
    : DWidget(parent)
    , m_isUserOperation(false)
{
    initUI();
    initConnection();
}

void CAlphaControlWidget::updateAlphaControlWidget(int alpha)
{
    int percentValue = (float)alpha / 255 * 100;
    m_alphaSlider->setValue(percentValue);
}

void CAlphaControlWidget::initUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);



    DLabel *nameLabel = new DLabel(this);
    nameLabel->setText(tr("Alpha"));


    m_alphaLabel = new DLabel(this);
    m_alphaLabel->setFixedWidth(38);
    m_alphaLabel->setText("0%");


    m_alphaSlider = new DSlider(Qt::Horizontal, this);
    m_alphaSlider->setMinimum(0);
    m_alphaSlider->setMaximum(100);
//    m_alphaSlider->slider()->setOrientation(Qt::Horizontal);
    m_alphaSlider->setFixedWidth(130);

//    m_alphaSlider->setStyleSheet("background-color: rgb(0, 255, 0);");

    layout->addWidget(nameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_alphaSlider);
    layout->addSpacing(10);
    layout->addWidget(m_alphaLabel);

    setLayout(layout);

}

void CAlphaControlWidget::initConnection()
{
    connect(m_alphaSlider, &DSlider::valueChanged, this, [ = ](int value) {
        int trueValue =  (float)value * 255 / 100;
        m_alphaLabel->setText(QString("%1%").arg(value));
        if (m_isUserOperation) {
            emit signalAlphaChanged(trueValue);
        }
    });

    connect(m_alphaSlider, &DSlider::sliderPressed, this, [ = ]() {
        m_isUserOperation = true;
    });

    connect(m_alphaSlider, &DSlider::sliderReleased, this, [ = ]() {
        m_isUserOperation = false;
    });
}
