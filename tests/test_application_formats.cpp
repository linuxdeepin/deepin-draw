// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QStringList>

#define protected public
#define private public
#include "application.h"
#undef protected
#undef private

TEST(ApplicationFormats, readableFormatsNonEmpty)
{
    QStringList formats = drawApp->readableFormats();
    EXPECT_FALSE(formats.isEmpty());
}

TEST(ApplicationFormats, readableFormatsContainsPng)
{
    QStringList formats = drawApp->readableFormats();
    EXPECT_TRUE(formats.contains("png"));
}

TEST(ApplicationFormats, readableFormatsContainsDdf)
{
    QStringList formats = drawApp->readableFormats();
    EXPECT_TRUE(formats.contains("ddf"));
}

TEST(ApplicationFormats, readableFormatNameFiltersNonEmpty)
{
    QStringList filters = drawApp->readableFormatNameFilters();
    EXPECT_FALSE(filters.isEmpty());
}

TEST(ApplicationFormats, readableFormatNameFiltersContainsPng)
{
    QStringList filters = drawApp->readableFormatNameFilters();
    bool found = false;
    for (const QString &f : filters) {
        if (f.contains("*.png"))
            found = true;
    }
    EXPECT_TRUE(found);
}

TEST(ApplicationFormats, writableFormatsNonEmpty)
{
    QStringList formats = drawApp->writableFormats();
    EXPECT_FALSE(formats.isEmpty());
}

TEST(ApplicationFormats, writableFormatsContainsPng)
{
    QStringList formats = drawApp->writableFormats();
    EXPECT_TRUE(formats.contains("png"));
}

TEST(ApplicationFormats, writableFormatsContainsPdf)
{
    QStringList formats = drawApp->writableFormats();
    EXPECT_TRUE(formats.contains("pdf"));
}

TEST(ApplicationFormats, writableFormatsNotContainsPdfInReadable)
{
    QStringList readable = drawApp->readableFormats();
    EXPECT_FALSE(readable.contains("pdf"));
}

TEST(ApplicationFormats, writableFormatNameFiltersNonEmpty)
{
    QStringList filters = drawApp->writableFormatNameFilters();
    EXPECT_FALSE(filters.isEmpty());
}

TEST(ApplicationFormats, writableFormatNameFiltersContainsPdf)
{
    QStringList filters = drawApp->writableFormatNameFilters();
    bool found = false;
    for (const QString &f : filters) {
        if (f.contains("*.pdf"))
            found = true;
    }
    EXPECT_TRUE(found);
}

TEST(ApplicationFormats, writableFormatNameFiltersStructConsistent)
{
    QStringList filters = drawApp->writableFormatNameFilters();
    QStringList formats = drawApp->writableFormats();
    QStringList extractedExts;
    for (const QString &f : filters) {
        int start = f.indexOf("(*.");
        if (start >= 0) {
            int end = f.indexOf(")", start);
            if (end > start) {
                QString extPart = f.mid(start + 2, end - start - 2);
                QStringList exts = extPart.split(" ");
                for (const QString &ext : exts) {
                    QString cleanExt = ext;
                    cleanExt.remove("*").remove(".");
                    if (!cleanExt.isEmpty())
                        extractedExts << cleanExt;
                }
            }
        }
    }
    for (const QString &ext : extractedExts) {
        EXPECT_TRUE(formats.contains(ext))
            << "Extension " << ext.toStdString()
            << " from filter not in writableFormats";
    }
}
