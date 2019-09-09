#include "clinetool.h"
#include "cdrawscene.h"
#include "cgraphicslineitem.h"
#include "cdrawparamsigleton.h"
#include <QGraphicsSceneMouseEvent>
CLineTool::CLineTool()
    : IDrawTool(line)
    , m_pLineItem(nullptr)
{

}

CLineTool::~CLineTool()
{

}

void CLineTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();
        m_sPointPress = event->scenePos();
        m_pLineItem = new CGraphicsLineItem(m_sPointPress, m_sPointPress);
        m_pLineItem->setPen(CDrawParamSigleton::GetInstance()->getPen());
        m_pLineItem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
        scene->addItem(m_pLineItem);
        m_pLineItem->setSelected(true);

        m_bMousePress = true;
    } else {
        scene->mouseEvent(event);
    }
}

void CLineTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        m_pLineItem->setLine(m_sPointPress, pointMouse);
    }
}

void CLineTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene);
    m_sPointRelease = event->scenePos();
    //如果鼠标没有移动
    if ( event->scenePos() == m_sPointPress ) {
        if ( m_pLineItem != nullptr)
            scene->removeItem(m_pLineItem);
        delete m_pLineItem;
    }

    m_pLineItem = nullptr;
    m_bMousePress = false;
}
