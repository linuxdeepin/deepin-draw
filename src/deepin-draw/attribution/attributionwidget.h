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
#ifndef ATTRIBUTIONWIDGET_H
#define ATTRIBUTIONWIDGET_H

#include <QWidget>
#include "attributemanager.h"

class AttributionWidget : public QWidget, public AttributionManager
{
    Q_OBJECT
public:
    explicit AttributionWidget(QWidget *parent = nullptr);
    void addAttriWidget(QWidget *w);
    void removeAttriWidget(QWidget *w);
    void removeAll();
protected:
    void showAt(int active, const QPoint &pos) override;
    void showWidgets(int active,
                     const QList<QWidget *> &oldWidgets, const QList<QWidget *> &exposeWidgets) override;
private:
    void initWidgetOrder();
signals:

private:
    QList<QWidget *> m_childWidgets;
    QList<int> m_attriShowOrder;
};

#endif // ATTRIBUTIONWIDGET_H
