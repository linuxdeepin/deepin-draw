#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "application.h"
#include <QTimer>

TEST(isFileNameLegal, ins1)
{
    ASSERT_EQ(false, dApp->isFileNameLegal("/sadhgasdjhasdg/sahdkjahskdjhaskd"));
}
TEST(isFileNameLegal, ins2)
{
    ASSERT_EQ(false, dApp->isFileNameLegal("./"));
}
TEST(isFileNameLegal, ins3)
{
    ASSERT_EQ(true, dApp->isFileNameLegal("./myfile.txt"));
}
TEST(isFileNameLegal, ins4)
{
    ASSERT_EQ(false, dApp->isFileNameLegal(""));
}
TEST(execDraw, ins5)
{
    QStringList li;
    li.append(":/test.ddf");
    li.append(":/test.png");
    QString path = "";
    QTimer::singleShot(500, [ = ] {
        dApp->quit();
    });
    ASSERT_EQ(0, dApp->execDraw(li, path));
}
