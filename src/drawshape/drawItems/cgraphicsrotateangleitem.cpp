/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "cgraphicsrotateangleitem.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "cgraphicsitem.h"
#include "cselecttool.h"
#include "cdrawscene.h"
#include "frame/cgraphicsview.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>

//CGraphicsRotateAngleItem::CGraphicsRotateAngleItem(qreal rotateAngle, qreal scale, QGraphicsItem *parent)
//    : QGraphicsRectItem(parent)
//    , m_rotateAngle(rotateAngle)
//{
//    m_width = 45.;
//    m_height = 20.;
//    m_fontSize = 11.;
//    m_scale = scale;
//    m_width = m_width / scale;
//    m_height = m_height / scale;
//    m_fontSize = m_fontSize / scale;
//    m_textFont.setPointSizeF(m_fontSize);

//    setRect(0, 0, m_width, m_height);

//    setFlag(ItemIsSelectable, false);
//}

//CGraphicsRotateAngleItem::CGraphicsRotateAngleItem(const QRectF &rect, qreal rotateAngle, QGraphicsItem *parent)
//    : QGraphicsRectItem(rect, parent)
//    , m_rotateAngle(rotateAngle)

//{
//    setRect(rect);
//}

//void CGraphicsRotateAngleItem::updateRotateAngle(qreal rotateAngle, QPointF pos)
//{
//    CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
//    qreal scaleTotal = pView != nullptr ? pView->getDrawParam()->getScale() : 1.0;

//    m_rotateAngle =  rotateAngle;
//    QString angle = QString("%1°").arg(QString::number(m_rotateAngle, 'f', 1));

//    QFontMetrics fontMetrics(m_textFont);
//    m_width = fontMetrics.width(angle);

//    setRect(pos.x(), pos.y(), m_width / scaleTotal, m_height / scaleTotal);
//}

////bool CGraphicsRotateAngleItem::isFatherRotating()
////{
////    CGraphicsItem *pParentItem = dynamic_cast<CGraphicsItem *>(parentItem());
////    if (pParentItem != nullptr && pParentItem->scene() != nullptr) {
////        EDrawToolMode model = pParentItem->drawScene()->getDrawParam()->getCurrentDrawToolMode();
////        int operatingTp = pParentItem->operatingType();
////        if (operatingTp == CSelectTool::ERotateMove && model == selection) {
////            return true;
////        }
////    }
////    return false;
////}

//void CGraphicsRotateAngleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{
//    Q_UNUSED(option)
//    Q_UNUSED(widget)

//    painter->save();
//    painter->setClipping(false);

//    painter->setPen(Qt::NoPen);
//    painter->setBrush(QColor("#E5E5E5"));
//    painter->drawRoundRect(rect());

//    QString angle = QString("%1°").arg(QString::number(m_rotateAngle, 'f', 1));
//    painter->setPen(Qt::black);

//    QFont f = m_textFont;

//    f.setPointSizeF(f.pointSizeF() / option->levelOfDetailFromTransform(painter->worldTransform()));

//    painter->setFont(f);
//    painter->drawText(rect(), Qt::AlignCenter, angle);

//    painter->restore();
//}
