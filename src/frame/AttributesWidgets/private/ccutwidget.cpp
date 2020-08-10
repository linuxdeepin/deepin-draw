/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ccutwidget.h"
#include "drawshape/cdrawscene.h"
#include "widgets/toolbutton.h"
#include "utils/cvalidator.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "service/cmanagerattributeservice.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int PUSHBUTTON_FONT_SIZE = 12;
const int TEXT_SIZE = 12;

CCutWidget::CCutWidget(DWidget *parent)
    : DWidget(parent)
{
    qRegisterMetaType<ECutType>("ECutType");
    initUI();
    initConnection();
}

CCutWidget::~CCutWidget()
{
}

void CCutWidget::setCutSize(const QSize &sz, bool emitSig)
{
    //QSize size = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize();

    if (sz != m_cutCutSize) {
        m_cutCutSize = sz;
        m_widthEdit->setText(QString::number(sz.width()));
        m_heightEdit->setText(QString::number(sz.height()));
        if (emitSig) {
            emit cutSizeChanged(sz);
        }
    }
}

QSize CCutWidget::cutSize()
{
    return m_cutCutSize;
}

void CCutWidget::clearAllChecked()
{
    m_scaleBtn1_1->setChecked(false);
    m_scaleBtn2_3->setChecked(false);
    m_scaleBtn8_5->setChecked(false);
    m_scaleBtn16_9->setChecked(false);
    m_freeBtn->setChecked(false);
    m_originalBtn->setChecked(false);
}

void CCutWidget::adjustSize(bool emitSig)
{
    if (m_curCutType < cut_free) {
        qreal rd = Radio[m_curCutType];
        QSize newSize = /*m_cutCutSize*//*QSize(1920, 1080)*/m_defultRadioSize;
        int newWidth = qRound(newSize.height() * rd);
        newSize.setWidth(newWidth);
        setCutSize(newSize, emitSig);
    }
}

void CCutWidget::changeButtonTheme()
{
    m_sepLine->updateTheme();
}

void CCutWidget::setCutType(ECutType current, bool emitSig, bool adjustSz)
{
    if (current != m_curCutType) {
        m_curCutType = current;
        if (emitSig) {
            emit cutTypeChanged(current);
        }
        if (adjustSz)
            adjustSize(emitSig);
    }

    clearAllChecked();
    // 设置按钮选中状态
    switch (m_curCutType) {

    case cut_1_1: {
        m_scaleBtn1_1->setChecked(true);
        break;
    }
    case cut_2_3: {
        m_scaleBtn2_3->setChecked(true);
        break;
    }
    case cut_8_5: {
        m_scaleBtn8_5->setChecked(true);
        break;
    }
    case cut_16_9: {
        m_scaleBtn16_9->setChecked(true);
        break;
    }
    case cut_free: {
        m_freeBtn->setChecked(true);
        break;
    }
    case cut_original: {
        m_originalBtn->setChecked(true);
        break;
    }
    default: break;
    }
}

ECutType CCutWidget::cutType()
{
    return m_curCutType;
}

void CCutWidget::setDefualtRaidoBaseSize(const QSize &sz)
{
    m_defultRadioSize = sz;
}

QSize CCutWidget::defualtRaidoBaseSize()
{
    return m_defultRadioSize;
}

