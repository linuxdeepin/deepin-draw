/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "application.h"
#include "cattributemanagerwgt.h"


#include <DLabel>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <DGuiApplicationHelper>
#include <QDesktopWidget>
#include <DFontSizeManager>

DGUI_USE_NAMESPACE

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int PUSHBUTTON_FONT_SIZE = 12;
const int TEXT_SIZE = 12;

CCutWidget::CCutWidget(DWidget *parent)
    : DrawAttribution::CAttriBaseOverallWgt(parent)
{
    setAttribution(DrawAttribution::ECutToolAttri);
    qRegisterMetaType<ECutType>("ECutType");
    initUI();
    initConnection();
}

CCutWidget::~CCutWidget()
{
}

void CCutWidget::setCutSize(const QSize &sz, bool emitSig)
{
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
        QSize newSize = m_defultRadioSize;
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

void CCutWidget::setAutoCalSizeIfRadioChanged(bool b)
{
    m_autoCal = b;
}

int CCutWidget::totalNeedWidth()
{
    int totalW = 720;
    return  totalW;
}

void CCutWidget::paintEvent(QPaintEvent *event)
{
    DrawAttribution::CAttriBaseOverallWgt::paintEvent(event);
//    QPainter painter(this);

//    painter.setBrush(Qt::blue);
//    painter.drawRect(rect());

//    auto p = qobject_cast<DrawAttribution::CAttriBaseOverallWgt *>(parentWidget());
//    if (p != nullptr)
//        qDebug() << "margin = " << this->contentsMargins() << p->centerLayout()->contentsMargins();
}

void CCutWidget::initUI()
{
    QDesktopWidget *desktopWidget = dApp->desktop();
    bool withNotVarble = desktopWidget->screenGeometry().width() < 1152;
    DLabel *sizeLabel = new DLabel(this);
    sizeLabel->setText(tr("Dimensions"));
    QFont ft;
    ft.setPixelSize(withNotVarble ? TEXT_SIZE - 2 : TEXT_SIZE);
    sizeLabel->setFont(ft);
    _allWgts << sizeLabel;

    m_widthEdit = new DLineEdit(this);
    m_widthEdit->setObjectName("CutWidthLineEdit");
    m_widthEdit->setText(QString::number(800));
    m_widthEdit->setClearButtonEnabled(false);
    m_widthEdit->setFixedWidth(withNotVarble ? 47 : 60);
    m_widthEdit->lineEdit()->setValidator(new CIntValidator(10, 9999, this));
    m_widthEdit->setFont(ft);
    _allWgts << m_widthEdit;

    DLabel *multiLabel = new DLabel(this);
    multiLabel->setText(tr("x"));
    _allWgts << multiLabel;

    m_heightEdit = new DLineEdit(this);
    m_heightEdit->setObjectName("CutHeightLineEdit");
    m_heightEdit->setText(QString::number(600));
    m_heightEdit->setClearButtonEnabled(false);
    m_heightEdit->setFixedWidth(withNotVarble ? 47 : 60);
    m_heightEdit->lineEdit()->setValidator(new CIntValidator(10, 9999, this));
    m_heightEdit->setFont(ft);
    _allWgts << m_heightEdit;

    if (Application::isTabletSystemEnvir()) {
        sizeLabel->hide();
        m_widthEdit->hide();
        multiLabel->hide();
        m_heightEdit->hide();
        _allWgts.removeOne(sizeLabel);
        _allWgts.removeOne(m_widthEdit);
        _allWgts.removeOne(multiLabel);
        _allWgts.removeOne(m_heightEdit);
    }

    m_SizeAddAction = new QAction(this);
    m_SizeAddAction->setObjectName("CutSizeKeyUp");
    m_SizeAddAction->setShortcut(QKeySequence(Qt::Key_Up));
    this->addAction(m_SizeAddAction);

    m_SizeReduceAction = new QAction(this);
    m_SizeReduceAction->setObjectName("CutSizeKeyDown");
    m_SizeReduceAction->setShortcut(QKeySequence(Qt::Key_Down));
    this->addAction(m_SizeReduceAction);

    DLabel *scaleLabel = new DLabel(this);
    scaleLabel->setText(tr("Aspect ratio"));
    _allWgts << scaleLabel;

    scaleLabel->setFont(ft);

    QFont pushBtnFont;
    pushBtnFont.setPixelSize(PUSHBUTTON_FONT_SIZE);

    m_scaleBtn1_1 = new DPushButton(this);
    setWgtAccesibleName(m_scaleBtn1_1, "Cut ratio(1:1) pushbutton");
    m_scaleBtn1_1->setText("1:1");
    m_scaleBtn1_1->setFont(pushBtnFont);
    _allWgts << m_scaleBtn1_1;

    m_scaleBtn2_3 = new DPushButton(this);
    setWgtAccesibleName(m_scaleBtn2_3, "Cut ratio(2:3) pushbutton");
    m_scaleBtn2_3->setText("2:3");
    m_scaleBtn2_3->setFont(pushBtnFont);
    _allWgts << m_scaleBtn2_3;

    m_scaleBtn8_5 = new DPushButton(this);
    setWgtAccesibleName(m_scaleBtn8_5, "Cut ratio(8:5) pushbutton");
    m_scaleBtn8_5->setText("8:5");
    m_scaleBtn8_5->setFont(pushBtnFont);
    _allWgts << m_scaleBtn8_5;

    m_scaleBtn16_9 = new DPushButton(this);
    setWgtAccesibleName(m_scaleBtn16_9, "Cut ratio(16:9) pushbutton");
    m_scaleBtn16_9->setText("16:9");
    m_scaleBtn16_9->setFont(pushBtnFont);
    _allWgts << m_scaleBtn16_9;

    m_freeBtn = new DPushButton(this);
    setWgtAccesibleName(m_freeBtn, "Cut ratio(free) pushbutton");
    m_freeBtn->setText(tr("Free"));
    m_freeBtn->setFont(pushBtnFont);
    _allWgts << m_freeBtn;

    m_originalBtn = new DPushButton(this);
    setWgtAccesibleName(m_originalBtn, "Cut ratio(Original) pushbutton");
    m_originalBtn->setText(tr("Original"));
    m_originalBtn->setFont(pushBtnFont);
    _allWgts << m_originalBtn;

    //修复切换维语和藏语后,裁剪模式按钮大小不一致
    int unifyHeight = m_originalBtn->height() + 8;
    m_scaleBtn1_1->setFixedHeight(unifyHeight);
    m_scaleBtn2_3->setFixedHeight(unifyHeight);
    m_scaleBtn8_5->setFixedHeight(unifyHeight);
    m_scaleBtn16_9->setFixedHeight(unifyHeight);
    m_freeBtn->setFixedHeight(unifyHeight);
    m_originalBtn->setFixedHeight(unifyHeight);

    m_scaleBtn1_1->setCheckable(true);
    m_scaleBtn2_3->setCheckable(true);
    m_scaleBtn8_5->setCheckable(true);
    m_scaleBtn16_9->setCheckable(true);
    m_freeBtn->setCheckable(true);
    m_originalBtn->setCheckable(true);

    m_freeBtn->setChecked(true);

    m_sepLine = new SeperatorLine(this);
    _allWgts.append(m_sepLine);

    m_doneBtn = new DPushButton(this);
    setWgtAccesibleName(m_doneBtn, "Cut done pushbutton");
    m_doneBtn->setMaximumSize(QSize(38, 38));
    m_doneBtn->setIcon(QIcon::fromTheme("ddc_cutting_normal"));
    m_doneBtn->setIconSize(QSize(48, 48));

    m_cancelBtn = new DPushButton(this);
    setWgtAccesibleName(m_cancelBtn, "Cut cancel pushbutton");
    m_cancelBtn->setMaximumSize(QSize(38, 38));
    m_cancelBtn->setIcon(QIcon::fromTheme("ddc_cancel_normal"));
    m_cancelBtn->setIconSize(QSize(48, 48));
    _allWgts << m_cancelBtn << m_doneBtn;

    QHBoxLayout *layout = static_cast<QHBoxLayout *>(centerLayout()) ;
    layout->setContentsMargins(0, 0, 0, 0);
    if (!Application::isTabletSystemEnvir()) {
        layout->addWidget(sizeLabel);
        layout->addWidget(m_widthEdit);
        layout->addWidget(multiLabel);
        layout->addWidget(m_heightEdit);
    }
    layout->addWidget(scaleLabel);
    layout->addWidget(m_scaleBtn1_1);
    layout->addWidget(m_scaleBtn2_3);
    layout->addWidget(m_scaleBtn8_5);
    layout->addWidget(m_scaleBtn16_9);
    layout->addWidget(m_freeBtn);
    layout->addWidget(m_originalBtn);

    layout->addWidget(m_sepLine);
    layout->addWidget(m_cancelBtn);
    layout->addWidget(m_doneBtn);

    m_scaleBtnGroup = new QButtonGroup(this);
    m_scaleBtnGroup->setExclusive(true);
    m_scaleBtnGroup->addButton(m_scaleBtn1_1, cut_1_1);
    m_scaleBtnGroup->addButton(m_scaleBtn2_3, cut_2_3);
    m_scaleBtnGroup->addButton(m_scaleBtn8_5, cut_8_5);
    m_scaleBtnGroup->addButton(m_scaleBtn16_9, cut_16_9);
    m_scaleBtnGroup->addButton(m_freeBtn, cut_free);
    m_scaleBtnGroup->addButton(m_originalBtn, cut_original);

    for (int i = 0; i < _allWgts.count(); ++i) {
        auto pw = _allWgts.at(i);
        DFontSizeManager::instance()->bind(pw, DFontSizeManager::T7, QFont::Normal);
    }
}

void CCutWidget::initConnection()
{
    connect(m_scaleBtnGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
    this, [ = ](int tp, bool checked) {
        if (checked) {
            this->setCutType(ECutType(tp), true, m_autoCal);
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
        //刷新画布，避免残影
        //CManageViewSigleton::GetInstance()->getCurView()->viewport()->update();

        this->setFocus();
        this->setCutType(ECutType::cut_free);
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
        //刷新画布，避免残影
        //CManageViewSigleton::GetInstance()->getCurView()->viewport()->update();

        this->setFocus();
        this->setCutType(ECutType::cut_free);
        this->setCutSize(QSize(newWidth, newHeight));
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
