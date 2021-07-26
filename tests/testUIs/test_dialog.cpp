/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     WangZhengYang <wangzhengyang@uniontech.com>
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
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QtConcurrent/QtConcurrent>

#include "../testItems/publicApi.h"

#include "application.h"
#include "mainwindow.h"
#include "shortcut.h"

#include "QTestEventList"

#include "ccutdialog.h"
#include "cexportimagedialog.h"

#if TEST_DIALOG

//对话框自动打桩机
//输入：对话框对象、对话框运行时要执行的操作
template<typename T>
void stubDialog(QDialog *dialog, T &&fun)
{
    QMetaObject::invokeMethod(dialog, "exec", Qt::QueuedConnection);

    QEventLoop loop;
    QtConcurrent::run([ =, &loop]() {
        (void)QTest::qWaitFor([ =, &loop]() {
            return (loop.isRunning());
        });
        (void)QTest::qWaitFor([ = ]() {
            return (qApp->activeModalWidget() != nullptr && qApp->activeModalWidget() != getMainWindow());
        });
        if (qApp->activeModalWidget() != nullptr) {
            QThread::msleep(200);
            fun(); //要执行的操作在这里
            QThread::msleep(200);
            QMetaObject::invokeMethod(&loop, "quit");
        } else {
            QMetaObject::invokeMethod(&loop, "quit");
        }
    });
    loop.exec();
}

TEST(dialog, cutdialog)
{
    CCutDialog cutDialog;
    ASSERT_EQ(cutDialog.getCutStatus(), CCutDialog::Discard);

    //key event
    stubDialog(&cutDialog, [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        DTestEventList e;
        e.addKeyPress(Qt::Key::Key_Escape);//这个会让它退出去，不需要执行done
        e.simulate(dialog);
    });
}

TEST(dialog, exportimagedialog)
{
    CExportImageDialog exportDialog;

    //exec
    stubDialog(&exportDialog, [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    //show
    exportDialog.showMe();
    QThread::sleep(200);
    exportDialog.close();
}

#endif
