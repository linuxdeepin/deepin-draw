#include "ctexttool.h"
#include "cgraphicstextitem.h"
#include "cdrawscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
CTextTool::CTextTool()
    : IDrawTool(text)
{

}

CTextTool::~CTextTool()
{

}

void CTextTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        CGraphicsTextItem *item = new CGraphicsTextItem(QObject::tr("请输入文字"));
        item->setPos(event->scenePos());
        scene->addItem(item);
        item->setFocus();
        QTextCursor textCursor = item->textCursor();
        textCursor.select(QTextCursor::Document);
        item->setTextCursor(textCursor);
    } else {
        scene->mouseEvent(event);
    }
}

void CTextTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    scene->mouseEvent(event);
}

void CTextTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    scene->mouseEvent(event);
}
