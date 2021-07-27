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
#include <functional>

#include "../testItems/publicApi.h"

#include "application.h"
#include "mainwindow.h"
#include "shortcut.h"

#include "QTestEventList"

#define protected public
#define private public

#include "ccutdialog.h"
#include "cexportimagedialog.h"

#if TEST_DIALOG

//对话框自动打桩机
//输入：对话框对象、对话框运行时要执行的操作
template<typename T, typename U>
void stubDialog(T &&activeFun, U &&processFun)
{
    activeFun();//启动函数

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
            processFun(); //要执行的操作在这里
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
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&cutDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
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
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&exportDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    //show
    exportDialog.showMe();
    QThread::msleep(200);
    exportDialog.close();

    //slots
    exportDialog.slotOnSavePathChange(CExportImageDialog::Documents);

    exportDialog.slotOnFormatChange(CExportImageDialog::PDF);

    exportDialog.slotOnQualityChanged(1);
    ASSERT_EQ(exportDialog.getQuality(), 1);

    //第二个参数不起作用，只测试第一个参数，函数的后续行为和里面的编辑框内容有关，以此作为依据进行测试
    exportDialog.slotOnDialogButtonClick(0, "");

    exportDialog.m_fileNameEdit->setText("");
    exportDialog.slotOnDialogButtonClick(1, "");

    std::function<void(void)> activeFunc([ & ]() {
        QMetaObject::invokeMethod(&exportDialog, "slotOnDialogButtonClick", Qt::QueuedConnection,
                                  Q_ARG(int, 1), Q_ARG(const QString &, ""));
    });

    exportDialog.m_fileNameEdit->setText(".");
    stubDialog(activeFunc, [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    QString data("/");
    for (int i = 0; i < 300; ++i) {
        data.append('1');
    }
    exportDialog.m_fileNameEdit->setText(data);
    stubDialog(activeFunc, [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    exportDialog.m_fileNameEdit->setText(QApplication::applicationDirPath());
    activeFunc();

    exportDialog.m_fileNameEdit->setText(QApplication::applicationDirPath() + "/fhiushf");
    activeFunc();
}

#endif
