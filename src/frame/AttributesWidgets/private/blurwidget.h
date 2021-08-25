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
#ifndef BLURWIDGET_H
#define BLURWIDGET_H

#include "globaldefine.h"
#include "cattributeitemwidget.h"

#include <DWidget>
#include <DSlider>
#include <DLabel>
#include <DToolButton>

class CSpinBox;

DWIDGET_USE_NAMESPACE

struct SBLurEffect {
    int width = 20;
    int type = 1;
};

Q_DECLARE_METATYPE(SBLurEffect)

class BlurWidget : public DrawAttribution::CAttriBaseOverallWgt
{
    Q_OBJECT

public:
    explicit BlurWidget(DWidget *parent = nullptr);
    ~BlurWidget();

    /**
     * @brief setBlurWidth 设置模糊宽度
     */
    void setBlurWidth(const int &width, bool emitSig = true);

    /**
     * @brief setBlurType 设置模糊类型
     */
    void setBlurType(const EBlurEffect &blurEffect, bool emitSig = true);

    /**
     * @brief getEffect 获取属性值
     */
    SBLurEffect getEffect() const;

signals:
//    void blurTypeChanged(EBlurEffect);
//    void blurWidthChanged(int);
    void blurEffectChanged(const SBLurEffect &effect);

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection 初始化连接
     */
    void initConnection();

private:
    QButtonGroup *m_TypeButtons = nullptr;

    DLabel   *m_pLineWidthLabel;
    CSpinBox *m_spinboxForLineWidth = nullptr;
};

#endif // BLURWIDGET_H
