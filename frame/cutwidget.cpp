#include "cutwidget.h"

#include <QMap>
#include <QHBoxLayout>
#include <QDebug>

#include "utils/imageutils.h"

CutWidget::CutWidget(QWidget *parent)
    : QWidget(parent)
{
    QList<PushButton*> btnList;
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
    m_cutBtn->lockedCheckedStatus(true);
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

    connect(m_leftRotateBtn, &PushButton::clicked, this, [=]{
        emit rotateImage(-90);
    });
    connect(m_rightRotateBtn, &PushButton::clicked, this, [=]{
        emit rotateImage(90);
    });

    connect(m_cutBtn, &PushButton::clicked, this, [=]{
        emit cutImage();
        m_leftRotateBtn->setDisabled(true);
        m_rightRotateBtn->setDisabled(true);
        m_flipHBtn->setDisabled(true);
        m_flipVBtn->setDisabled(true);
    });
    connect(m_flipHBtn, &PushButton::clicked, this, [=]{
         emit mirroredImage(true, false);
    });
    connect(m_flipVBtn, &PushButton::clicked, this, [=]{
        emit mirroredImage(false, true);
    });

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(4);
    layout->addStretch();
    layout->addWidget(m_leftRotateBtn);
    layout->addWidget(m_rightRotateBtn);
    layout->addWidget(m_cutBtn);
    layout->addWidget(m_flipHBtn);
    layout->addWidget(m_flipVBtn);
    layout->addStretch();
    setLayout(layout);
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

CutWidget::~CutWidget()
{
}
