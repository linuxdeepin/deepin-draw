/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef BORDERCOLORBUTTON_H
#define BORDERCOLORBUTTON_H

#include <DPushButton>
#include <QWidget>
#include "globaldefine.h"

DWIDGET_USE_NAMESPACE

class BorderColorButton : public DPushButton
{
    Q_OBJECT
public:
    BorderColorButton(DWidget *parent = nullptr);
    ~BorderColorButton();

    void setColor(QColor color, EChangedPhase phase = EChanged);
    QColor getColor() const;

    void setColorIndex(int index);
    QColor getColorByIndex() const;

    void updateCheckedStatus();
    void updateConfigColor();
    void resetChecked();

    void setIsMultColorSame(bool isMultColorSame);
    bool getIsMultColorSame() const;

    void setButtonText(QString text);
    QString getButtonText() const;

signals:
    void colorChanged(const QColor &color, EChangedPhase phase);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void paintLookStyle(QPainter *painter, bool isMult = false);

private:
    QString m_text;
    QColor m_color;
    bool m_isHover;
    bool m_isChecked;
    bool m_isMultColorSame;//多选图元，颜色不一致时使用,单选的时候默认为true
    int m_textWidth; // 字符的宽度，用于自适应文字的宽度大小
};

#endif // BORDERCOLORBUTTON_H
