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
#ifndef CALPHACONTROLWIDGET_H
#define CALPHACONTROLWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>

#include "globaldefine.h"
#include "adaptivedef.h"

#ifdef USE_DTK
#include <DLineEdit>
#include <DSlider>
DWIDGET_USE_NAMESPACE
#endif

class CAlphaControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CAlphaControlWidget(QWidget *parent = nullptr);

    /**
     * @brief setAlpha　通过颜色setAlpha值更新控件
     * @param internalChanged　由内发生的变化
     */
    void setAlpha(int alpha, bool internalChanged = true);

    /**
     * @brief alphaValue　当前的alpha值
     */
    int alpha();

signals:
    /**
     * @brief alphaChanged　alpha值改变信号
     */
    void alphaChanged(int apl, EChangedPhase phase);

private:
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();

private:
    DSlider *m_alphaSlider;
    DLineEdit *m_alphaLabel;
};

#endif // CALPHACONTROLWIDGET_H
