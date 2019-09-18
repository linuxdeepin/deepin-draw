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
#include "ccheckbutton.h"

#include <QPainter>
#include <QDebug>

CCheckButton::CCheckButton(const QMap<EButtonSattus, QString> &pictureMap, DWidget *parent, bool isCheckLock) :
    DPushButton(parent),
    m_isHover(false),
    m_isChecked(false),
    m_isPressed(false),
    m_isCheckLock(isCheckLock),
    m_currentStatus(Normal),
    m_pictureMap(pictureMap)
{
    m_currentPicture = m_pictureMap[m_currentStatus];
}

void CCheckButton::setChecked(bool checked)
{
    if (checked) {
        m_currentStatus = Active;

    } else {
        m_currentStatus = Normal;
    }

    m_isChecked = checked;

    update();
}

void CCheckButton::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

    QPixmap pixmap(m_pictureMap[m_currentStatus]);
    setFixedSize(pixmap.size());
    if (! pixmap.isNull()) {
        painter.drawPixmap(rect(), pixmap);
    }
}

bool CCheckButton::isChecked() const
{
    return m_isChecked;
}

void CCheckButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    if (m_isChecked && m_isCheckLock) {
        return;
    }

    m_isPressed = true;

    m_currentStatus = Press;

    m_isChecked = !m_isChecked;

    update();

    emit buttonClick();
}

void CCheckButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_isPressed = false;

    if (m_isChecked) {
        m_currentStatus = Active;
    } else {
        m_currentStatus = Normal;
    }

    update();

}

void CCheckButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_isHover = true;
    m_tmpStatus = m_currentStatus;
    m_currentStatus = Hover;
    update();

}

void CCheckButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_isHover = false;

    if (m_currentStatus == Hover) {
        m_currentStatus = m_tmpStatus;
        update();
    }
}

