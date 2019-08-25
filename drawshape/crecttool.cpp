#include "crecttool.h"
#include "cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
CRectTool::CRectTool ()
    : IDrawTool (rectangle)
    , m_pRectItem(nullptr)
{

}

CRectTool::~CRectTool()
{

}

void CRectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    scene->clearSelection();

    m_sPointPress = event->scenePos();
    m_pRectItem = new CGraphicsRectItem(m_sPointPress.x(), m_sPointPress.y(), 0, 0);
    m_pRectItem->setPen(CDrawParamSigleton::GetInstance()->getPen());
    scene->addItem(m_pRectItem);

    m_bMousePress = true;
}

void CRectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        QRectF rectF(m_sPointPress, pointMouse);
        QRectF normalRectF = rectF.normalized();
        m_pRectItem->setRect(normalRectF);
    }
}

void CRectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    m_sPointRelease = event->scenePos();
    m_pRectItem = nullptr;
    m_bMousePress = false;
}
