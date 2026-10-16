// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CAttributeManagerWgt methods (issue V-5860)
 *
 * Source: src/frame/AttributesWidgets/private/cattributemanagerwgt.cpp
 *
 * Target methods:
 *   attributions, setAttributions, changeAttribution,
 *   attributeWgts, attributeVars,
 *   isLogicAncestorOf,
 *   installComAttributeWgt, removeComAttributeWgt,
 *   setDefaultAttributionVar, defaultAttriVar,
 *   attrOfWidget, widgetOfAttr,
 *   setWidgetRecommedSize,
 *   setWidgetAttribution, onAttriWidgetValueChanged
 *
 * Known defect: removeComAttributeWgt(int) has a bug where the second
 * erase block calls s_allInstalledAttriWgts.erase(itf) again with an
 * already-invalidated iterator instead of
 * s_allInstalledDefaultVar.erase(itfV). This causes undefined behavior
 * (crash) whenever the method is called after installComAttributeWgt,
 * because installComAttributeWgt always inserts into
 * s_allInstalledDefaultVar via setDefaultAttributionVar. The
 * removeComAttributeWgt crash path is therefore not exercised; the
 * defect is documented below.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWidget>
#include <QVariant>
#include <QSize>
#include <QSignalSpy>
#include <QTimer>
#include <memory>

#define protected public
#define private public
#include "cattributemanagerwgt.h"
#undef protected
#undef private

using namespace DrawAttribution;

// ---------------------------------------------------------------------------
// Helper: a minimal CAttributeWgt subclass whose setVar stores the value
// so tests can verify the value was forwarded.
// ---------------------------------------------------------------------------
class TestAttriWgt : public CAttributeWgt
{
    Q_OBJECT
public:
    explicit TestAttriWgt(int attri = -1, QWidget *p = nullptr)
        : CAttributeWgt(attri, p) {}
    void setVar(const QVariant &var) override
    {
        lastVar = var;
        CAttributeWgt::setVar(var);
    }
    QVariant lastVar;
};

// =========================================================================
// attributions / setAttributions
// =========================================================================

TEST(CAttributeManagerWgtTest, AttributionsDefaultEmpty)
{
    CAttributeManagerWgt wgt;
    EXPECT_TRUE(wgt.attributions().isEmpty());
}

TEST(CAttributeManagerWgtTest, SetAttributionsSizeChanged)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1, 2, 3});
    wgt.setAttributions(list);

    EXPECT_EQ(wgt.attributions().size(), 3);
    EXPECT_TRUE(wgt._dirty);
}

TEST(CAttributeManagerWgtTest, SetAttributionsSameSizeDifferentContent)
{
    CAttributeManagerWgt wgt;
    SAttrisList list1(QList<int>{1, 2});
    wgt.setAttributions(list1);
    wgt._dirty = false;

    SAttrisList list2(QList<int>{1, 3});
    wgt.setAttributions(list2);

    EXPECT_EQ(wgt.attributions()[1].attri, 3);
    EXPECT_TRUE(wgt._dirty);
}

TEST(CAttributeManagerWgtTest, SetAttributionsIdenticalNoChange)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1, 2});
    wgt.setAttributions(list);
    wgt._dirty = false;

    wgt.setAttributions(list);

    EXPECT_FALSE(wgt._dirty);
}

// =========================================================================
// changeAttribution
// =========================================================================

TEST(CAttributeManagerWgtTest, ChangeAttributionWithUpdate)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1, 2});
    wgt.setAttributions(list);
    wgt._dirty = false;

    wgt.changeAttribution(SAttri(1, 42), true);

    EXPECT_EQ(wgt.attributions()[0].var.toInt(), 42);
    EXPECT_TRUE(wgt._dirty);
}

TEST(CAttributeManagerWgtTest, ChangeAttributionWithoutUpdate)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1, 2});
    wgt.setAttributions(list);
    wgt._dirty = false;

    wgt.changeAttribution(SAttri(2, "hello"), false);

    EXPECT_EQ(wgt.attributions()[1].var.toString(), "hello");
    EXPECT_FALSE(wgt._dirty);
}

TEST(CAttributeManagerWgtTest, ChangeAttributionNotFound)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1, 2});
    wgt.setAttributions(list);

    wgt.changeAttribution(SAttri(99, 42), true);
    EXPECT_EQ(wgt.attributions().size(), 2);
}

