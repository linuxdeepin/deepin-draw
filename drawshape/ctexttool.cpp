#include "ctexttool.h"
#include "cdrawscene.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cdrawparamsigleton.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QWidget>
#include <QGraphicsView>
#include <QTextEdit>

CTextTool::CTextTool()
    : IDrawTool(text)
{

}

CTextTool::~CTextTool()
{

}
#include <QLabel>
void CTextTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();
        m_sPointPress = event->scenePos();
//        QLabel *edit = new QLabel("请输入汉字");
//        edit->setStyleSheet("background-color: rgb(255, 0, 0);");
////        edit->setWindowFlags(Qt::FramelessWindowHint);
////        edit->setFrameShape(QTextEdit::NoFrame);
//        edit->resize(5, 200);
//        edit->show();



//        scene->addWidget(edit);
//        edit->setFocus();

        CGraphicsTextItem *item = new CGraphicsTextItem();
        item->setRect(QRectF(m_sPointPress.x(), m_sPointPress.y(), 200, 30));
        item->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        item->setTextColor(CDrawParamSigleton::GetInstance()->getTextColor());
        item->setFontSize(CDrawParamSigleton::GetInstance()->getTextSize());
        item->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        item->setSelected(true);
        scene->addItem(item);
        emit scene->itemAdded(item);
        item->getCGraphicsProxyWidget()->setFocus();
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
