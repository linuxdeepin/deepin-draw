// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "csidewidthwidget.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "application.h"
//#include "widgets/dmenucombobox.h"

#include <DGuiApplicationHelper>
#include <DComboBox>

#include <QDebug>

DGUI_USE_NAMESPACE

CSideWidthWidget::CSideWidthWidget(DWidget *parent)
    : DWidget(parent)
    , m_comboxHeight(28)
{
    initUI();
    initConnection();
}

void CSideWidthWidget::setText(const QString &text)
{
    _textLabel->setVisible(!text.isEmpty());
    _textLabel->setText(text);
}

void CSideWidthWidget::setSpace(int space)
{
    m_layout->setSpacing(space);
}

void CSideWidthWidget::setWidth(int width)
{
    if (width >= 0) {
        QString current_px = QString::number(width) + "px";
        m_menuComboBox->setCurrentText(current_px);
        m_maskLable->setVisible(false);
    } else {
        setVaild(false);
    }
}

QComboBox *CSideWidthWidget::menuComboBox()
{
    return m_menuComboBox;
}

QSize CSideWidthWidget::sizeHint() const
{
    return DWidget::sizeHint();
}

void CSideWidthWidget::setVaild(bool vaild)
{
    if (!vaild) {
        m_menuComboBox->setCurrentIndex(-1);
    }
    m_maskLable->setVisible(!vaild);
}

void CSideWidthWidget::initUI()
{
    setWgtAccesibleName(this, "CSideWidthWidget");
    m_layout = new QHBoxLayout(this);
    _textLabel = new DLabel(this);
    m_menuComboBox = new QComboBox(this);
    m_menuComboBox->setFocusPolicy(Qt::NoFocus);
    m_maskLable = new DLabel(m_menuComboBox);
    m_maskLable->setText("— —");
    m_maskLable->move(6, 6);
    m_maskLable->setFixedSize(35, 20);
    m_maskLable->setVisible(true);
    m_maskLable->setFont(m_menuComboBox->font());

    m_menuComboBox->/*setMaximumWidth*/setMinimumWidth(/*100*/90);

    initLineWidthToCombox();
    m_layout->addWidget(_textLabel);
    m_layout->addWidget(m_menuComboBox);
    m_layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(m_layout);

    _textLabel->hide();
}

void CSideWidthWidget::initConnection()
{
    connect(m_menuComboBox, &QComboBox::currentTextChanged, [ = ](const QString & text) {
        if (text.contains("px")) {
            // 判断并且获取当前线宽度
            bool flag = false;
            int lineWidth = text.trimmed().toLower().replace("px", "").toInt(&flag);

            if (flag) {
                emit widthChanged(lineWidth);
            }
            this->setVaild(true);
        }
    });

    // 设置默认2px的线宽度
    m_menuComboBox->setCurrentIndex(1);
}

void CSideWidthWidget::initLineWidthToCombox()
{
    m_menuComboBox->addItem("0px");
    m_menuComboBox->addItem("1px");
    m_menuComboBox->addItem("2px");
    m_menuComboBox->addItem("4px");
    m_menuComboBox->addItem("8px");
    m_menuComboBox->addItem("10px");
}


