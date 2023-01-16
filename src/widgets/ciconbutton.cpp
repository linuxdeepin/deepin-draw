// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ciconbutton.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"

#include <QPainter>
#include <QDebug>

#include <DPalette>
#include <DApplicationHelper>

#include <QMouseEvent>

CIconButton::CIconButton(const QMap<int, QMap<EIconButtonSattus, QString> > &pictureMap, const QSize &size, DWidget *parent, bool isCheckLock) :
    DIconButton(parent),
    m_isHover(false),
    m_isChecked(false),
    m_isPressed(false),
    m_isCheckLock(isCheckLock),
    m_currentStatus(Normal),
    m_currentTheme(DGuiApplicationHelper::LightType),
    m_pictureMap(pictureMap)
{
    setFixedSize(size);
    setIconSize(size);

    m_currentTheme = DGuiApplicationHelper::instance()->themeType();

    updateImage();
}

void CIconButton::setChecked(bool checked)
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


bool CIconButton::isChecked() const
{
    return m_isChecked;
}

void CIconButton::mousePressEvent(QMouseEvent *e)
{
//    Q_UNUSED(e)

    if (m_isChecked && m_isCheckLock) {
        return;
    }

    m_isPressed = true;

    m_currentStatus = Press;

    m_isChecked = !m_isChecked;

    updateImage();

    emit buttonClick();
    DIconButton::mousePressEvent(e);
}

void CIconButton::mouseReleaseEvent(QMouseEvent *e)
{
//    Q_UNUSED(e)

    m_isPressed = false;

    if (m_isChecked) {
        m_currentStatus = Active;
    } else {
        m_currentStatus = Normal;
    }

    updateImage();

    if (e->button() == Qt::LeftButton) {
        emit mouseRelease();
    }

    DIconButton::mouseReleaseEvent(e);
}

void CIconButton::enterEvent(QEvent *e)
{
    m_isHover = true;
    m_tmpStatus = m_currentStatus;
    m_currentStatus = Hover;
    updateImage();
    DIconButton::enterEvent(e);
}

void CIconButton::leaveEvent(QEvent *e)
{
    m_isHover = false;

    if (m_currentStatus == Hover) {
        m_currentStatus = m_tmpStatus;
        updateImage();
    }
    DIconButton::leaveEvent(e);
}

void CIconButton::updateImage()
{
    if (!m_iconMode) {
        setIcon(QIcon(m_pictureMap[m_currentTheme][m_currentStatus]));
    }
}

void CIconButton::setTheme(int currentTheme)
{
    m_currentTheme = currentTheme;

    updateImage();

}

void CIconButton::setIconMode()
{
    m_iconMode = true;
}

//void CIconButton::setButtonIcon(QIcon t_icon)
//{
//    m_normalIcon = t_icon;
//}

