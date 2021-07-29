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
#include <QMetaType>

#include "../testItems/publicApi.h"

#include "application.h"
#include "mainwindow.h"
#include "shortcut.h"

#include "QTestEventList"

#define protected public
#define private public

#include "ccutdialog.h"
#include "cexportimagedialog.h"
#include "cprintmanager.h"
#include "drawdialog.h"

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

    //button
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&cutDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "buttonClicked", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(QString, "123"));
    });
    ASSERT_EQ(cutDialog.getCutStatus(), CCutDialog::Discard);

    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&cutDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "buttonClicked", Qt::QueuedConnection, Q_ARG(int, 1), Q_ARG(QString, "123"));
    });
    ASSERT_EQ(cutDialog.getCutStatus(), CCutDialog::Save);
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
    //slotOnSavePathChange
    exportDialog.slotOnSavePathChange(CExportImageDialog::Documents);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Downloads);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Music);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Other);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Videos);
    exportDialog.slotOnSavePathChange(99999);

    //slotOnFormatChange
    exportDialog.slotOnFormatChange(CExportImageDialog::PDF);

    //slotOnQualityChanged
    exportDialog.slotOnQualityChanged(1);
    ASSERT_EQ(exportDialog.getQuality(), 1);

    //slotOnDialogButtonClick
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

    //showQuestionDialog
    exportDialog.showQuestionDialog("123");
}

TEST(dialog, CPrintManager)
{
    //由于QMetaObject::invokeMethod机制，需要采用一点特殊方法来使其生效
    CPrintManager printManager;

    //showPrintDialog
    qRegisterMetaType<DWidget *>("DWidget *");
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(qApp, [ & ]() {
            printManager.showPrintDialog(QImage(), nullptr, "");
        });
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(qApp, [ & ]() {
            printManager.showPrintDialog(QImage(500, 500, QImage::Format_RGB32), nullptr, "");
        });
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });
}

TEST(dialog, ProgressDialog)
{
    //ProgressDialog
    QWidget widget;
    ProgressDialog progressDialog("321", &widget);

    progressDialog.setText("123");
    progressDialog.setProcess(1, 100);

    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&progressDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    //CAbstractProcessDialog
    CAbstractProcessDialog abstractProcessDialog;
    abstractProcessDialog.setTitle("123");
    abstractProcessDialog.setProcess(1);

    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&abstractProcessDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        QDialog *dialog = qobject_cast<QDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });
}

TEST(dialog, DrawDialog)
{
    DrawDialog drawDialog;

    //key event
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&drawDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
        DTestEventList e;
        e.addKeyPress(Qt::Key::Key_Escape);//这个会让它退出去，不需要执行done
        e.simulate(dialog);
    });
}

#endif
