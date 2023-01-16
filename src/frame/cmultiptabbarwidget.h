// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CMULTIPTABBARWIDGET_H
#define CMULTIPTABBARWIDGET_H

#include <DTabBar>
#include <DMenu>
#include <DFileDialog>

DWIDGET_USE_NAMESPACE

/**
 * @description: 实现多标签拖拽选项栏
*/
class DrawBoard;
class TabBarWgt : public DTabBar
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

class FileSelectDialog: public DFileDialog
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


#endif // CMULTIPTABBARWIDGET_H
