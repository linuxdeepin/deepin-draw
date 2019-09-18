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
#include "widgets/toolbutton.h"
#include "drawshape/cdrawparamsigleton.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QIntValidator>


const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int PUSHBUTTON_FONT_SIZE = 12;
const int TEXT_SIZE = 12;

CCutWidget::CCutWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CCutWidget::~CCutWidget()
{

}

void CCutWidget::updateCutSize()
{
    QSize size = CDrawParamSigleton::GetInstance()->getCutSize();
    m_widthEdit->setText(QString::number(size.width()));
    m_heightEdit->setText(QString::number(size.height()));
}

void CCutWidget::initUI()
{
    DLabel *sizeLabel = new DLabel(this);
    sizeLabel->setText(tr("尺寸"));
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
    sizeLabel->setFont(ft);

    QIntValidator v( 0, 16384, this );
    // 这个行编辑只接受从0到100的整数

    m_widthEdit = new DLineEdit(this);
    m_widthEdit->setText(QString::number(800));
    m_widthEdit->setClearButtonEnabled(false);
    m_widthEdit->setFixedWidth(60);
    m_widthEdit->lineEdit()->setValidator( &v );
    m_widthEdit->setFont(ft);

    DLabel *multiLabel = new DLabel(this);
    multiLabel->setText(tr("x"));

    m_heightEdit = new DLineEdit(this);
    m_heightEdit->setText(QString::number(600));
    m_heightEdit->setClearButtonEnabled(false);
    m_heightEdit->setFixedWidth(60);
    m_heightEdit->lineEdit()->setValidator( &v );
    m_heightEdit->setFont(ft);

    DLabel *scaleLabel = new DLabel(this);
    scaleLabel->setText(tr("比例"));
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
    m_freeBtn->setText(tr("自由"));
    m_freeBtn->setFont(pushBtnFont);


    m_originalBtn = new DPushButton(this);
    m_originalBtn->setText(tr("原始"));
    m_originalBtn->setFont(pushBtnFont);

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

    layout->addStretch();
    setLayout(layout);
}

void CCutWidget::initConnection()
{
    connect(m_scaleBtn1_1, &DPushButton::clicked, this, [ = ]() {
        CDrawParamSigleton::GetInstance()->setCutAttributeType(ButtonClickAttribute);
        CDrawParamSigleton::GetInstance()->setCutType(cut_1_1);
        emit signalCutAttributeChanged();

        this->updateCutSize();
    });

    connect(m_scaleBtn2_3, &DPushButton::clicked, this, [ = ]() {
        CDrawParamSigleton::GetInstance()->setCutAttributeType(ButtonClickAttribute);
        CDrawParamSigleton::GetInstance()->setCutType(cut_2_3);
        emit signalCutAttributeChanged();

        this->updateCutSize();
    });

    connect(m_scaleBtn8_5, &DPushButton::clicked, this, [ = ]() {
        CDrawParamSigleton::GetInstance()->setCutAttributeType(ButtonClickAttribute);
        CDrawParamSigleton::GetInstance()->setCutType(cut_8_5);
        emit signalCutAttributeChanged();

        this->updateCutSize();
    });

    connect(m_scaleBtn16_9, &DPushButton::clicked, this, [ = ]() {
        CDrawParamSigleton::GetInstance()->setCutAttributeType(ButtonClickAttribute);
        CDrawParamSigleton::GetInstance()->setCutType(cut_16_9);
        emit signalCutAttributeChanged();

        this->updateCutSize();
    });

    connect(m_freeBtn, &DPushButton::clicked, this, [ = ]() {
        CDrawParamSigleton::GetInstance()->setCutAttributeType(ButtonClickAttribute);
        CDrawParamSigleton::GetInstance()->setCutType(cut_free);
        emit signalCutAttributeChanged();
    });

    connect(m_originalBtn, &DPushButton::clicked, this, [ = ]() {
        CDrawParamSigleton::GetInstance()->setCutAttributeType(ButtonClickAttribute);
        CDrawParamSigleton::GetInstance()->setCutType(cut_original);
        emit signalCutAttributeChanged();

        this->updateCutSize();
    });

    connect(m_widthEdit, &DLineEdit::editingFinished, this, [ = ]() {
        int w = m_widthEdit->text().trimmed().toInt();
        int h = m_heightEdit->text().trimmed().toInt();
        CDrawParamSigleton::GetInstance()->setCutAttributeType(LineEditeAttribute);
        CDrawParamSigleton::GetInstance()->setCutSize(QSize(w, h));
        emit signalCutAttributeChanged();
    });

    connect(m_heightEdit, &DLineEdit::editingFinished, this, [ = ]() {
        int w = m_widthEdit->text().trimmed().toInt();
        int h = m_heightEdit->text().trimmed().toInt();
        CDrawParamSigleton::GetInstance()->setCutAttributeType(LineEditeAttribute);
        CDrawParamSigleton::GetInstance()->setCutSize(QSize(w, h));
        emit signalCutAttributeChanged();
    });
}
