#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "application.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    int ret = RUN_ALL_TESTS();

    Q_UNUSED(ret)

    a.exec();
}