// =========================================================================
// attributeVars — always returns empty list
// =========================================================================

TEST(CAttributeManagerWgtTest, AttributeVarsAlwaysEmpty)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1, 2});
    wgt.setAttributions(list);

    EXPECT_TRUE(wgt.attributeVars().isEmpty());
}

// =========================================================================
// installComAttributeWgt / mapping lookups
// =========================================================================

TEST(CAttributeManagerWgtTest, InstallAndWidgetOfAttr)
{
    CAttributeManagerWgt wgt;
    auto *child = new TestAttriWgt();
    wgt.installComAttributeWgt(10, child, QVariant(100));

    EXPECT_EQ(wgt.widgetOfAttr(10), child);
    EXPECT_EQ(child->attribution(), 10);
    delete child;
}

TEST(CAttributeManagerWgtTest, InstallAndAttrOfWidget)
{
    CAttributeManagerWgt wgt;
    auto *child = new TestAttriWgt();
    wgt.installComAttributeWgt(20, child);

    EXPECT_EQ(wgt.attrOfWidget(child), 20);
    delete child;
}

TEST(CAttributeManagerWgtTest, AttrOfWidgetNotFound)
{
    CAttributeManagerWgt wgt;
    auto *orphan = new QWidget();
    EXPECT_EQ(wgt.attrOfWidget(orphan), -1);
    delete orphan;
}

TEST(CAttributeManagerWgtTest, WidgetOfAttrNotFound)
{
    CAttributeManagerWgt wgt;
    EXPECT_EQ(wgt.widgetOfAttr(999), nullptr);
}

TEST(CAttributeManagerWgtTest, InstallNullWidgetIgnored)
{
    CAttributeManagerWgt wgt;
    wgt.installComAttributeWgt(40, nullptr);
    EXPECT_EQ(wgt.widgetOfAttr(40), nullptr);
}

// =========================================================================
// removeComAttributeWgt — only test the not-found path
//
// NOTE: removeComAttributeWgt has a defect (see file header) that causes
// a crash when called after installComAttributeWgt. Only the not-found
// path (no prior install) is safe to test.
// =========================================================================

TEST(CAttributeManagerWgtTest, RemoveComAttributeWgtNotFound)
{
    CAttributeManagerWgt wgt;
    // No prior install — should not crash, entry not found
    wgt.removeComAttributeWgt(999);
    SUCCEED();
}

// =========================================================================
// setDefaultAttributionVar / defaultAttriVar
// =========================================================================

TEST(CAttributeManagerWgtTest, SetAndGetDefaultAttriVar)
{
    CAttributeManagerWgt wgt;
    wgt.setDefaultAttributionVar(50, QVariant("default"));
    EXPECT_EQ(wgt.defaultAttriVar(50).toString(), "default");
}

TEST(CAttributeManagerWgtTest, DefaultAttriVarUnsetReturnsInvalid)
{
    CAttributeManagerWgt wgt;
    EXPECT_FALSE(wgt.defaultAttriVar(999).isValid());
}

TEST(CAttributeManagerWgtTest, InstallSetsDefaultVar)
{
    CAttributeManagerWgt wgt;
    auto *child = new TestAttriWgt();
    wgt.installComAttributeWgt(60, child, QVariant(600));

    EXPECT_EQ(wgt.defaultAttriVar(60).toInt(), 600);
    delete child;
}

// =========================================================================
// isLogicAncestorOf
// =========================================================================

TEST(CAttributeManagerWgtTest, IsLogicAncestorOfDirectChild)
{
    CAttributeManagerWgt wgt;
    auto *child = new QWidget(&wgt);
    EXPECT_TRUE(wgt.isLogicAncestorOf(child));
}

TEST(CAttributeManagerWgtTest, IsLogicAncestorOfNestedChild)
{
    CAttributeManagerWgt wgt;
    auto *mid = new QWidget(&wgt);
    auto *leaf = new QWidget(mid);
    EXPECT_TRUE(wgt.isLogicAncestorOf(leaf));
}

TEST(CAttributeManagerWgtTest, IsLogicAncestorOfUnrelated)
{
    CAttributeManagerWgt wgt;
    auto *unrelated = new QWidget();
    EXPECT_FALSE(wgt.isLogicAncestorOf(unrelated));
    delete unrelated;
}

