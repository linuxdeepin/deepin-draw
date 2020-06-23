#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "application.h"

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
    ASSERT_EQ(false, dApp->isFileNameLegal(""));
}
