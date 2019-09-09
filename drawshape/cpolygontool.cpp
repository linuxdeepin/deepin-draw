#include "cpolygontool.h"
#include "cdrawscene.h"
#include "cgraphicsellipseitem.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtMath>

CPolygonTool::CPolygonTool()
    : IDrawTool(polygon)
{

}

CPolygonTool::~CPolygonTool()
{

}

void CPolygonTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();
        m_sPointPress = event->scenePos();
        int num = CDrawParamSigleton::GetInstance()->getSideNum();
        m_pPolygonItem = new CGraphicsPolygonItem(num, m_sPointPress.x(), m_sPointPress.y(), 0, 0);
        m_pPolygonItem->setPen(CDrawParamSigleton::GetInstance()->getPen());
        m_pPolygonItem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
        scene->addItem(m_pPolygonItem);
        m_pPolygonItem->setSelected(true);
        m_bMousePress = true;
    } else {
        scene->mouseEvent(event);
    }
}

void CPolygonTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        QRectF resultRect;
        bool shiftKeyPress = CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
        bool altKeyPress = CDrawParamSigleton::GetInstance()->getAltKeyStatus();
        //按下SHIFT键
        if (shiftKeyPress && !altKeyPress) {
            QPointF resultPoint = pointMouse;
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
            QRectF rectF(m_sPointPress, resultPoint);
            resultRect = rectF.normalized();

        }
        //按下ALT键
        else if (!shiftKeyPress && altKeyPress) {

            QPointF point1 = pointMouse;
            QPointF centerPoint = m_sPointPress;
            QPointF point2 = 2 * centerPoint - point1;
            QRectF rectF(point1, point2);
            resultRect = rectF.normalized();
        }
        //ALT SHIFT都按下
        else if (shiftKeyPress && altKeyPress) {
            QPointF resultPoint = pointMouse;
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

            QPointF point1 = resultPoint;
            QPointF centerPoint = m_sPointPress;
            QPointF point2 = 2 * centerPoint - point1;
            QRectF rectF(point1, point2);
            resultRect = rectF.normalized();

        }
        //都没按下
        else {
            QPointF resultPoint = pointMouse;
            QRectF rectF(m_sPointPress, resultPoint);
            resultRect = rectF.normalized();
        }
        m_pPolygonItem->setRect(resultRect);
    }
}

void CPolygonTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    m_sPointRelease = event->scenePos();
    //如果鼠标没有移动
    if ( event->scenePos() == m_sPointPress ) {
        if ( m_pPolygonItem != nullptr)
            scene->removeItem(m_pPolygonItem);
        delete m_pPolygonItem;
    }
    m_pPolygonItem = nullptr;
    m_bMousePress = false;

    //TODO 如果没有拖动的功能   是否删除矩形
}
