/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
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
#ifndef ADJUSTMENTATRRIWIDGET_H
#define ADJUSTMENTATRRIWIDGET_H

#include <QWidget>
#include "attributewidget.h"

class DPushButton;
class AdjustmentAtrriWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit AdjustmentAtrriWidget(QWidget *parent = nullptr);
    DIconButton *button();
    virtual void setVar(const QVariant &var) override;
private:
    void initUi();
signals:

private:
    DIconButton *m_adjustmentBtn = nullptr;
    QLabel *m_titleLabel = nullptr;
};

#endif // ADJUSTMENTATRRIWIDGET_H
