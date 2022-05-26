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

#include "cdoublespinbox.h"

#include <QLineEdit>
#include <QTimer>

/**
 * @brief 内部私有类
 */
class CDoubleSpinBox::CDoubleSpinBox_private
{
public:
    explicit CDoubleSpinBox_private(CDoubleSpinBox *q)
        : q_ptr(q)
    {
        m_finishedTimer.setSingleShot(true);
    }

    CDoubleSpinBox *q_ptr;
    QTimer          m_finishedTimer;    // 超时更新定时
};

/**
 * @brief CDoubleSpinBox::CDoubleSpinBox
 * @param parent
 */
CDoubleSpinBox::CDoubleSpinBox(QWidget *parent)
    : CDOUBLESPINBOXFATHERCLASS(parent)
    , CDoubleSpinBox_d(new CDoubleSpinBox_private(this))
{
    // 当前控件输入框完成编辑时触发
    connect(this, &QDoubleSpinBox::editingFinished, this, [ this ]() {
        Q_EMIT phaseValueChanged(value(), EChangedFinished);
    });

    // 超过50ms的按步数值变更，才会更新变更完成信号
    connect(&d_CDoubleSpinBox()->m_finishedTimer, &QTimer::timeout, this, [ this ]() {
        Q_EMIT phaseValueChanged(value(), EChangedFinished);
    });
}

/**
 * @brief 输入框按步 \a steps 变更时，判断数据是否变化，
 *      若变更则发送数据更新信号。
 * @param steps 输入框数据变化的步数
 */
void CDoubleSpinBox::stepBy(int steps)
{
    double old = value();
    DDoubleSpinBox::stepBy(steps);
    if (!qFuzzyCompare(old, value())) {
        Q_EMIT phaseValueChanged(value(), EChangedUpdate);

        // 启动超时判断定时，超时50ms后的按步数值变更才会被认为完成
        d_CDoubleSpinBox()->m_finishedTimer.start(50);
    }
}