void CCutWidget::initUI()
{
    DLabel *sizeLabel = new DLabel(this);
    sizeLabel->setText(tr("Dimensions"));
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
    sizeLabel->setFont(ft);

    m_widthEdit = new DLineEdit(this);
    m_widthEdit->setText(QString::number(800));
    m_widthEdit->setClearButtonEnabled(false);
    m_widthEdit->setFixedWidth(60);
    m_widthEdit->lineEdit()->setValidator(new CIntValidator(10, 9999, this));
    m_widthEdit->setFont(ft);

    DLabel *multiLabel = new DLabel(this);
    multiLabel->setText(tr("x"));

    m_heightEdit = new DLineEdit(this);
    m_heightEdit->setText(QString::number(600));
    m_heightEdit->setClearButtonEnabled(false);
    m_heightEdit->setFixedWidth(60);
    m_heightEdit->lineEdit()->setValidator(new CIntValidator(10, 9999, this));
    m_heightEdit->setFont(ft);

    m_SizeAddAction = new QAction(this);
    m_SizeAddAction->setShortcut(QKeySequence(Qt::Key_Up));
    this->addAction(m_SizeAddAction);

    m_SizeReduceAction = new QAction(this);
    m_SizeReduceAction->setShortcut(QKeySequence(Qt::Key_Down));
    this->addAction(m_SizeReduceAction);

    DLabel *scaleLabel = new DLabel(this);
    scaleLabel->setText(tr("Aspect ratio"));

    scaleLabel->setFont(ft);

    QFont pushBtnFont;
    pushBtnFont.setPixelSize(PUSHBUTTON_FONT_SIZE);

    m_scaleBtn1_1 = new DPushButton(this);
    m_scaleBtn1_1->setText("1:1");
    m_scaleBtn1_1->setFont(pushBtnFont);

    m_scaleBtn2_3 = new DPushButton(this);
    m_scaleBtn2_3->setText("2:3");
    m_scaleBtn2_3->setFont(pushBtnFont);

    m_scaleBtn8_5 = new DPushButton(this);
    m_scaleBtn8_5->setText("8:5");
    m_scaleBtn8_5->setFont(pushBtnFont);

    m_scaleBtn16_9 = new DPushButton(this);
    m_scaleBtn16_9->setText("16:9");
    m_scaleBtn16_9->setFont(pushBtnFont);

    m_freeBtn = new DPushButton(this);
    m_freeBtn->setText(tr("Free"));
    m_freeBtn->setFont(pushBtnFont);

    m_originalBtn = new DPushButton(this);
    m_originalBtn->setText(tr("Original"));
    m_originalBtn->setFont(pushBtnFont);

    m_scaleBtn1_1->setCheckable(true);
    m_scaleBtn2_3->setCheckable(true);
    m_scaleBtn8_5->setCheckable(true);
    m_scaleBtn16_9->setCheckable(true);
    m_freeBtn->setCheckable(true);
    m_originalBtn->setCheckable(true);

    m_freeBtn->setChecked(true);

    m_sepLine = new SeperatorLine(this);

    m_doneBtn = new DPushButton(this);
    m_doneBtn->setMaximumSize(QSize(38, 38));
    m_doneBtn->setIcon(QIcon::fromTheme("ddc_cutting_normal"));
    m_doneBtn->setIconSize(QSize(48, 48));

    m_cancelBtn = new DPushButton(this);
    m_cancelBtn->setMaximumSize(QSize(38, 38));
    m_cancelBtn->setIcon(QIcon::fromTheme("ddc_cancel_normal"));
    m_cancelBtn->setIconSize(QSize(48, 48));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(sizeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_widthEdit);
    layout->addWidget(multiLabel);
    layout->addWidget(m_heightEdit);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(scaleLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn1_1);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn2_3);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn8_5);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_scaleBtn16_9);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_freeBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_originalBtn);

    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_sepLine);
    layout->addWidget(m_cancelBtn);
    layout->addWidget(m_doneBtn);

    layout->addStretch();
    setLayout(layout);

    m_scaleBtnGroup = new QButtonGroup(this);
    m_scaleBtnGroup->setExclusive(true);
    m_scaleBtnGroup->addButton(m_scaleBtn1_1, cut_1_1);
    m_scaleBtnGroup->addButton(m_scaleBtn2_3, cut_2_3);
    m_scaleBtnGroup->addButton(m_scaleBtn8_5, cut_8_5);
    m_scaleBtnGroup->addButton(m_scaleBtn16_9, cut_16_9);
    m_scaleBtnGroup->addButton(m_freeBtn, cut_free);
    m_scaleBtnGroup->addButton(m_originalBtn, cut_original);
}

