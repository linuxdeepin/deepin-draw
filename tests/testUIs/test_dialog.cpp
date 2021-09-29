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
#include "cexportimagedialog_p.h"
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

    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    //exec
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&exportDialog, "exec", Qt::QueuedConnection);
    },
    [ = ]() {
        CExportImageDialog *dialog = qobject_cast<CExportImageDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    exportDialog._pPrivate->_radioPiexlBtn->setChecked(true);
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&exportDialog, "execFor", Qt::QueuedConnection, Q_ARG(Page *, c));
    },
    [ = ]() {
        CExportImageDialog *dialog = qobject_cast<CExportImageDialog *>(qApp->activeModalWidget());

        DTestEventList e;
        e.addMouseClick(Qt::LeftButton, Qt::NoModifier, QPoint(1, 1), 100);
        e.simulate(dialog->_pPrivate->_radioPiexlBtn);

        e.clear();
        e.addMouseClick(Qt::LeftButton, Qt::NoModifier);
        e.addKeyClick(Qt::Key_Backspace, Qt::NoModifier, 100);
        e.addKeyClick(Qt::Key_Tab, Qt::NoModifier, 100);
        e.addDelay(1000);
        e.simulate(dialog->_pPrivate->_widthEditor);
        e.simulate(dialog->_pPrivate->_heightEditor);

        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 1));
    });

    //done函数传入0
    stubDialog(
    [ & ]() {
        QMetaObject::invokeMethod(&exportDialog, "execFor", Qt::QueuedConnection, Q_ARG(Page *, c));
    },
    [ = ]() {
        CExportImageDialog *dialog = qobject_cast<CExportImageDialog *>(qApp->activeModalWidget());
        QMetaObject::invokeMethod(dialog, "done", Q_ARG(int, 0));
    });

    ASSERT_EQ(exportDialog.getImageType(), CExportImageDialog::JPG);

    //slots
    //slotOnSavePathChange
    exportDialog.slotOnSavePathChange(CExportImageDialog::Documents);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Downloads);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Desktop);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Music);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Other);
    exportDialog.slotOnSavePathChange(CExportImageDialog::Videos);
    exportDialog.slotOnSavePathChange(99999);

    //slotOnFormatChange
    exportDialog.slotOnFormatChange(CExportImageDialog::PDF);

    ASSERT_EQ(exportDialog.getImageFormate(), QObject::tr("pdf"));

    //slotOnQualityChanged
    exportDialog.slotOnQualityChanged(1);
    ASSERT_EQ(exportDialog.getQuality(), 1);

    //get result
    exportDialog.resultFile();
    exportDialog.desImageSize();

    //other functions
    exportDialog._pPrivate->resetImageSettingSizeTo({1920, 1080});
    exportDialog._pPrivate->showTip(CExportImageDialog::CExportImageDialog_private::ETooBig);
    exportDialog._pPrivate->showTip(CExportImageDialog::CExportImageDialog_private::ETooSmall);
    exportDialog._pPrivate->showTip(CExportImageDialog::CExportImageDialog_private::ENoAlert);
    ASSERT_EQ(exportDialog._pPrivate->isFocusInEditor(), false);
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
