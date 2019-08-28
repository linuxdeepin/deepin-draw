#include "calphacontrolwidget.h"

#include <QHBoxLayout>

CAlphaControlWidget::CAlphaControlWidget(QWidget *parent)
    : DWidget(parent)
    , m_isUserOperation(false)
{
    initUI();
    initConnection();
}

void CAlphaControlWidget::updateAlphaControlWidget(int alpha)
{
    int percentValue = alpha / 255 * 100;
    m_alphaSlider->setValue(percentValue);
}

void CAlphaControlWidget::initUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    QFont font;
    font.setPointSize(8);

    DLabel *nameLabel = new DLabel(this);
    nameLabel->setText(tr("Alpha"));
    nameLabel->setFont(font);

    m_alphaLabel = new DLabel(this);
    m_alphaLabel->setFixedWidth(38);
    m_alphaLabel->setFont(font);
    m_alphaLabel->setText("0%");
//    m_alphaLabel->setStyleSheet("background-color: rgb(255, 0, 0);");

    m_alphaSlider = new DSlider(this);
    m_alphaSlider->setMinimum(0);
    m_alphaSlider->setMaximum(100);
    m_alphaSlider->setMinimumWidth(140);
    m_alphaSlider->setOrientation(Qt::Horizontal);
//    m_alphaSlider->setStyleSheet("background-color: rgb(0, 255, 0);");

    layout->addStretch();
    layout->addWidget(nameLabel);

    layout->addSpacing(18);
    layout->addWidget(m_alphaSlider);
    layout->addSpacing(5);
    layout->addWidget(m_alphaLabel);

    setLayout(layout);

}

void CAlphaControlWidget::initConnection()
{
    connect(m_alphaSlider, &DSlider::valueChanged, this, [ = ](int value) {
        int trueValue =  value * 255 / 100;
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
