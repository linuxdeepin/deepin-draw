/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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
#ifndef ROTATEATTRIWIDGET_H
#define ROTATEATTRIWIDGET_H
#include <QPushButton>
#include <QLineEdit>
#include <DDoubleSpinBox>

#include "attributewidget.h"
DWIDGET_USE_NAMESPACE
class DrawBoard;
class PageItem;
class PageScene;

class RotateAttriWidget: public AttributeWgt
{
    Q_OBJECT
public:
    RotateAttriWidget(DrawBoard *drawBoard, QWidget *parent = nullptr);
    void setVar(const QVariant &var) override;
    void setAngle(double angle);
private:
    void showTooltip();
private:
    double checkValue(double value);
signals:
    void buttonClicked(bool doHorFlip, bool doVerFlip);

protected:
    Q_SLOT void onSceneSelectionChanged(const QList<PageItem * > &selectedItems);
private:
    QLabel *m_label;
    DDoubleSpinBox *m_angle;
    DIconButton *m_horFlipBtn;
    DIconButton *m_verFlipBtn;
    DIconButton *m_clockwiseBtn;
    DIconButton *m_anticlockwiseBtn;
    DrawBoard   *m_drawBoard = nullptr;
    PageScene   *m_currentScene = nullptr;
};
#endif // ROTATEATTRIWIDGET_H
