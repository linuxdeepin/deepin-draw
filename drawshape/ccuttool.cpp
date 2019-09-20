#include "ccuttool.h"

#include "cdrawscene.h"
#include "cdrawparamsigleton.h"

#include <QGraphicsSceneMouseEvent>


CCutTool::CCutTool()
    : IDrawTool (cut)
    , m_pCutItem(nullptr)
{

}

CCutTool::~CCutTool()
{

}

void CCutTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();

        m_sPointPress = event->scenePos();

        m_pCutItem = new CGraphicsCutItem(m_sPointPress.x(), m_sPointPress.y(), 0, 0);
//        m_pCutItem->setPen(CDrawParamSigleton::GetInstance()->getPen());
//        m_pCutItem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
        scene->addItem(m_pCutItem);
        m_pCutItem->setSelected(true);

        m_bMousePress = true;
    } else {
        scene->mouseEvent(event);
    }
}

void CCutTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        QRectF resultRect;

        QPointF resultPoint = pointMouse;
        QRectF rectF(m_sPointPress, resultPoint);
        resultRect = rectF.normalized();

        m_pCutItem->setRect(resultRect);
    }
}

void CCutTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_sPointRelease = event->scenePos();
        //如果鼠标没有移动
        if ( m_pCutItem != nullptr) {
            if ( event->scenePos() == m_sPointPress ) {

                scene->removeItem(m_pCutItem);
                delete m_pCutItem;

            } else {
//                emit scene->itemAdded(m_pCutItem);
            }
        }
        m_pCutItem = nullptr;
        m_bMousePress = false;
    }
}
