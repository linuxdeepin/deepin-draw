#include "crecttool.h"
#include "cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtMath>
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
    m_pRectItem->setSelected(true);

    m_bMousePress = true;
}

void CRectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        QPointF resultPoint = pointMouse;
        //按下SHIFT键
        if (m_bShiftKeyPress && !m_bAltKeyPress) {

            qreal w = resultPoint.x() - m_sPointPress.x();
            qreal h = resultPoint.y() - m_sPointPress.y();
            qreal abslength = abs(w) - abs(h);
            if (abslength >= 0.1) {
                if (h >= 0) {
                    resultPoint.setY(m_sPointPress.y() + abs(w));
                } else {
                    resultPoint.setY(m_sPointPress.y() - abs(w));
                }

            } else {
                if (w >= 0) {
                    resultPoint.setX(m_sPointPress.x() + abs(h));
                } else {
                    resultPoint.setX(m_sPointPress.x() - abs(h));
                }
            }
        } else if (!m_bShiftKeyPress && m_bAltKeyPress) {

        }





        QRectF rectF(m_sPointPress, resultPoint);
        QRectF normalRectF = rectF.normalized();
//        QRectF result = normalRectF;
//        if (m_bShiftKeyPress && !m_bAltKeyPress) {
//            qreal size = (result.width() - result.height() > 0.00001) ? result.width() : result.height();
//            result.setWidth(size);
//            result.setHeight(size);
//        }
        m_pRectItem->setRect(normalRectF);
    }
}

void CRectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    m_sPointRelease = event->scenePos();
    m_pRectItem = nullptr;
    m_bMousePress = false;

    //TODO 如果没有拖动的功能   是否删除矩形
}

