/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "csidewidthwidget.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
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

void CSideWidthWidget::updateSideWidth()
{
    int lineWidth = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineWidth();
    QString current_px = QString::number(lineWidth) + "px";
    m_menuComboBox->setCurrentText(current_px);
}

void CSideWidthWidget::setSideWidth(int width)
{
    QString current_px = QString::number(width) + "px";
    m_menuComboBox->setCurrentText(current_px);
}

void CSideWidthWidget::setMenuButtonICon(QString text, QIcon icon)
{
    Q_UNUSED(text)
    Q_UNUSED(icon)
}

void CSideWidthWidget::initUI()
{
    m_layout = new QHBoxLayout(this);
    m_menuComboBox = new DComboBox(this);

    m_menuComboBox->setMaximumWidth(100);

    initLineWidthToCombox();
    m_layout->addWidget(m_menuComboBox);

    this->setLayout(m_layout);
}

void CSideWidthWidget::initConnection()
{
    connect(m_menuComboBox, &DComboBox::currentTextChanged, [ = ](QString text) {
        if (text.contains("px")) {
            // 判断并且获取当前线宽度
            bool flag = false;
            int lineWidth = text.trimmed().toLower().replace("px", "").toInt(&flag);

            if (flag) {
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineWidth(lineWidth);
                emit signalSideWidthChange();
            }
        }
    });

    connect(m_menuComboBox, QOverload<const QString &>::of(&DComboBox::highlighted), [ = ](const QString & text) {
        if (text.contains("px")) {
            // 判断并且获取当前线宽度
            bool flag = false;
            int lineWidth = text.trimmed().toLower().replace("px", "").toInt(&flag);

            if (flag) {
                emit signalSideWidthChoosed(lineWidth);
            }
        }
    });

//    connect(m_menuComboBox, &DComboBox::signalAboutToShow, [ = ]() {
//        emit signalSideWidthMenuShow();
//    });

    // 设置默认2px的线宽度
    m_menuComboBox->setCurrentIndex(2);
}

void CSideWidthWidget::initLineWidthToCombox()
{
    m_menuComboBox->addItem("0px");
    m_menuComboBox->addItem("1px");
    m_menuComboBox->addItem("2px");
    m_menuComboBox->addItem("4px");
    m_menuComboBox->addItem("8px");
    m_menuComboBox->addItem("10px");
    changeButtonTheme();
}

QPixmap CSideWidthWidget::drawLinePixmap(int lineWidth, QColor lineColor, int width, int height)
{
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);//用透明色填充
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(lineColor, Qt::SolidPattern)); //设置画刷形式
    painter.drawRect(0, (height - lineWidth) / 2, width, lineWidth);
    painter.end();
    return pixmap;
}

void CSideWidthWidget::changeButtonTheme()
{
//    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
//    QColor lineColor;

//    if (1 == themeType) {
//        lineColor.setRgb(0, 0, 0);
//    } else {
//        lineColor.setRgb(255, 255, 255);
//    }

//    m_menuComboBox->blockSignals(true);
//    m_menuComboBox->setItemIcon(0, QIcon(drawLinePixmap(0, lineColor, m_comboxHeight, m_comboxHeight)));
//    m_menuComboBox->setItemIcon(1, QIcon(drawLinePixmap(1, lineColor, m_comboxHeight, m_comboxHeight)));
//    m_menuComboBox->setItemIcon(2, QIcon(drawLinePixmap(2, lineColor, m_comboxHeight, m_comboxHeight)));
//    m_menuComboBox->setItemIcon(3, QIcon(drawLinePixmap(4, lineColor, m_comboxHeight, m_comboxHeight)));
//    m_menuComboBox->setItemIcon(4, QIcon(drawLinePixmap(8, lineColor, m_comboxHeight, m_comboxHeight)));
//    m_menuComboBox->setItemIcon(5, QIcon(drawLinePixmap(10, lineColor, m_comboxHeight, m_comboxHeight)));
//    m_menuComboBox->blockSignals(false);
}


