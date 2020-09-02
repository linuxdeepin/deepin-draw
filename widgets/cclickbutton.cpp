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
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"

#include <DApplicationHelper>

#include <QPainter>
#include <QDebug>
#include <DWidget>

CClickButton::CClickButton(const QMap<int, QMap<EClickBtnSatus, QString> > &pictureMap, const QSize &size, DWidget *parent) :
    DToolButton(parent),
    m_currentStatus(Normal),
    m_pictureMap(pictureMap)
{
    setFixedSize(size);
    setIconSize(size);

//    qDebug() << "@@@@@@@" << CDrawParamSigleton::GetInstance()->getThemeType();

    DPalette pa = DApplicationHelper::instance()->palette(this);
    pa.setColor(DPalette::Button, Qt::transparent);
    DApplicationHelper::instance()->setPalette(this, pa);
    this->setAutoFillBackground(true);
//    this->setFlat(true);


    m_currentTheme = CManageViewSigleton::GetInstance()->getThemeType();

    updateImage();
}

void CClickButton::setDisable(bool isDisable)
{
    if (isDisable) {
        m_currentStatus = Disable;

    } else {
        m_currentStatus = Normal;
    }

    updateImage();

    setEnabled(!isDisable);
}


//void CClickButton::paintEvent(QPaintEvent *e)
//{
//    QWidget::paintEvent(e);
//    QPainter painter(this);

//    QPixmap pixmap(m_pictureMap[m_currentStatus]);
//    this->setFixedSize(pixmap.size());
//    if (! pixmap.isNull()) {
//        painter.drawPixmap(this->rect(), pixmap);
//    }
//}

void CClickButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Press;

    updateImage();

    emit buttonClick();
    DToolButton::mousePressEvent(e);
}

void CClickButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Normal;

    updateImage();

    DToolButton::mouseReleaseEvent(e);

}

void CClickButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Hover;

    updateImage();

    DToolButton::enterEvent(e);
}

void CClickButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Normal;

    updateImage();

    DToolButton::leaveEvent(e);
}

void CClickButton::updateImage()
{
    setIcon(QIcon(m_pictureMap[m_currentTheme][m_currentStatus]));
}

void CClickButton::setCurrentTheme(int currentTheme)
{
    m_currentTheme = currentTheme;
    updateImage();
}
