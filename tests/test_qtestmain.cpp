/*
 *  Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao <zhanghao@uniontech.com>
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
#define private protected
#include <QtTest>
#undef private

#include <QCoreApplication>
#include <qglobal.h>
#define private public
#include "application.h"
#include "mainwindow.h"
#include "drawboard.h"
#undef private

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#ifdef ENABLE_FSANITIZE
#include <sanitizer/asan_interface.h>
#endif

static void initEnv()
{
#if TEST_OFFSCREENT
    qputenv("QT_QPA_PLATFORM", "offscreen");
#endif
    qputenv("QTEST_FUNCTION_TIMEOUT", "3000000");
}
static void initQrcIfStaticLib()
{
#if defined(STATIC_LIB)
    DWIDGET_INIT_RESOURCE();
#endif

#ifdef LINK_DRAWBASELIB_STATIC
    Q_INIT_RESOURCE(drawBaseRes);

    Q_INIT_RESOURCE(images);
    Q_INIT_RESOURCE(cursorIcons);
    Q_INIT_RESOURCE(widgetsRes);
#endif
    //Q_INIT_RESOURCE(frameRes);
}
#if TEST_OFFSCREENT
//qputenv("QT_QPA_PLATFORM","offscreen");

#endif

#define QMYTEST_MAIN(TestObject) \
    QT_BEGIN_NAMESPACE \
    QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
    QT_END_NAMESPACE \
    int main(int argc, char *argv[]) \
    { \
        initEnv();\
        initQrcIfStaticLib();\
        Application app(argc, argv); \
        QTEST_DISABLE_KEYPAD_NAVIGATION \
        QTEST_ADD_GPU_BLACKLIST_SUPPORT \
        TestObject tc; \
        QTEST_SET_MAIN_SOURCE_PATH \
        return QTest::qExec(&tc, argc, argv); \
    }
int quitResult = -1;
QList<int> quitResults;
class QTestMain : public QObject
{
    Q_OBJECT

public:
    QTestMain();
    ~QTestMain();

    bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testGTest();
    void autoQuitActivedModalWidget();
};

QTestMain::QTestMain()
{
    drawApp->showMainWindow(QStringList());
    drawApp->topMainWindow()->showMaximized();
    drawApp->topMainWindow()->drawBoard();//->initTools();

    qApp->installEventFilter(this);
}

QTestMain::~QTestMain()
{
    delete  drawApp->topMainWindow();
}

bool QTestMain::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::WindowBlocked) {
        if (qApp->activeModalWidget() != nullptr) {
            QMetaObject::invokeMethod(this, &QTestMain::autoQuitActivedModalWidget, Qt::QueuedConnection);
        }
    }
    return QObject::eventFilter(o, e);
}

void QTestMain::initTestCase()
{
}

void QTestMain::cleanupTestCase()
{
}

void QTestMain::testGTest()
{
    testing::GTEST_FLAG(output) = "xml:./report/report_deepin-draw.xml";
    int argc = 1;
    const auto arg0 = "dummy";
    char *argv0 = const_cast<char *>(arg0);
    char **argv = &argv0;
    testing::InitGoogleTest(&argc, argv);

#ifdef ENABLE_FSANITIZE
    __sanitizer_set_report_path("./asan_deepin-draw.log");
#endif
    int ret = RUN_ALL_TESTS();
    Q_UNUSED(ret)
}

void QTestMain::autoQuitActivedModalWidget()
{
    QTimer::singleShot(300, this, [ = ]() {
        if (qApp->activeModalWidget() != nullptr) {
            DDialog *dialog = qobject_cast<DDialog *>(qApp->activeModalWidget());
            if (dialog != nullptr) {
                int quitValue = quitResult;
                if (!quitResults.isEmpty()) {
                    quitValue = quitResults.takeFirst();
                }
                qWarning() << "quitResult ============ " << quitValue << "dialog = " << dialog;
                dialog->done(quitValue);
            } else {
                qobject_cast<QWidget *>(qApp->activeModalWidget())->close();
            }
        }
    });
}

QMYTEST_MAIN(QTestMain)

#include "test_qtestmain.moc"
