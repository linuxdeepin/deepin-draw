#ifndef CPICTURETOOL_H
#define CPICTURETOOL_H
#include "idrawtool.h"
#include "cpictureitem.h"

class CPictureTool : public IDrawTool
{
public:
    CPictureTool();
    virtual ~CPictureTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    //QList< CPictureItem *> m_pPictureItemList;
    CPictureItem *m_pPictureItem;

};

#endif // CPICTURETOOL_H
