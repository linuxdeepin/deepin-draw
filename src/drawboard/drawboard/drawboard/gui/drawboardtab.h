/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao<zhanghao@uniontech.com>
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

#ifndef CMULTIPTABBARWIDGET_H
#define CMULTIPTABBARWIDGET_H

#include "config.h"

#include <QMenu>
#include <QFileDialog>
#include <QMouseEvent>

#ifdef USE_DTK
#include <DTabBar>
#define TABBAR DTabBar
DWIDGET_USE_NAMESPACE
#else
#include <QTabBar>
#define TABBAR QTabBar
#endif

#include <QStackedWidget>

/**
 * @description: 实现多标签拖拽选项栏
*/
class DrawBoard;
class TabBarWgt : public TABBAR
{
    Q_OBJECT
private:
    explicit TabBarWgt(DrawBoard *parent);
    ~TabBarWgt();

public:
    DrawBoard *pageManager() const;

    void addItem(const QString &name, const QString &key);
    void removeItem(const QString &key);

    int index(const QString &key)const;
    QString key(int index) const;

    Q_SLOT void onTabCountChanged(int index);


private:
    QMenu *menu()const;
    void mousePressEvent(QMouseEvent *event) override;

    friend class DrawBoard;
};

class FileSelectDialog: public QFileDialog
{
    Q_OBJECT
public:
    explicit FileSelectDialog(DrawBoard *parent = nullptr);

    DrawBoard *borad() const;

    int exec() override;

    QString resultFile()const;
private:
    void saveSetting();

private:
    QString _resultFile;

    friend class DrawBoard;

    friend class Page;
};


class PageStackedWidget: public QStackedWidget
{
    Q_OBJECT
public:
    using QStackedWidget::QStackedWidget;
    ~PageStackedWidget();
};

#endif // CMULTIPTABBARWIDGET_H
