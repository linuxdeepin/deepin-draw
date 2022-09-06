// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
