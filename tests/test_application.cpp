#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "application.h"
#include <QTimer>

#include "mainwindow.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"

TEST(isFileNameLegal, ins1)
{
    ASSERT_EQ(false, drawApp->isFileNameLegal("/sadhgasdjhasdg/sahdkjahskdjhaskd"));
}

TEST(isFileNameLegal, ins2)
{
    ASSERT_EQ(false, drawApp->isFileNameLegal("./"));
}

TEST(isFileNameLegal, ins3)
{
    ASSERT_EQ(true, drawApp->isFileNameLegal("./myfile.txt"));
}

TEST(isFileNameLegal, ins4)
{
    ASSERT_EQ(false, drawApp->isFileNameLegal(""));
}
