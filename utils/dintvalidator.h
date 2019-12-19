/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#ifndef DINTVALIDATOR_H
#define DINTVALIDATOR_H

#include <QIntValidator>

class DIntValidator : public QIntValidator
{
    Q_OBJECT
public:
    explicit DIntValidator(QObject *parent = 0) : QIntValidator(parent) {}
    DIntValidator(int bottom, int top, QObject *parent) : QIntValidator(
            bottom, top, parent) {}
    ~DIntValidator() {}
    virtual void setRange(int bottom, int top)
    {
        QIntValidator::setRange(bottom, top);
    }
    virtual State validate(QString &s, int &n) const
    {
        return QIntValidator::validate(s, n);
    }
    virtual void fixup(QString &s) const
    {
        s = QString("%1").arg(bottom());
    }

};
#endif // DINTVALIDATOR_H
