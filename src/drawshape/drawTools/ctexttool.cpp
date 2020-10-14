/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ctexttool.h"
#include "cdrawscene.h"
#include "cgraphicstextitem.h"
#include "cdrawparamsigleton.h"
#include "widgets/ctextedit.h"
#include "frame/cgraphicsview.h"

#include "service/cmanagerattributeservice.h"

#include <QGraphicsView>

CTextTool::CTextTool()
    : IDrawTool(text)
{

}

CTextTool::~CTextTool()
{

}

void CTextTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsTextItem *pItem = dynamic_cast<CGraphicsTextItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            if (pItem->scene() == nullptr) {
                emit event->scene()->itemAdded(pItem);
            }
            pItem->makeEditabel();
            pItem->getTextEdit()->document()->clearUndoRedoStacks();
            event->scene()->selectItem(pItem);
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CTextTool::creatItem(IDrawTool::CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {
        CGraphicsTextItem *pItem =  new CGraphicsTextItem();
        pItem->setPos(event->pos().x(), event->pos().y());
        pItem->getTextEdit()->setText(QObject::tr("Input text here"));
        pItem->getTextEdit()->setAlignment(Qt::AlignLeft);
        pItem->getTextEdit()->selectAll();

        CGraphicsView *pView = event->scene()->drawView();

        QFontMetrics fm(pView->getDrawParam()->getTextFont());
        QSizeF size = pItem->getTextEdit()->document()->size();
        // 设置默认的高度会显示不全,需要设置为字体高度的1.4倍
        pItem->setRect(QRectF(m_sPointPress.x(), m_sPointPress.y(), size.width(), fm.height() * 1.4));

        if (event->scene()->sceneRect().right() - m_sPointPress.x() > 0) {
            pItem->setLastDocumentWidth(event->scene()->sceneRect().right() - m_sPointPress.x());
        } else {
            pItem->setLastDocumentWidth(0);
        }

        // 设置新建图元属性
        pItem->setFontSize(pView->getDrawParam()->getTextFont().pointSize());
        pItem->setFontFamily(pView->getDrawParam()->getTextFont().family());
        pItem->setTextFontStyle(pView->getDrawParam()->getTextFontStyle());
        pItem->setTextColor(pView->getDrawParam()->getTextColor());
        pItem->setTextColorAlpha(pView->getDrawParam()->getTextColor().alpha());

        pItem->getTextEdit()->document()->clearUndoRedoStacks();
        qDebug() << "pItem->getTextEdit()->document() undo steps = " << pItem->getTextEdit()->document()->availableUndoSteps();

        //pItem->setZValue(event->scene()->getMaxZValue() + 1);
        qreal newZ = event->scene()->getMaxZValue() + 1;
        pItem->setZValue(newZ);
        event->scene()->setMaxZValue(newZ);
        event->scene()->addItem(pItem);

        return pItem;
    }
    return nullptr;
}

int CTextTool::minMoveUpdateDistance()
{
    return 0;
}

bool CTextTool::isPressEventHandledByQt(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)

//    if (pInfo->businessItem != nullptr) {
//        return false;
//    }
//    return true;
    return IDrawTool::isPressEventHandledByQt(event, pInfo);
}
