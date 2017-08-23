#include "cutwidget.h"

#include <QMap>
#include <QHBoxLayout>
#include <QDebug>

#include "widgets/pushbutton.h"

CutWidget::CutWidget(QWidget *parent)
    : QWidget(parent)
{
    QList<PushButton*> btnList;
    PushButton* leftRotateBtn = new PushButton(this);
    leftRotateBtn->setObjectName("LeftRotate");
    leftRotateBtn->setToolTip(tr("Rotate 90° CCW"));
    btnList.append(leftRotateBtn);

    PushButton* rightRotateBtn = new PushButton(this);
    rightRotateBtn->setObjectName("RightRotate");
    rightRotateBtn->setToolTip(tr("Rotate 90° CW"));
    btnList.append(rightRotateBtn);

    PushButton* cutBtn = new PushButton(this);
    cutBtn->setObjectName("CutButton");
    cutBtn->setToolTip(tr("Clip"));
    btnList.append(cutBtn);

    PushButton* flipHBtn = new PushButton(this);
    flipHBtn->setObjectName("FlipHorizontalBtn");
    flipHBtn->setToolTip(tr("Flip horizontally"));
    btnList.append(flipHBtn);

    PushButton* flipVBtn = new PushButton(this);
    flipVBtn->setObjectName("FlipVerticalBtn");
    flipVBtn->setToolTip(tr("Flip vertically"));
    btnList.append(flipVBtn);

    connect(leftRotateBtn, &PushButton::clicked, this, [=]{
        for(int j = 0; j < btnList.length(); j++) {
            btnList[j]->setChecked(false);
        }
        leftRotateBtn->setChecked(true);
        emit rotateImage(-90);
    });
    connect(rightRotateBtn, &PushButton::clicked, this, [=]{
        for(int j = 0; j < btnList.length(); j++) {
            btnList[j]->setChecked(false);
        }
        rightRotateBtn->setChecked(true);
        emit rotateImage(90);
    });
    connect(cutBtn, &PushButton::clicked, this, [=]{
        for(int j = 0; j < btnList.length(); j++) {
            btnList[j]->setChecked(false);
        }
        cutBtn->setChecked(true);
        emit cutImage();
    });
    connect(flipHBtn, &PushButton::clicked, this, [=]{
        for(int j = 0; j < btnList.length(); j++) {
            btnList[j]->setChecked(false);
        }
        flipHBtn->setChecked(true);
         emit mirroredImage(true, false);
    });
    connect(flipVBtn, &PushButton::clicked, this, [=]{
        for(int j = 0; j < btnList.length(); j++) {
            btnList[j]->setChecked(false);
        }
        flipVBtn->setChecked(true);
        emit mirroredImage(false, true);
    });


    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(4);
    layout->addStretch();
    layout->addWidget(leftRotateBtn);
    layout->addWidget(rightRotateBtn);
    layout->addWidget(cutBtn);
    layout->addWidget(flipHBtn);
    layout->addWidget(flipVBtn);
    layout->addStretch();
    setLayout(layout);
}

CutWidget::~CutWidget()
{

}
