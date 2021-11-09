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
#include "cvalidator.h"

CIntValidator::CIntValidator(int bottom, int top, QObject *parent)
    : QIntValidator(bottom, top, parent)
{

}

void CIntValidator::setEmptyStrToBottom(bool b)
{
    m_emptyStrIsBottom = b;
}

//bool CIntValidator::emptyStrIsBottom() const
//{
//    return m_emptyStrIsBottom;
//}

QValidator::State CIntValidator::validate(QString &s, int &n) const
{
    if (s.isEmpty() && !m_emptyStrIsBottom)
        return QValidator::Acceptable;

    return QIntValidator::validate(s, n);
}

void CIntValidator::fixup(QString &str) const
{
    if (str.isEmpty() && !m_emptyStrIsBottom) {
        return;
    }
    str = QString("%1").arg(bottom());
    QIntValidator::fixup(str);
}