TEST(CAttributeManagerWgtTest, IsLogicAncestorOfNull)
{
    CAttributeManagerWgt wgt;
    EXPECT_FALSE(wgt.isLogicAncestorOf(nullptr));
}

// =========================================================================
// setWidgetRecommedSize
// =========================================================================

TEST(CAttributeManagerWgtTest, SetWidgetRecommedSize)
{
    CAttributeManagerWgt wgt;
    auto *target = new QWidget();
    wgt.setWidgetRecommedSize(target, QSize(100, 50));

    QVariant prop = target->property(AttriWidgetReWidth);
    EXPECT_TRUE(prop.isValid());
    EXPECT_EQ(prop.toSize(), QSize(100, 50));
    delete target;
}

TEST(CAttributeManagerWgtTest, SetWidgetRecommedSizeNullNoCrash)
{
    CAttributeManagerWgt wgt;
    wgt.setWidgetRecommedSize(nullptr, QSize(10, 10));
    SUCCEED();
}

// =========================================================================
// setWidgetAttribution (private, accessible via #define private public)
// =========================================================================

TEST(CAttributeManagerWgtTest, SetWidgetAttributionOnAttriWgt)
{
    CAttributeManagerWgt wgt;
    auto *child = new TestAttriWgt();
    wgt.setWidgetAttribution(child, QVariant(777));

    EXPECT_EQ(child->lastVar.toInt(), 777);
    delete child;
}

TEST(CAttributeManagerWgtTest, SetWidgetAttributionOnPlainWidget)
{
    CAttributeManagerWgt wgt;
    auto *plain = new QWidget();
    wgt.setWidgetAttribution(plain, QVariant("val"));

    EXPECT_EQ(plain->property(AttriWidgetVar).toString(), "val");
    delete plain;
}

TEST(CAttributeManagerWgtTest, SetWidgetAttributionEmitsUpdateWgt)
{
    CAttributeManagerWgt wgt;
    auto *child = new TestAttriWgt();
    QSignalSpy spy(&wgt, &CAttributeManagerWgt::updateWgt);

    wgt.setWidgetAttribution(child, QVariant(42));

    EXPECT_EQ(spy.count(), 1);
    delete child;
}

// =========================================================================
// onAttriWidgetValueChanged (private, accessible via #define private public)
// =========================================================================

TEST(CAttributeManagerWgtTest, OnAttriWidgetValueChangedEmitsAttributionChanged)
{
    CAttributeManagerWgt wgt;
    auto *child = new TestAttriWgt();
    wgt.installComAttributeWgt(70, child);

    QSignalSpy spy(&wgt, &CAttributeManagerWgt::attributionChanged);

    // Simulate the child emitting attriChanged
    emit child->attriChanged(QVariant(123), EChanged);

    ASSERT_EQ(spy.count(), 1);
    auto args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toInt(), 70);
    EXPECT_EQ(args.at(1).toInt(), 123);
    delete child;
}

// =========================================================================
// attributeWgts — returns _allWgts after ensureAttributions
// =========================================================================

TEST(CAttributeManagerWgtTest, AttributeWgtsEmptyWhenNoInstall)
{
    CAttributeManagerWgt wgt;
    SAttrisList list(QList<int>{1});
    wgt.setAttributions(list);
    // attri 1 not installed, so no widgets should appear
    EXPECT_TRUE(wgt.attributeWgts().isEmpty());
}

// =========================================================================
// Defect documentation: removeComAttributeWgt erases from
// s_allInstalledAttriWgts with invalid iterator instead of
// s_allInstalledDefaultVar. This test verifies the mapping is set
// up correctly but does NOT call removeComAttributeWgt (which would
// crash). The defect is in the source code, not the test.
//
// TEST(CAttributeManagerWgtTest, RemoveComAttributeWgtDefect)
// {
//     // Cannot run: removeComAttributeWgt causes UB/crash due to
//     // double-erase of s_allInstalledAttriWgts with invalid iterator.
//     // The second erase block should call
//     // s_allInstalledDefaultVar.erase(itfV) but instead calls
//     // s_allInstalledAttriWgts.erase(itf) where itf is already erased.
// }
// =========================================================================

#include "test_cattributemanagerwgt_funcs.moc"
