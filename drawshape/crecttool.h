/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#ifndef CRECTTOOL_H
#define CRECTTOOL_H

#include "idrawtool.h"
#include "cgraphicsrectitem.h"
class CRectTool : public IDrawTool
{
public:
    CRectTool();
    virtual ~CRectTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsRectItem *m_pRectItem;
};

#endif // CRECTTOOL_H
