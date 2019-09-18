#include "ctexttool.h"
#include "cdrawscene.h"
#include "cgraphicstextitem2.h"
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

void CTextTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();
        m_sPointPress = event->scenePos();
//        QTextEdit *edit = new QTextEdit("wangxin");
//        scene->addWidget(edit);
//        edit->setFocus();

        CGraphicsTextItem2 *item = new CGraphicsTextItem2();
        item->setRect(QRectF(m_sPointPress.x(), m_sPointPress.y(), 100, 100));
        item->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        item->setTextColor(CDrawParamSigleton::GetInstance()->getTextColor());
        item->setFontSize(CDrawParamSigleton::GetInstance()->getTextSize());
        item->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        item->setSelected(true);
        scene->addItem(item);
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
