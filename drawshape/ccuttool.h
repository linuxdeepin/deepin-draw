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
#ifndef CCUTTOOL_H
#define CCUTTOOL_H

#include "idrawtool.h"
#include "cgraphicscutitem.h"
#include "csizehandlerect.h"
#include "cbuttonrect.h"


class CCutTool : public IDrawTool
{
public:
    CCutTool();
    virtual ~CCutTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;
    void createCutItem(CDrawScene *scene);
    void deleteCutItem(CDrawScene *scene);
    void changeCutType(int, CDrawScene *scene);
    void changeCutSize(const QSize &);
    QRectF getCutRect();

    bool getModifyFlag() const;
    void setModifyFlag(bool flag);

private:
    CGraphicsCutItem *m_pCutItem;
    CSizeHandleRect::EDirection m_dragHandle; //选中的方块方向
    CButtonRect::EButtonType m_buttonType;

    bool m_bModify;

};

#endif // CCUTTOOL_H
