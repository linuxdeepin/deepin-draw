/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     RenBin <renbin@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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

#ifndef CDOUBLESPINBOX_H
#define CDOUBLESPINBOX_H

#include <QWidget>
#include "globaldefine.h"

#ifdef USE_DTK
#include <DDoubleSpinBox>
#define CDOUBLESPINBOXFATHERCLASS DDoubleSpinBox
DWIDGET_USE_NAMESPACE
#else
#include <QDoubleSpinBox>
#define CDOUBLESPINBOXFATHERCLASS QDoubleSpinBox
#endif

/**
 * @brief 用于过滤部分信号，当输入值完成输入及按步变更时
 *      发送数值变更信号。
 */
class CDoubleSpinBox : public CDOUBLESPINBOXFATHERCLASS
{
    Q_OBJECT

public:
    explicit CDoubleSpinBox(QWidget *parent = nullptr);

    // 数值按步变更时触发，判断是否向外抛出信号
    virtual void stepBy(int steps) override;
    // 数值变更时触发信号
    Q_SIGNAL void phaseValueChanged(double value, EChangedPhase phase);

private:
    PRIVATESAFECLASS(CDoubleSpinBox)
};

#endif // CDOUBLESPINBOX_H
