#include "cgraphicsview.h"
#include "drawshape/cdrawparamsigleton.h"
#include <QWheelEvent>

CGraphicsView::CGraphicsView(QWidget *parent)
    : DGraphicsView (parent)
    , m_scale(1)
{

}

void CGraphicsView::zoomOut()
{
    this->scale(1.1 * m_scale);
    emit signalSetScale(m_scale);
}

void CGraphicsView::zoomIn()
{
    this->scale(0.9 * m_scale);
    emit signalSetScale(m_scale);
}

void CGraphicsView::scale(qreal scale)
{
    qreal multiple = scale / m_scale;
    DGraphicsView::scale(multiple, multiple);
    m_scale = scale;
}

void CGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (CDrawParamSigleton::GetInstance()->getCtlKeyStatus()) {
        if (event->delta() > 0) {
            zoomOut();

        } else {
            zoomIn();
            emit signalSetScale(m_scale);
        }
    }
}
