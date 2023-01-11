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
#ifndef CPICTURETOOL_H
#define CPICTURETOOL_H

#include "drawtool.h"

class DRAWLIB_EXPORT ImageLoadTool: public DrawTool
{
    Q_OBJECT
public:
    explicit ImageLoadTool(QObject *parent = nullptr);
    ~ImageLoadTool();

    int toolType() const override;

    void pressOnScene(ToolSceneEvent *event) override;
    void moveOnScene(ToolSceneEvent *event) override;
    void releaseOnScene(ToolSceneEvent *event) override;

    SAttrisList attributions() override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    PRIVATECLASS(ImageLoadTool)
};


#endif // CPICTURETOOL_H
