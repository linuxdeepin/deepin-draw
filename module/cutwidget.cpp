#include "cutwidget.h"

#include <QMap>
#include <QHBoxLayout>
#include <QDebug>

#include "widgets/pushbutton.h"

CutWidget::CutWidget(QWidget *parent)
    : QWidget(parent)
{
//    QMap<int, QString> btnInfoMap;
//    btnInfoMap.insert(0, "LeftRotate");
//    btnInfoMap.insert(1, "RightRotate");
//    btnInfoMap.insert(2, "CutButton");
//    btnInfoMap.insert(3, "FlipHorizontalBtn");
//    btnInfoMap.insert(4, "FlipVerticalBtn");

//    QStringList btnTextList;
//    btnTextList << tr("Rotate 90° CCW") << tr("Rotate 90° CW") << tr("Clip")
//                          << tr("Flip horizontally") << tr("FlipVertically");

//    QList<PushButton*> btnList;
//    QMap<int, QString>::const_iterator i = btnInfoMap.constBegin();

    PushButton* leftRotateBtn = new PushButton(this);
    leftRotateBtn->setObjectName("LeftRotate");
    PushButton* rightRotateBtn = new PushButton(this);
    rightRotateBtn->setObjectName("RightRotate");
    PushButton* cutBtn = new PushButton(this);
    cutBtn->setObjectName("CutButton");
    PushButton* flipHBtn = new PushButton(this);
    flipHBtn->setObjectName("FlipHorizontalBtn");
    PushButton* flipVBtn = new PushButton(this);
    flipVBtn->setObjectName("FlipVerticalBtn");

//    while (i != btnInfoMap.constEnd()) {
//        PushButton* btn = new PushButton();
//        btnList.append(btn);
//        btn->setObjectName(i.value());
//        btn->setText(btnTextList[i.key()]);

//        if (i.key() == 0) {
//            connect(btn, &PushButton::clicked, this, [=]{
//                emit rotateImage(-90);
//                qDebug() << "topToolbar rotateImage:" /*<< m_path*/;
//            });
//        }
//        if (i.key() == 1) {
//            connect(btn, &PushButton::clicked, this, [=]{
//                emit rotateImage(90);
//            });
//        }
//        if (i.key() == 2) {
//            connect(btn, &PushButton::clicked, this, [=]{
//                emit cutImage();
//                emit drawShapes("cutImage");
//            });
//        }
//        if (i.key() == 3) {
//            connect(btn, &PushButton::clicked, this, [=]{
//                emit mirroredImage(true, false);
//            });
//        }
//        if (i.key() == 4) {
//            connect(btn, &PushButton::clicked, this, [=]{
//                emit mirroredImage(false, true);
//            });
//        }
//        ++i;
//    }

//    for(int k = 0; k < btnList.length(); k++) {
//        connect(btnList[k], &PushButton::clicked, this, [=]{
//            if (btnList[k]->getChecked()) {
//                for (int j = 0; j < k; j++) {
//                    btnList[j]->setChecked(false);
//                }

//                for(int p = k + 1; p < btnList.length(); p++) {
//                    btnList[p]->setChecked(false);
//                }
//            } else {
//                qDebug() << "Btn exclusive failed" << k;
//            }
//        });
//    }

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(leftRotateBtn);
    layout->addWidget(rightRotateBtn);
    layout->addWidget(cutBtn);
    layout->addWidget(flipHBtn);
    layout->addWidget(flipVBtn);
    setLayout(layout);
}

CutWidget::~CutWidget()
{

}
