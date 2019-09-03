#include "cutwidget.h"

#include <QMap>
#include <QHBoxLayout>
#include <QDebug>

#include "utils/imageutils.h"

const int BTN_SPACING = 13;

CutWidget::CutWidget(QWidget *parent)
    : QWidget(parent)
{
    QList<PushButton *> btnList;
    m_leftRotateBtn = new PushButton(this);
    m_leftRotateBtn->setCheckable(false);
    m_leftRotateBtn->setObjectName("LeftRotate");
    m_leftRotateBtn->setToolTip(tr("Rotate 90° CCW"));
    btnList.append(m_leftRotateBtn);

    m_rightRotateBtn = new PushButton(this);
    m_rightRotateBtn->setCheckable(false);
    m_rightRotateBtn->setObjectName("RightRotate");
    m_rightRotateBtn->setToolTip(tr("Rotate 90° CW"));
    btnList.append(m_rightRotateBtn);

    m_cutBtn = new PushButton(this);
    m_cutBtn->setCheckable(true);
    m_cutBtn->setObjectName("CutButton");
    m_cutBtn->setToolTip(tr("Clip"));
    btnList.append(m_cutBtn);

    m_flipHBtn = new PushButton(this);
    m_flipHBtn->setCheckable(false);
    m_flipHBtn->setObjectName("FlipHorizontalBtn");
    m_flipHBtn->setToolTip(tr("Flip horizontally"));
    btnList.append(m_flipHBtn);

    m_flipVBtn = new PushButton(this);
    m_flipVBtn->setCheckable(false);
    m_flipVBtn->setObjectName("FlipVerticalBtn");
    m_flipVBtn->setToolTip(tr("Flip vertically"));
    btnList.append(m_flipVBtn);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_leftRotateBtn);
    layout->addWidget(m_rightRotateBtn);
    layout->addWidget(m_cutBtn);
    layout->addWidget(m_flipHBtn);
    layout->addWidget(m_flipVBtn);
    layout->addStretch();
    setLayout(layout);

    initConnect();
}

void CutWidget::initConnect()
{
    connect(m_flipHBtn, SIGNAL(clicked()), this, SLOT(mirrorHor()));
    connect(m_flipVBtn, SIGNAL(clicked()), this, SLOT(mirrorVer()));
    connect(m_leftRotateBtn, SIGNAL(clicked()), this, SLOT(rotateLeft()));
    connect(m_rightRotateBtn, SIGNAL(clicked()), this, SLOT(rotateRight()));



//    connect(m_flipHBtn, &PushButton::clicked(), [ = ] () {

//    });setRotation90
}

void CutWidget::mirrorHor()
{
    mirroredImage(true, false);

}

void CutWidget::mirrorVer()
{
    mirroredImage(false, true);

}

void CutWidget::rotateLeft()
{
    emit rotateLeftOrRight(true);

}

void CutWidget::rotateRight()
{
    emit rotateLeftOrRight(false);

}

void CutWidget::updateBtns(const QString &path)
{
    using namespace utils::image;
    if (!QFileInfo(path).exists()) {
        m_leftRotateBtn->setDisabled(true);
        m_rightRotateBtn->setDisabled(true);
        m_cutBtn->setDisabled(true);
        m_flipHBtn->setDisabled(true);
        m_flipVBtn->setDisabled(true);
    } else {
        if (imageSupportSave(path)) {
            m_leftRotateBtn->setDisabled(false);
            m_rightRotateBtn->setDisabled(false);
            m_flipHBtn->setDisabled(false);
            m_flipVBtn->setDisabled(false);
        } else {
            m_leftRotateBtn->setDisabled(true);
            m_rightRotateBtn->setDisabled(true);
            m_flipHBtn->setDisabled(true);
            m_flipVBtn->setDisabled(true);
        }
    }

}

void  CutWidget::cutImageBtnReset()
{
    m_cutBtn->setChecked(false);
    m_leftRotateBtn->setDisabled(false);
    m_rightRotateBtn->setDisabled(false);
    m_flipHBtn->setDisabled(false);
    m_flipVBtn->setDisabled(false);
}

bool CutWidget::cuttingStatus()
{
    return m_cutBtn->isChecked();
}

CutWidget::~CutWidget()
{
}
