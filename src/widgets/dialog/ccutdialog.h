// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CCUTDIALOG_H
#define CCUTDIALOG_H

#include "dialog.h"

class CCutDialog : public DDialog
{
    Q_OBJECT
public:
    explicit CCutDialog(DWidget *parent = nullptr);

    enum CutStatus {
        Save = 0,  // 确认裁剪
        Cancel, // 保持现状
        Discard// 取消裁剪
    };

    /*
    * @bref: getCutStatus 获取裁剪的状态选择结果
    * @return: CutStatus 需要执行的裁剪结果
    */
    CutStatus getCutStatus();


    int exec() override;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void showEvent(QShowEvent *event) override;
private:
    CutStatus m_cutStatus;
};

#endif // CCUTDIALOG_H
