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
#include "resizelabel.h"

#include <QPainter>

ResizeLabel::ResizeLabel(DWidget *parent)
    : DLabel(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void ResizeLabel::paintResizeLabel(bool drawing, FourPoints fpoints)
{
    if (drawing) {
        this->show();
    } else {
        this->hide();
    }
    m_drawArtboard = drawing;
    m_artboardMPoints = fpoints;
    update();
}

ResizeLabel::~ResizeLabel()
{
}

void ResizeLabel::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if (!m_drawArtboard)
        return;

    else {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform);
        painter.setBrush(Qt::transparent);
        QPen dragPen;
        dragPen.setColor(QColor("#888888"));
        dragPen.setStyle(Qt::DashLine);
        painter.setPen(dragPen);
        m_artboardMPoints[3] = QPointF(std::max(m_artboardMPoints[3].x(),
                                                m_artboardMPoints[0].x() + 20), std::max(m_artboardMPoints[3].y(),
                                                                                         m_artboardMPoints[0].y() + 20));
        QRect rect = QRect(int(m_artboardMPoints[0].x()),
                           int(m_artboardMPoints[0].y()),
                           int(m_artboardMPoints[3].x() - m_artboardMPoints[0].x()),
                           int(m_artboardMPoints[3].y() - m_artboardMPoints[0].y()));
        painter.drawRect(rect);
    }
}
