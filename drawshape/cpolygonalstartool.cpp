#include "cpolygonalstartool.h"
#include "cdrawscene.h"
#include "cgraphicspolygonalstaritem.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtMath>

CPolygonalStarTool::CPolygonalStarTool ()
    : IDrawTool (polygonalStar)
    , m_pPolygonalStarItem(nullptr)
{

}

CPolygonalStarTool::~CPolygonalStarTool()
{

}

void CPolygonalStarTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    scene->clearSelection();

    m_sPointPress = event->scenePos();
    m_pPolygonalStarItem = new CGraphicsPolygonalStarItem(CDrawParamSigleton::GetInstance()->getAnchorNum(),
                                                          CDrawParamSigleton::GetInstance()->getRadiusNum(),
                                                          m_sPointPress.x(), m_sPointPress.y(), 0, 0);
    m_pPolygonalStarItem->setPen(CDrawParamSigleton::GetInstance()->getPen());
    m_pPolygonalStarItem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
    scene->addItem(m_pPolygonalStarItem);
    m_pPolygonalStarItem->setSelected(true);

    m_bMousePress = true;
}

void CPolygonalStarTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        QRectF resultRect;
        QPointF resultPoint = pointMouse;
        QRectF rectF(m_sPointPress, resultPoint);
        resultRect = rectF.normalized();
        m_pPolygonalStarItem->setRect(resultRect);
    }
}

void CPolygonalStarTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    m_sPointRelease = event->scenePos();
    //如果鼠标没有移动
    if ( event->scenePos() == m_sPointPress ) {
        if ( m_pPolygonalStarItem != nullptr)
            scene->removeItem(m_pPolygonalStarItem);
        delete m_pPolygonalStarItem;
    }
    m_pPolygonalStarItem = nullptr;
    m_bMousePress = false;
}
