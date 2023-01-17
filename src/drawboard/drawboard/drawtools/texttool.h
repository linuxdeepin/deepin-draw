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
#ifndef CTEXTTOOL_H
#define CTEXTTOOL_H
#include "drawitemtool.h"

class DRAWLIB_EXPORT TextTool : public DrawItemTool
{
    Q_OBJECT
public:
    TextTool(QObject *parent = nullptr);

    ~TextTool() override;

    int toolType() const override;

    SAttrisList attributions() override;
    void setAttributionVar(int attri, const QVariant &var, int phase, bool autoCmdStack) override;

protected:
    PageItem *drawItemStart(ToolSceneEvent *event) override;
    void      drawItemFinish(ToolSceneEvent *event, PageItem *pItem) override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;
    bool isPressEventHandledByQt(ToolSceneEvent *event) override;

    int minMoveUpdateDistance() override;


    PRIVATECLASS(TextTool)
};

#endif // CTEXTTOOL_H
