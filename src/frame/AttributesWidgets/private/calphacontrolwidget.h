// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CALPHACONTROLWIDGET_H
#define CALPHACONTROLWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLabel>
#include <DLineEdit>

#include "globaldefine.h"

DWIDGET_USE_NAMESPACE

class CAlphaControlWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CAlphaControlWidget(DWidget *parent = nullptr);

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
