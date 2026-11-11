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
#include <QStackedWidget>
#include <qglobal.h>
#define private public
#include "application.h"
#include "mainwindow.h"
#include "ccentralwidget.h"
#undef private
#include "cmultiptabbarwidget.h"

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

// Most suites obtain their canvas via createNewViewByShortcutKey(), which
// simulates Ctrl+N: every call adds a page/view and nothing ever closed the
// previous ones. Over a full-suite run that accumulates ~2400 live pages and
// pushes RSS past the session cgroup limit (user-1000.slice memory.max),
// where the kernel SIGKILLs the process silently — dropping the remaining
// tests and all buffered gcov data (no .gcda files, empty coverage report).
// This listener bounds the page count after each test. It deliberately closes
// only ONE page (the oldest non-current one) and never pumps deferred
// deletions itself: force-closing many pages in one go and draining their
// deleteLater queue re-entrantly, while the board is still inside the close
// handling, leaves dangling page/view references in board state (observed as
// a freed-scene SEGV in TextItem.TestLayerChange). Deferred deletions are
// processed by the normal event loop during the next test's qWait instead.
class PageJanitorListener : public ::testing::EmptyTestEventListener
{
public:
    static TabBarWgt *tabBarOf(DrawBoard *board)
    {
        const QList<TabBarWgt *> tabs = board->findChildren<TabBarWgt *>();
        for (TabBarWgt *t : tabs) {
            if (t->parentWidget() == board) {
                return t;
            }
        }
        return nullptr;
    }

    static QStackedWidget *stackOf(DrawBoard *board)
    {
        const QList<QStackedWidget *> stacks = board->findChildren<QStackedWidget *>();
        for (QStackedWidget *s : stacks) {
            if (s->parentWidget() == board) {
                return s;
            }
        }
        return nullptr;
    }

    static Page *stackPageAt(QStackedWidget *stack, int i)
    {
        return qobject_cast<Page *>(stack->widget(i));
    }

    static QStringList stackKeys(QStackedWidget *stack)
    {
        QStringList keys;
        for (int i = 0; i < stack->count(); ++i) {
            Page *pg = stackPageAt(stack, i);
            keys << (pg != nullptr ? pg->key() : QStringLiteral("<non-page>"));
        }
        return keys;
    }

    static QStringList tabKeys(TabBarWgt *tabs)
    {
        QStringList keys;
        for (int i = 0; i < tabs->count(); ++i) {
            keys << tabs->key(i);
        }
        return keys;
    }

    static bool isSynced(QStackedWidget *stack, TabBarWgt *tabs)
    {
        if (tabs->count() != stack->count()) {
            return false;
        }
        return stackKeys(stack) == tabKeys(tabs);
    }

    // Repairs both drift directions between the board's two parallel page
    // lists: removes tab items whose key no live stack page carries (stale
    // tab), and force-closes stack pages that have no tab item (orphans —
    // closePageHelper's removeWidget() drops them from the stack while
    // removeItem() on an untabbed key is a harmless no-op). Returns true
    // when the two lists are consistent again.
    static bool repair(TabBarWgt *tabs, QStackedWidget *stack)
    {
        const QStringList liveKeys = stackKeys(stack);
        for (const QString &k : tabKeys(tabs)) {
            if (!liveKeys.contains(k)) {
                qWarning() << "PageJanitor: removing stale tab item" << k;
                tabs->removeItem(k);
            }
        }
        for (int i = liveKeys.size() - 1; i >= 0; --i) {
            if (liveKeys.at(i) == QStringLiteral("<non-page>")) {
                continue;
            }
            if (tabs->index(liveKeys.at(i)) == -1) {
                Page *pg = stackPageAt(stack, i);
                if (pg != nullptr && pg != qobject_cast<Page *>(stack->currentWidget())) {
                    qWarning() << "PageJanitor: closing orphan stack page" << liveKeys.at(i);
                    pg->close(true);
                }
            }
        }
        return isSynced(stack, tabs);
    }

    void reportDesync(const char *phase, const QString &closedKey,
                      const ::testing::TestInfo &info,
                      QStackedWidget *stack, TabBarWgt *tabs)
    {
        qWarning() << "PageJanitor:" << phase << "after test"
                   << info.test_suite_name() << info.name()
                   << "closedKey =" << closedKey
                   << "stack =" << stackKeys(stack)
                   << "tabs =" << tabKeys(tabs);
    }

    void OnTestEnd(const ::testing::TestInfo &info) override
    {
        DrawBoard *board = drawApp->topMainWindow()->drawBoard();
        if (board == nullptr) {
            return;
        }
        TabBarWgt *tabs = tabBarOf(board);
        QStackedWidget *stack = stackOf(board);
        if (tabs == nullptr || stack == nullptr) {
            return;
        }

        if (!isSynced(stack, tabs)) {
            reportDesync("tab/stack desync BEFORE close", QString(), info, stack, tabs);
            if (!repair(tabs, stack)) {
                m_disabled = true;
                qWarning() << "PageJanitor: repair failed, janitor disabled";
            }
            return;
        }

        if (m_disabled || board->count() <= 4) {
            return;
        }

        Page *cur = board->currentPage();
        for (int i = 0; i < stack->count(); ++i) {
            Page *pg = stackPageAt(stack, i);
            if (pg != nullptr && pg != cur) {
                const QString closedKey = pg->key();
                pg->close(true);   // force: no save prompt, no modal
                if (!isSynced(stack, tabs)) {
                    reportDesync("desync AFTER close", closedKey, info, stack, tabs);
                    if (!repair(tabs, stack)) {
                        m_disabled = true;
                        qWarning() << "PageJanitor: repair failed, janitor disabled";
                    }
                }
                break;
            }
        }
    }

private:
    bool m_disabled = false;
};

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
    // Intentionally do not delete the MainWindow. After a full-suite run the
    // widget tree can contain stale references (the teardown used to SEGV in
    // TabBarWgt/DTabBar child destruction), and the process still holds it
    // reachable via the drawApp singleton, so LeakSanitizer stays quiet.
    // Skipping the deletion lets the process exit cleanly so gcov writes its
    // .gcda data and coverage collection produces a full report.
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
    testing::UnitTest::GetInstance()->listeners().Append(new PageJanitorListener);

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
