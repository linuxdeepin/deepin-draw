/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "pushbutton.h"

#include  <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>

PushButton::PushButton(DWidget *parent)
    : DPushButton(parent)
    , m_checked(false)
    , m_isPressed(false)
    , m_isHovered(false)
    , m_isLocked(false)
    , m_spacing(2)
{
    setCheckable(true);
}

QString PushButton::normalPic() const
{
    return m_normalPic;
}

QString PushButton::hoverPic() const
{
    return m_hoverPic;
}

QString PushButton::pressPic() const
{
    return m_pressPic;
}

QString PushButton::disablePic() const
{
    return m_disablePic;
}

QString PushButton::text() const
{
    return m_text;
}

QColor PushButton::normalColor() const
{
    return m_normalColor;
}

QColor PushButton::hoverColor() const
{
    return m_hoverColor;
}

QColor PushButton::pressColor() const
{
    return m_pressColor;
}

QColor PushButton::disableColor() const
{
    return m_disableColor;
}

//void PushButton::setNormalPic(QString normalPic)
//{
//    if (m_normalPic == normalPic)
//        return;

//    m_normalPic = normalPic;
//    m_currentPic = normalPic;
//    emit normalPicChanged(normalPic);
//}

//void PushButton::setHoverPic(QString hoverPic)
//{
//    if (m_hoverPic == hoverPic)
//        return;

//    m_hoverPic = hoverPic;
//    emit hoverPicChanged(hoverPic);
//}

//void PushButton::setPressPic(QString pressPic)
//{
//    if (m_pressPic == pressPic)
//        return;

//    m_pressPic = pressPic;
//    emit pressPicChanged(pressPic);
//}

//void PushButton::setDisablePic(QString disablePic)
//{
//    if (m_disablePic == disablePic)
//        return;

//    m_disablePic = disablePic;
//    emit disablePicChanged(disablePic);
//}

void PushButton::setText(const QString &text)
{
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged(text);
}

//void PushButton::setNormalColor(QColor normalColor)
//{
//    if (m_normalColor == normalColor)
//        return;

//    m_normalColor = normalColor;
//    m_currentColor = normalColor;
//    emit normalColorChanged(normalColor);
//}

//void PushButton::setHoverColor(QColor hoverColor)
//{
//    if (m_hoverColor == hoverColor)
//        return;

//    m_hoverColor = hoverColor;
//    emit hoverColorChanged(hoverColor);
//}

//void PushButton::setPressColor(QColor pressColor)
//{
//    if (m_pressColor == pressColor)
//        return;

//    m_pressColor = pressColor;
//    emit pressColorChanged(pressColor);
//}

//void PushButton::setDisableColor(QColor disableColor)
//{
//    if (m_disableColor == disableColor)
//        return;

//    m_disableColor = disableColor;
//    emit disableColorChanged(disableColor);
//}

//void PushButton::setCheckedPic(QString checkedPic)
//{
//    if (m_checkedPic == checkedPic)
//        return;

//    m_checkedPic = checkedPic;
//    emit checkedPicChanged(checkedPic);
//}

void PushButton::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

//    QMargins m = contentsMargins();
//    int ph = 0;
//    int spacing = 0;
    QPixmap pixmap(getPixmap());
    this->setFixedSize(pixmap.size());
    if (! pixmap.isNull()) {
        painter.drawPixmap(this->rect(), pixmap);
//        if (pixmap.width() > width() || pixmap.height() > height()) {
//            ph = height() - m.top() - m.bottom();
//            const QRect pr(m.left(), (height() - ph) / 2, ph, ph);
//            painter.drawPixmap(pr, pixmap.scaled(pr.size(), Qt::KeepAspectRatioByExpanding));
//        }
//        else {
//            ph = pixmap.height();
//            const QRect pr(m.left(), (height() - ph) / 2, pixmap.width(), ph);
//            painter.drawPixmap(pr, pixmap);
//        }
//        spacing = m_spacing;
    }

//    QFontMetrics fm(font());
//    const int tw = width() - m.left() - spacing - ph - m.right();
//    const int th = fm.height();
//    const QRect textRect(m.left() + ph + spacing, (height() - th) / 2,
//                   tw, th);
//    const QString mt = fm.elidedText(m_text, Qt::ElideMiddle, tw);
//    painter.setPen(QPen(getTextColor()));
//    painter.drawText(textRect, Qt::AlignCenter, mt);
}

void PushButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)
    m_isHovered = true;
    m_currentPic = hoverPic();
    m_currentColor = hoverColor();
//    setCursor(Qt::PointingHandCursor);
    this->update();
}

void PushButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)
    m_isHovered = false;
    m_currentColor = normalColor();
    m_currentPic = normalPic();
//    setCursor(Qt::ArrowCursor);
    this->update();

    emit mouseLeave();
}

void PushButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    m_isPressed = true;
    m_currentColor = pressColor();
    m_currentPic = pressPic();

    if (m_isCheckable) {
        if (!m_checked)
            setChecked(true);
        else {
            if (!m_isLocked)
                setChecked(!m_checked);
        }
    }

    this->update();
}

void PushButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    m_currentColor = normalColor();
    m_currentPic = normalPic();
    this->update();
    if (m_isPressed) {
        m_isPressed = false;
        emit clicked();
    }
}

QString PushButton::getPixmap() const
{
    if (m_checked) {
        return checkedPic();
    } else if (isEnabled()) {
        return m_currentPic;
    } else {
        return disablePic();
    }
}

QColor PushButton::getTextColor() const
{
    if (isEnabled()) {
        return m_currentColor;
    } else {
        return disableColor();
    }
}

//bool PushButton::isCheckable()
//{
//    return m_isCheckable;
//}

void PushButton::setCheckable(bool checkeable)
{
    m_isCheckable = checkeable;
}

//void PushButton::lockedCheckedStatus(bool locked)
//{
//    m_isLocked = locked;
//}

//bool PushButton::getChecked() const
//{
//    return m_checked;
//}

void PushButton::setChecked(bool checked)
{
    m_checked = checked;
    this->update();
}

QString PushButton::checkedPic() const
{
    return m_checkedPic;
}

//int PushButton::getSpacing() const
//{
//    return m_spacing;
//}

void PushButton::setSpacing(int spacing)
{
    m_spacing = spacing;
}

