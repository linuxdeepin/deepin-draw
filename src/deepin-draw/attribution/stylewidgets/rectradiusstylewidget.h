/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Tan Lang <tanlang@uniontech.com>
 *
 * Maintainer: Tan Lang <tanlang@uniontech.com>
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
#ifndef ROUNDEDANGLESTRYLEWIDGET_H
#define ROUNDEDANGLESTRYLEWIDGET_H
#include "attributewidget.h"
#include <DLabel>
#include <DWidget>
#include <DSpinBox>
#include <DSlider>
#include <DToolButton>
DWIDGET_USE_NAMESPACE

class CSpinBox;
class NumberSlider;
class BoxLayoutWidget;

class BackgroundWidget : public QWidget
{
public:
    using QWidget::QWidget;
    void setBackgroudColor(QColor c);
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QColor m_bkColor = Qt::transparent;
};

class RectRadiusStyleWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit RectRadiusStyleWidget(QWidget *parent = nullptr);
    void setVar(const QVariant &var) override;
private:
    void initUi();
    void initConnect();
    void emitValueChange(QVariant left, QVariant right, QVariant leftBottom, QVariant rightBottom, EChangedPhase phase = EChanged);
    void setSpinBoxValue(CSpinBox *s, int value);
    void showByChecked(bool bDiffMode = false);
signals:
    void valueChanged(QVariant value, EChangedPhase phase);
private:
    QButtonGroup *m_checkgroup = nullptr;
    CSpinBox    *m_left = nullptr;
    CSpinBox    *m_right = nullptr;
    CSpinBox    *m_leftBottom = nullptr;
    CSpinBox    *m_rightBottom = nullptr;
    DSlider    *m_radiusSlider = nullptr;
    CSpinBox        *m_radius = nullptr;
    DToolButton *m_sameRadiusButton = nullptr;
    DToolButton *m_diffRadiusButton = nullptr;
    BackgroundWidget     *m_diffRadiusWidget = nullptr;
    BoxLayoutWidget     *m_sameRadiusWidget = nullptr;
};

#endif // ROUNDEDANGLESTRYLEWIDGET_H
