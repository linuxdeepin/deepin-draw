/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zhanghao<zhanghao@uniontech.com>
* Maintainer: zhanghao<zhanghao@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtTest>
#include <QCoreApplication>
#include "application.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define QMYTEST_MAIN(TestObject) \
    QT_BEGIN_NAMESPACE \
    QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
    QT_END_NAMESPACE \
    int main(int argc, char *argv[]) \
    { \
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
    testing::InitGoogleTest();
    int ret = RUN_ALL_TESTS();
    // __sanitizer_set_report_path("./asan.log");
    Q_UNUSED(ret)
}

QMYTEST_MAIN(QTestMain)

#include "test_qtestmain.moc"
