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
    Q_UNUSED(event)
    CGraphicsTextItem *item = new CGraphicsTextItem(QObject::tr("请输入文字"));
    item->setPos(event->scenePos());
    scene->addItem(item);
    item->setFocus();

    //textCursor.setBlockFormat()
    //item->setSelected(true);

//    QTextCursor textCursor = item->textCursor();
//    textCursor.setPosition(0, QTextCursor::MoveAnchor);
//    textCursor.setPosition(4, QTextCursor::KeepAnchor);
    //textCursor.select(QTextCursor::Document);
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
