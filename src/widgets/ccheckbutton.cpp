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
#include "ccheckbutton.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"

#include <QPainter>
#include <QDebug>

#include <DPalette>
#include <DApplicationHelper>
#include <DWidget>

#include <QMouseEvent>

CCheckButton::CCheckButton(const QMap<int, QMap<EButtonSattus, QString> > &pictureMap, const QSize &size, DWidget *parent, bool isCheckLock) :
    DToolButton(parent),
    m_isHover(false),
    m_isChecked(false),
    m_isPressed(false),
    m_isCheckLock(isCheckLock),
    m_currentStatus(Normal),
    m_currentTheme(DGuiApplicationHelper::LightType),
    m_pictureMap(pictureMap)
{
    setFixedSize(size);


//    if (size.width() <= 36) {
//        setIconSize(size);
//    } else {
//        setIconSize(QSize(60, 60));
//    }
    setIconSize(size);
    DPalette pa = DApplicationHelper::instance()->palette(this);
    pa.setColor(DPalette::Button, Qt::transparent);
    DApplicationHelper::instance()->setPalette(this, pa);
    this->setAutoFillBackground(true);
    // this->setFlat(true);

//    qDebug() << "!!!!!" << CDrawParamSigleton::GetInstance()->getThemeType();

    m_currentTheme = CManageViewSigleton::GetInstance()->getThemeType();

    updateImage();
}

void CCheckButton::setChecked(bool checked)
{
    if (checked) {
        m_currentStatus = Active;

    } else {
        if (hasFocus()) {
            clearFocus();
        }
        m_currentStatus = Normal;
    }

    m_isChecked = checked;

    updateImage();
}

//void CCheckButton::paintEvent(QPaintEvent *e)
//{
//    QPainter painter(this);
//    painter.setBrush(Qt::transparent);
//    DPushButton::paintEvent(e);
//}

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

    updateImage();

    emit buttonClick();
}


void CCheckButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_isHover = true;
    m_tmpStatus = m_currentStatus;
    m_currentStatus = Hover;
    updateImage();

}

void CCheckButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_isHover = false;

    if (m_currentStatus == Hover) {
        m_currentStatus = m_tmpStatus;
        updateImage();
    }
}

void CCheckButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_isPressed = false;

    m_currentStatus = Active;

    updateImage();
}

void CCheckButton::updateImage()
{
//    DPalette pa = DApplicationHelper::instance()->palette(this);

//    switch (m_currentStatus) {
//    case Normal:
//    case Active:
//        pa.setColor(DPalette::Button, Qt::transparent);
//        DApplicationHelper::instance()->setPalette(this, pa);
//        this->setAutoFillBackground(true);
//        this->setFlat(true);
//        break;
//    case Hover:
//    case Press:
//        pa.setColor(DPalette::Button, QColor("#e5e5e5"));
//        DApplicationHelper::instance()->setPalette(this, pa);
//        this->setAutoFillBackground(false);
//        this->setFlat(false);
//        break;
//    }
    setIcon(QIcon(m_pictureMap[m_currentTheme][m_currentStatus]));
}

//void CCheckButton::setCurrentTheme(int currentTheme)
//{
//    m_currentTheme = currentTheme;
//    updateImage();
//}

//int CCheckButton::getCurrentTheme() const
//{
//    return m_currentTheme;
//}

//CCheckButton::EButtonSattus CCheckButton::getCurrentStatus()
//{
//    return m_currentStatus;
//}

