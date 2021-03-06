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
#include "application.h"
#include "mainwindow.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#ifdef ENABLE_FSANITIZE
#include <sanitizer/asan_interface.h>
#endif



#define QMYTEST_MAIN(TestObject) \
    QT_BEGIN_NAMESPACE \
    QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
    QT_END_NAMESPACE \
    int main(int argc, char *argv[]) \
    { \
        qputenv("QTEST_FUNCTION_TIMEOUT", "1000000");\
        Application app(argc, argv); \
        QTEST_DISABLE_KEYPAD_NAVIGATION \
        QTEST_ADD_GPU_BLACKLIST_SUPPORT \
        TestObject tc; \
        QTEST_SET_MAIN_SOURCE_PATH \
        return QTest::qExec(&tc, argc, argv); \
    }

class QTestMain : public QObject
{
    Q_OBJECT

public:
    QTestMain();
    ~QTestMain();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testGTest();
};

QTestMain::QTestMain()
{

}

QTestMain::~QTestMain()
{
    delete  drawApp->topMainWindow();
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

QMYTEST_MAIN(QTestMain)

#include "test_qtestmain.moc"
