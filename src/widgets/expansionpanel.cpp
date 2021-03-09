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
#include "expansionpanel.h"
#include "frame/cviewmanagement.h"

#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>

ExpansionPanel::ExpansionPanel(QWidget *parent) : DBlurEffectWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;


    //设置圆角
    setBlurRectXRadius(18);
    setBlurRectYRadius(18);
    setBlurEnabled(true);
    setMode(DBlurEffectWidget::GaussianBlur);

    //绘制背景阴影
    setAttribute(Qt::WA_TranslucentBackground);
    const int nMargin = 10;     // 设置阴影宽度
    shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setBlurRadius(nMargin);
    this->setGraphicsEffect(shadow_effect); //最外层的Frame

    //组合按钮
    m_groupButton = new ToolButton(this);
    m_groupButton->setFixedSize(190, 34);
    m_groupButton->setText(tr("Group"));
    m_groupButton->setIcon(QIcon::fromTheme("icon_group_normal"));

    //释放组合按钮
    m_unGroupButton = new ToolButton(this);
    m_unGroupButton->setFixedSize(190, 34);
    m_unGroupButton->setText(tr("Ungroup"));
    m_unGroupButton->setIcon(QIcon::fromTheme("icon_ungroup_normal"));

    layout->addWidget(m_groupButton);
    layout->addWidget(m_unGroupButton);

    //设置边距
    layout->setContentsMargins(0, 12, 0, 12);

    setLayout(layout);

    connect(m_groupButton, &DToolButton::clicked, this, [ = ] {
        // 组合按钮的点击信号
        Q_EMIT signalItemGroup();
    });

    connect(m_unGroupButton, &DToolButton::clicked, this, [ = ] {

        // 释放组合按钮的点击信号
        Q_EMIT signalItemgUngroup();
    });
}

ToolButton *ExpansionPanel::getGroupButton()
{
    // 组合按钮
    return m_groupButton;
}

ToolButton *ExpansionPanel::getUngroupButton()
{
    // 释放组合按钮
    return  m_unGroupButton;
}

void ExpansionPanel::changeTheme()
{
    if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {

        //背景颜色及透明度
        auto effect = QColor("#EBEBEB");
        effect.setAlpha(80);
        setMaskColor(effect);
        shadow_effect->setColor(QColor(150, 150, 150));
    } else {
        //背景颜色及透明度
        auto effect = QColor("#404040");
        setMaskColor(effect);
        shadow_effect->setColor(QColor("#404040"));
    }
}


