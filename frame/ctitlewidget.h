/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef CEMPTYWIDGET_H
#define CEMPTYWIDGET_H

#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class CTitleWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CTitleWidget(DWidget *parent = nullptr);
    ~CTitleWidget();
    /**
     * @brief updateTitleWidget 更新标题栏
     */
    void updateTitleWidget();

private:
    DLabel *m_title;
};

#endif // CEMPTYWIDGET_H
