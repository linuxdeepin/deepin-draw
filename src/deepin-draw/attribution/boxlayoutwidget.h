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
#ifndef HBOXLAYOUTWIDGET_H
#define HBOXLAYOUTWIDGET_H

#include <QWidget>

class QBoxLayout;
class BoxLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoxLayoutWidget(QWidget *parent = nullptr, int hMargin = 5, int vMargin = 0);
    void addWidget(QWidget *w, int stretch = 0);
    void resetLayout(QBoxLayout *l);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    void init();
    void setColor(QColor c);
signals:

private:
    QColor  m_color;//背景色
    QBoxLayout  *m_layout = nullptr;
    int m_hMargin;
    int m_vMargin;
};

#endif // HBOXLAYOUTWIDGET_H