void CCutWidget::initConnection()
{
    connect(m_scaleBtnGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
    this, [ = ](int tp, bool checked) {
        if (checked) {
            this->setCutType(ECutType(tp), true);
        }
    });

    connect(m_widthEdit, &DLineEdit::editingFinished, this, [ = ]() {
        int newWidth = m_widthEdit->text().trimmed().toInt();
        int newHeight = m_heightEdit->text().trimmed().toInt();

        if (newWidth > 4096) {
            newWidth = 4096;
            m_widthEdit->blockSignals(true);
            m_widthEdit->setText(QString::number(newWidth));
            m_widthEdit->blockSignals(false);
        }

        qreal newRadio = qreal(newWidth) / qreal(newHeight);
        int count = sizeof(Radio) / sizeof(Radio[0]);
        ECutType tp = cut_free;
        for (int i = 0; i < count; ++i) {
            if (qFuzzyIsNull(Radio[i] - newRadio)) {
                tp = ECutType(i);
                break;
            }
        }
        this->setCutType(tp, true, false);
        this->setCutSize(QSize(newWidth, newHeight));
    });

    connect(m_heightEdit, &DLineEdit::editingFinished, this, [ = ]() {
        int newWidth = m_widthEdit->text().trimmed().toInt();
        int newHeight = m_heightEdit->text().trimmed().toInt();

        if (newHeight > 4096) {
            newHeight = 4096;
            m_heightEdit->blockSignals(true);
            m_heightEdit->setText(QString::number(newHeight));
            m_heightEdit->blockSignals(false);
        }

        qreal newRadio = qreal(newWidth) / qreal(newHeight);
        int count = sizeof(Radio) / sizeof(Radio[0]);
        ECutType tp = cut_free;
        for (int i = 0; i < count; ++i) {
            if (qFuzzyIsNull(Radio[i] - newRadio)) {
                tp = ECutType(i);
                break;
            }
        }
        this->setCutType(tp, true, false);
        this->setCutSize(QSize(newWidth, newHeight));

        //        if (m_heightEdit->lineEdit()->hasFocus()) {
        //            m_heightEdit->lineEdit()->clearFocus();
        //            return;
        //        }

        //        activeFreeMode();

        //        int w = m_widthEdit->text().trimmed().toInt();
        //        int h = m_heightEdit->text().trimmed().toInt();
        //        if (h > 4096) {
        //            h = 4096;
        //            m_heightEdit->setText(QString::number(h));

        //        }
        //        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutAttributeType(LineEditeAttribute);
        //        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutSize(QSize(w, h));
        //        CManagerAttributeService::getInstance()->doCut();
    });

    connect(m_doneBtn, &DPushButton::clicked, this, [ = ]() {
        if (m_widthEdit->lineEdit()->hasFocus()) {
            m_widthEdit->lineEdit()->clearFocus();
        }

        if (m_heightEdit->lineEdit()->hasFocus()) {
            m_heightEdit->lineEdit()->clearFocus();
        }
        emit finshed(true);
    });

    connect(m_cancelBtn, &DPushButton::clicked, this, [ = ]() {
        emit finshed(false);
    });

    connect(m_SizeAddAction, &QAction::triggered, this, [ = ](bool) {
        if (m_widthEdit->lineEdit()->hasFocus()) {
            int widthSizeNum = m_widthEdit->lineEdit()->text().trimmed().toInt();
            widthSizeNum++;
            if (widthSizeNum > 4096) {
                return;
            }
            QString text = QString::number(widthSizeNum);
            m_widthEdit->setText(text);
            emit m_widthEdit->lineEdit()->textEdited(text);
        } else if (m_heightEdit->lineEdit()->hasFocus()) {
            int heightSizeNum = m_heightEdit->lineEdit()->text().trimmed().toInt();
            heightSizeNum++;
            if (heightSizeNum > 4096) {
                return;
            }
            QString text = QString::number(heightSizeNum);
            m_heightEdit->setText(text);
            emit m_heightEdit->lineEdit()->textEdited(text);
        }
    });

    connect(m_SizeReduceAction, &QAction::triggered, this, [ = ](bool) {
        if (m_widthEdit->lineEdit()->hasFocus()) {
            int widthSizeNum = m_widthEdit->lineEdit()->text().trimmed().toInt();
            widthSizeNum--;
            if (widthSizeNum < 10) {
                return;
            }
            QString text = QString::number(widthSizeNum);
            m_widthEdit->setText(text);
            emit m_widthEdit->lineEdit()->textEdited(text);
        } else if (m_heightEdit->lineEdit()->hasFocus()) {
            int heightSizeNum = m_heightEdit->lineEdit()->text().trimmed().toInt();
            heightSizeNum--;
            if (heightSizeNum < 10) {
                return;
            }
            QString text = QString::number(heightSizeNum);
            m_heightEdit->setText(text);
            emit m_heightEdit->lineEdit()->textEdited(text);
        }
    });
}

void CCutWidget::activeFreeMode()
{
    if (!m_freeBtn->isChecked()) {
        emit m_freeBtn->clicked(true);
    }
}
