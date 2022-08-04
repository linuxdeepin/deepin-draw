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
#ifndef COLORPICKWIDGET_H
#define COLORPICKWIDGET_H
#include <DArrowRectangle>

DWIDGET_USE_NAMESPACE
class ColorSettingWgt;

class ColorPickWidget : public DArrowRectangle
{
    Q_OBJECT
public:
    ColorPickWidget(QWidget *parent = nullptr);
    void  setVar(const QVariant &var);
    QWidget *caller();
    void setShowPos(const QPoint &pos);

    void setCaller(QWidget *pCaller);
signals:
    void colorChanged(const QColor &color, int phase);
protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual void resizeEvent(QResizeEvent *e) override;
public slots:
    void setColor(const QColor &color, int phase);
private:
    QColor      m_color;
    ColorSettingWgt *m_colorSettingWgt;
    QWidget      *m_caller = nullptr;
    QPoint      m_point;
};

#endif // COLORPICKWIDGET_H
