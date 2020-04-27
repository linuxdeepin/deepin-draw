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
#include "cgraphicsproxywidget.h"
#include "cdrawparamsigleton.h"
#include "widgets/ctextedit.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QWidget>
#include <QGraphicsView>

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
        QFont font = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont();
        CGraphicsTextItem *item = new CGraphicsTextItem();
        item->getTextEdit()->setText(QObject::tr("Input text here"));
        item->getTextEdit()->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        item->getTextEdit()->selectAll();

        QFontMetrics fm(font);
        QSizeF size = item->getTextEdit()->document()->size();
        QRectF rect = item->rect();
        rect.setHeight(size.height());
        rect.setWidth(size.width());
        item->setRect(QRectF(m_sPointPress.x(), m_sPointPress.y(), rect.width(), rect.height()));
        if (scene->sceneRect().right() - m_sPointPress.x() > 0) {
            item->setLastDocumentWidth(scene->sceneRect().right() - m_sPointPress.x());
        } else {
            item->setLastDocumentWidth(0);
        }

        // 设置新建图元属性
        item->setFontSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSize());
        item->setFontFamily(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family());
        item->setTextFontStyle(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFontStyle());
        item->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());

        item->setZValue(scene->getMaxZValue() + 1);
        scene->addItem(item);
        emit scene->itemAdded(item);
        item->setSelected(true);
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
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
    emit scene->signalChangeToSelect();
}
