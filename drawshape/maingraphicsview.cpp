#include "maingraphicsview.h"
#include "widgets/graphicsgloweffect.h"



MainGraphicsView::MainGraphicsView(QGraphicsScene *scene) :
    QGraphicsView(scene)
{
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}


