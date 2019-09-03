#include "cpicturewidget.h"
#include "widgets/cclickbutton.h"

#include <QMap>
#include <QHBoxLayout>
#include <QDebug>


const int BTN_SPACING = 13;

CPictureWidget::CPictureWidget(QWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CPictureWidget::~CPictureWidget()
{
}

void CPictureWidget::initUI()
{
    QMap<CClickButton::EClickBtnSatus, QString> pictureMapClick;

    pictureMapClick[CClickButton::Normal] = QString(":/theme/light/images/attribute/contrarotate_normal.svg");
    pictureMapClick[CClickButton::Hover] = QString(":/theme/light/images/attribute/contrarotate_hover.svg");
    pictureMapClick[CClickButton::Press] = QString(":/theme/light/images/attribute/contrarotate_press.svg");
    pictureMapClick[CClickButton::Disable] = QString(":/theme/light/images/attribute/contrarotate_disabled.svg");
    m_leftRotateBtn = new CClickButton(pictureMapClick, this);

    pictureMapClick[CClickButton::Normal] = QString(":/theme/light/images/attribute/clockwise rotation_normal.png");
    pictureMapClick[CClickButton::Hover] = QString(":/theme/light/images/attribute/clockwise rotation_hover.png");
    pictureMapClick[CClickButton::Press] = QString(":/theme/light/images/attribute/clockwise rotation_press.png");
    pictureMapClick[CClickButton::Disable] = QString(":/theme/light/images/attribute/clockwise rotation_disabled.svg");
    m_rightRotateBtn = new CClickButton(pictureMapClick, this);

    pictureMapClick[CClickButton::Normal] = QString(":/theme/light/images/attribute/flip horizontal_normal.svg");
    pictureMapClick[CClickButton::Hover] = QString(":/theme/light/images/attribute/flip horizontal_hover.png");
    pictureMapClick[CClickButton::Press] = QString(":/theme/light/images/attribute/flip horizontal_press.png");
    pictureMapClick[CClickButton::Disable] = QString(":/theme/light/images/attribute/flip horizontal_disabled.svg");
    m_flipHBtn = new CClickButton(pictureMapClick, this);

    pictureMapClick[CClickButton::Normal] = QString(":/theme/light/images/attribute/flip vertical_normal.svg");
    pictureMapClick[CClickButton::Hover] = QString(":/theme/light/images/attribute/flip vertical_hover.svg");
    pictureMapClick[CClickButton::Press] = QString(":/theme/light/images/attribute/flip vertical_press.png");
    pictureMapClick[CClickButton::Disable] = QString(":/theme/light/images/attribute/flip vertical_disabled.svg");
    m_flipVBtn = new CClickButton(pictureMapClick, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_leftRotateBtn);
    layout->addWidget(m_rightRotateBtn);
    layout->addWidget(m_flipHBtn);
    layout->addWidget(m_flipVBtn);
    layout->addStretch();
    setLayout(layout);
}

void CPictureWidget::initConnection()
{
    connect(m_leftRotateBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(LeftRotate);
    });

    connect(m_rightRotateBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(RightRotate);
    });

    connect(m_flipHBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(FlipHorizontal);
    });

    connect(m_flipVBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(FlipVertical);
    });
}
