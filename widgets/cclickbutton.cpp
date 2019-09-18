/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
#include "cclickbutton.h"

#include <QPainter>
#include <QDebug>

CClickButton::CClickButton(const QMap<EClickBtnSatus, QString> &pictureMap, DWidget *parent) :
    DPushButton(parent),
    m_currentStatus(Normal),
    m_pictureMap(pictureMap)
{
    m_currentPicture = m_pictureMap[m_currentStatus];
}

void CClickButton::setDisable(bool isDisable)
{
    if (isDisable) {
        m_currentStatus = Disable;

    } else {
        m_currentStatus = Normal;
    }
    update();

    setEnabled(!isDisable);
}


void CClickButton::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

    QPixmap pixmap(m_pictureMap[m_currentStatus]);
    this->setFixedSize(pixmap.size());
    if (! pixmap.isNull()) {
        painter.drawPixmap(this->rect(), pixmap);
    }
}

void CClickButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Press;

    update();

    emit buttonClick();
}

void CClickButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Normal;

    update();

}

void CClickButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Hover;

    update();

}

void CClickButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Normal;

    update();

}
