// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSvgLabel (issue V-6403).
 *
 * Source: src/widgets/csvglabel.cpp
 *
 * Target methods:
 *   CSvgLabel(QString, QWidget*), paintEvent(QPaintEvent*)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QTemporaryDir>
#include <QFile>

#define protected public
#define private public
#include "widgets/csvglabel.h"
#undef protected
#undef private

// ---- CSvgLabel constructors ----

TEST(CSvgLabelTest, DefaultConstructorCreatesValidObject)
{
    CSvgLabel label;
    EXPECT_TRUE(label.text().isEmpty());
}

TEST(CSvgLabelTest, ConstructorWithText)
{
    // The str parameter is treated as an SVG file path; write a real
    // SVG file so the constructor's load can be verified.
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString svgPath = tempDir.filePath("test.svg");
    QFile svgFile(svgPath);
    ASSERT_TRUE(svgFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    svgFile.write("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"10\" height=\"10\"><rect width=\"10\" height=\"10\" fill=\"red\"/></svg>");
    svgFile.close();

    CSvgLabel label(svgPath);
    EXPECT_TRUE(label.m_svgRender.isValid());
}

TEST(CSvgLabelTest, ConstructorWithParent)
{
    QWidget parent;
    CSvgLabel label(QString(), &parent);
    EXPECT_EQ(label.parent(), &parent);
}

// ---- paintEvent ----

TEST(CSvgLabelPaintEvent, EmptySvgRenderDoesNotCrash)
{
    CSvgLabel label;
    label.resize(100, 100);
    label.show();

    // grab() triggers paintEvent on the widget.
    QPixmap pix = label.grab();
    EXPECT_FALSE(pix.isNull());
}

TEST(CSvgLabelPaintEvent, ValidSvgContentRenders)
{
    // A minimal valid SVG document.
    static const char svgData[] =
        "<svg xmlns=\"http://www.w3.org/2000/svg\""
        " width=\"50\" height=\"50\">"
        "<rect width=\"50\" height=\"50\" fill=\"red\"/>"
        "</svg>";

    CSvgLabel label;
    label.m_svgRender.load(QByteArray(svgData));
    ASSERT_TRUE(label.m_svgRender.isValid());

    label.resize(60, 60);
    label.show();

    QPixmap pix = label.grab();
    EXPECT_FALSE(pix.isNull());
    // paintEvent renders the SVG into the whole widget, so the red rect
    // must show up in the captured image.
    const QImage image = pix.toImage();
    ASSERT_FALSE(image.isNull());
    EXPECT_EQ(image.pixelColor(image.width() / 2, image.height() / 2).rgba(), QColor(Qt::red).rgba());
}
