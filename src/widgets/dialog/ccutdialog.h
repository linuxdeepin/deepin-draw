/*
* Copyright (C) 2020 ~ 2021 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: WangYu<wangyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
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
private:
    CutStatus m_cutStatus;
};

#endif // CCUTDIALOG_H
