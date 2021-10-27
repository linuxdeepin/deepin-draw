/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
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

#ifndef CSIDEWIDTHWIDGET_H
#define CSIDEWIDTHWIDGET_H

#include <DFrame>
#include <QHBoxLayout>
#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class CSideWidthWidget : public DWidget
{
public:
    Q_OBJECT
public:
    explicit CSideWidthWidget(DWidget *parent = nullptr);

    void setText(const QString &text);

    void setSpace(int space);

    void setWidth(int width);
    QComboBox *menuComboBox();

    QSize sizeHint()const override;

signals:
    void widthChanged(int width, bool preview = false);

public:
    void setVaild(bool vaild);

private:
    QComboBox *m_menuComboBox;// 线宽度选择下拉框
    DLabel *_textLabel = nullptr;
    QHBoxLayout *m_layout;    // 水平布局器
    quint8 m_comboxHeight;    // 下拉框的高度
    DLabel *m_maskLable;

private:
    void initUI();
    void initConnection();

    /*
     * @bref: initLineWidthToCombox 初始化线宽度
    */
    void initLineWidthToCombox();
};

#endif // CSIDEWIDTHWIDGET_H
