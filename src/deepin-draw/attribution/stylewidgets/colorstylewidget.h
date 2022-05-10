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
#ifndef FILLSTYLEWIDGET_H
#define FILLSTYLEWIDGET_H
#include "styleattriwidget.h"
#include <DCheckBox>
DWIDGET_USE_NAMESPACE

class CColorSettingButton;
class ColorStyleWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit ColorStyleWidget(QWidget *parent = nullptr);
    virtual void setVar(const QVariant &var) override;
    void setTitleText(QString text);
    void setColorFill(int fillStyle);//0 fill 1 border
    void setEnableAttriVisible(bool bVisible);
    bool isEnableAttriVisible();
    bool isAttriApply();
    void setColorTextVisible(bool bVisible);
    void addWidget(QWidget *w);
signals:
    void colorChanged(const QColor &color, int phase);
private:
    void initUi();
    void enableColorEdit(bool bEnable);
private:
    CColorSettingButton *m_fillColor = nullptr;
    DLineEdit           *m_fillColorEdit = nullptr;
    DCheckBox           *m_bEnableAtrri = nullptr;
    DLabel              *m_titleLabel = nullptr;
    bool                m_bEnableAttriVisible = true;
    QHBoxLayout         *m_lFillColor = nullptr;
};

#endif // FILLSTYLEWIDGET_H
