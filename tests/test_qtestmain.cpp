// SPDX-FileCopyrightText: 2020-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Pre-include standard headers before #define private to avoid
// breaking C++ standard library internals (e.g. std::basic_stringbuf).
#include <sstream>

#define private protected
#include <QtTest>
#undef private

#include <QCoreApplication>
#include <qglobal.h>
#define private public
#include "application.h"
#include "mainwindow.h"
#include "ccentralwidget.h"
#undef private

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#ifdef ENABLE_FSANITIZE
#include <sanitizer/asan_interface.h>
#endif

static int g_argc = 0;
static char **g_argv = nullptr;

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
    Q_INIT_RESOURCE(frameRes);
    Q_INIT_RESOURCE(images);
    Q_INIT_RESOURCE(cursorIcons);
    Q_INIT_RESOURCE(widgetsRes);
#endif
}
#if TEST_OFFSCREENT
//qputenv("QT_QPA_PLATFORM","offscreen");

#endif

#define QMYTEST_MAIN(TestObject) \
    QT_BEGIN_NAMESPACE \
    QT_END_NAMESPACE \
    int main(int argc, char *argv[]) \
    { \
        g_argc = argc; \
        g_argv = argv; \
        initEnv();\
        initQrcIfStaticLib();\
        Application app(argc, argv); \
        QTEST_DISABLE_KEYPAD_NAVIGATION \
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
    drawApp->topMainWindow()->drawBoard()->initTools();

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
    // Allow filtering via GTEST_FILTER env var (QTest intercepts --gtest_filter).
    QByteArray gtestFilter = qgetenv("GTEST_FILTER");
    if (!gtestFilter.isEmpty()) {
        testing::GTEST_FLAG(filter) = gtestFilter.toStdString();
    }
    // Pass the original command-line arguments so --gtest_filter works.
    int argc = g_argc;
    char **argv = g_argv;
    if (argc < 1 || argv == nullptr) {
        argc = 1;
        const auto arg0 = "dummy";
        static char *argv0 = const_cast<char *>(arg0);
        argv = &argv0;
    }
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
