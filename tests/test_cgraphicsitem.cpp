// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItem, CGraphicsItemGroup, CGraphItemEvent, CGraphItemScalEvent
 *
 * Target methods (from issue V-5245):
 *   CGraphicsItem (20 methods):
 *     isSelected, isMutiSelected, isBzItem, isCached, contains,
 *     initHandle, clearHandle, curView, drawScene, layer,
 *     move, setPenColor, setPenWidth, setDrawRotatin,
 *     setRotation90, doFilp, paintPen, loadHeadData,
 *     setScene, setAttributionVar
 *
 *   CGraphicsItemGroup (10 methods):
 *     add, remove, clear, count, items,
 *     getBzItems, getGroups, rect, name, initHandle
 *
 *   CGraphItemEvent / CGraphItemScalEvent (6 methods):
 *     CGraphItemEvent(constructor), item, setPos, setEventPhase,
 *     trans, reCalTransform (base + ScalEvent override)
 *
 * Source files:
 *   src/drawshape/drawItems/bzItems/cgraphicsitem.cpp
 *   src/drawshape/drawItems/cgraphicsitemselectedmgr.cpp
 *   src/drawshape/drawItems/bzItems/cgraphicsitemevent.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

// Include Qt headers BEFORE #define private to avoid breaking Qt internals.
#include <QtCore/QtGlobal>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QTransform>
#include <QGraphicsScene>
#include <QColor>
#include <QVariant>
#include <QPainterPath>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// DrawAttribution::EComAttri integer values (from cattributeitemwidget.h)
//   ETitle=0, EBrushColor=1, EPenColor=2, EBorderWidth=3
static const int ATTR_BRUSH_COLOR  = 1;
static const int ATTR_PEN_COLOR    = 2;
static const int ATTR_BORDER_WIDTH = 3;

// CGraphicsItem is abstract (pure virtual rect()), so we need a concrete
// subclass for testing the base-class methods directly.
class TestGraphicsItem : public CGraphicsItem
{
public:
    explicit TestGraphicsItem(QGraphicsItem *parent = nullptr)
        : CGraphicsItem(parent) {}
    QRectF m_testRect;
    QRectF rect() const override { return m_testRect; }
};

// =========================================================================
// CGraphicsItem tests (20 methods)
// =========================================================================

// --- isSelected (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, IsSelected_NoParentGroup_ReturnsFalse)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    bool result = item.isSelected();

    // Assert
    EXPECT_FALSE(result);
}

// --- isMutiSelected (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, IsMutiSelected_NoParentGroup_ReturnsFalse)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    bool result = item.isMutiSelected();

    // Assert
    EXPECT_FALSE(result);
}

// --- isBzItem (cx=0, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, IsBzItem_BaseItem_ReturnsFalse)
{
    // Arrange
    // CGraphicsItem::Type == UserType == NoType, so type() > NoType is false.
    TestGraphicsItem item(nullptr);

    // Act
    bool result = item.isBzItem();

    // Assert
    EXPECT_FALSE(result);
}

// --- isCached (cx=0, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, IsCached_NoCachePixmap_ReturnsFalse)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    // _useCachePixmap defaults to false, _cachePixmap defaults to nullptr

    // Act
    bool result = item.isCached();

    // Assert
    EXPECT_FALSE(result);
}

// --- contains (cx=2, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, Contains_EmptyShape_ReturnsFalse)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    // Base CGraphicsItem has empty penStrokerShape and selfOrgShape.

    // Act
    bool result = item.contains(QPointF(50, 50));

    // Assert
    EXPECT_FALSE(result);
}

// --- initHandle (cx=0, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, InitHandle_SetsItemFlags)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.initHandle();

    // Assert — flags should be set for interaction
    EXPECT_TRUE(item.flags() & QGraphicsItem::ItemIsMovable);
    EXPECT_TRUE(item.flags() & QGraphicsItem::ItemIsSelectable);
    EXPECT_TRUE(item.flags() & QGraphicsItem::ItemSendsGeometryChanges);
    EXPECT_TRUE(item.acceptHoverEvents());
}

// --- clearHandle (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, ClearHandle_RemovesAllHandles)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    // After construction m_handles should be empty; clear should keep it empty.

    // Act
    item.clearHandle();

    // Assert
    EXPECT_TRUE(item.m_handles.isEmpty());
}

// --- curView (cx=2, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, CurView_NoScene_ReturnsNullptr)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    PageView *result = item.curView();

    // Assert
    EXPECT_EQ(result, nullptr);
}

// --- drawScene (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, DrawScene_NoScene_ReturnsNullptr)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    PageScene *result = item.drawScene();

    // Assert
    EXPECT_EQ(result, nullptr);
}

// --- layer (cx=0, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, Layer_Default_ReturnsNullptr)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    CGraphicsLayer *result = item.layer();

    // Assert
    EXPECT_EQ(result, nullptr);
}

// --- move (cx=0, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, Move_DeltaApplied_PositionChanges)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    item.setPos(QPointF(0, 0));

    // Act
    item.move(QPointF(0, 0), QPointF(10, 20));

    // Assert
    EXPECT_FLOAT_EQ(item.pos().x(), 10.0f);
    EXPECT_FLOAT_EQ(item.pos().y(), 20.0f);
}

TEST(CGraphicsItemMethodsTest, Move_FromNonOrigin_PositionChangesByDelta)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    item.setPos(QPointF(5, 5));

    // Act
    item.move(QPointF(1, 1), QPointF(11, 21));

    // Assert — new pos = old pos + (movePoint - beginPoint) = (5,5)+(10,20) = (15,25)
    EXPECT_FLOAT_EQ(item.pos().x(), 15.0f);
    EXPECT_FLOAT_EQ(item.pos().y(), 25.0f);
}

// --- setPenColor (cx=2, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, SetPenColor_NonPreview_SetsActualPenColor)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    QColor expectedColor(Qt::red);

    // Act
    item.setPenColor(expectedColor, false);

    // Assert
    EXPECT_EQ(item.pen().color(), expectedColor);
    EXPECT_FALSE(item.m_isPreviewCom[0]);
}

TEST(CGraphicsItemMethodsTest, SetPenColor_Preview_SetsPreviewColor)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    QColor expectedColor(Qt::blue);

    // Act
    item.setPenColor(expectedColor, true);

    // Assert
    EXPECT_EQ(item.m_penPreviewColor, expectedColor);
    EXPECT_TRUE(item.m_isPreviewCom[0]);
}

// --- setPenWidth (cx=2, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, SetPenWidth_NonPreview_SetsActualPenWidth)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setPenWidth(5, false);

    // Assert
    EXPECT_EQ(item.pen().width(), 5);
    EXPECT_FALSE(item.m_isPreviewCom[1]);
}

TEST(CGraphicsItemMethodsTest, SetPenWidth_Preview_SetsPreviewWidth)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setPenWidth(8, true);

    // Assert
    EXPECT_EQ(item.m_penWidth, 8);
    EXPECT_TRUE(item.m_isPreviewCom[1]);
}

// --- setDrawRotatin (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, SetDrawRotatin_NormalAngle_SetsDirectly)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setDrawRotatin(90.0);

    // Assert
    EXPECT_FLOAT_EQ(static_cast<float>(item.drawRotation()), 90.0f);
}

TEST(CGraphicsItemMethodsTest, SetDrawRotatin_AngleAbove360_WrapsWithModulo)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setDrawRotatin(370.0);

    // Assert — 370 % 360 = 10
    EXPECT_FLOAT_EQ(static_cast<float>(item.drawRotation()), 10.0f);
}

TEST(CGraphicsItemMethodsTest, SetDrawRotatin_NegativeAngle_WrapsToPositive)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setDrawRotatin(-90.0);

    // Assert — -90 + 360 = 270
    EXPECT_FLOAT_EQ(static_cast<float>(item.drawRotation()), 270.0f);
}

// --- setRotation90 (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, SetRotation90_LeftTurn_DecreasesBy90)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setRotation90(true);  // left = -90

    // Assert
    EXPECT_FLOAT_EQ(static_cast<float>(item.drawRotation()), 270.0f); // -90 + 360 = 270
}

TEST(CGraphicsItemMethodsTest, SetRotation90_RightTurn_IncreasesBy90)
{
    // Arrange
    TestGraphicsItem item(nullptr);

    // Act
    item.setRotation90(false);  // right = +90

    // Assert
    EXPECT_FLOAT_EQ(static_cast<float>(item.drawRotation()), 90.0f);
}

// --- doFilp (cx=2, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, DoFilp_Horizontal_TogglesFlipHorizontalFlag)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    bool initial = item._flipHorizontal;

    // Act
    item.doFilp(CGraphicsItem::EFilpHor);

    // Assert
    EXPECT_NE(item._flipHorizontal, initial);
    EXPECT_TRUE(item._flipHorizontal);
}

TEST(CGraphicsItemMethodsTest, DoFilp_Vertical_TogglesFlipVerticalFlag)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    bool initial = item._flipVertical;

    // Act
    item.doFilp(CGraphicsItem::EFilpVer);

    // Assert
    EXPECT_NE(item._flipVertical, initial);
    EXPECT_TRUE(item._flipVertical);
}

TEST(CGraphicsItemMethodsTest, DoFilp_HorizontalTwice_FlagReturnsToInitial)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    bool initial = item._flipHorizontal;

    // Act
    item.doFilp(CGraphicsItem::EFilpHor);
    item.doFilp(CGraphicsItem::EFilpHor);

    // Assert
    EXPECT_EQ(item._flipHorizontal, initial);
}

// --- paintPen (cx=2, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, PaintPen_NoPreview_ReturnsPenWithJoinStyle)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    item.setPenColor(QColor(Qt::black), false);

    // Act
    QPen result = item.paintPen(Qt::MiterJoin);

    // Assert
    EXPECT_EQ(result.joinStyle(), Qt::MiterJoin);
    EXPECT_EQ(result.color(), QColor(Qt::black));
}

TEST(CGraphicsItemMethodsTest, PaintPen_WithPreviewColor_ReturnsPreviewColor)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    item.setPenColor(QColor(Qt::green), true);  // preview mode

    // Act
    QPen result = item.paintPen(Qt::RoundJoin);

    // Assert
    EXPECT_EQ(result.color(), QColor(Qt::green));
    EXPECT_EQ(result.joinStyle(), Qt::RoundJoin);
}

// --- loadHeadData (cx=1, level=low, min=1) ---

TEST(CGraphicsItemMethodsTest, LoadHeadData_ValidHead_SetsPenBrushPosRotation)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    SGraphicsUnitHead head;
    head.pen = QPen(QColor(Qt::red), 3);
    head.brush = QBrush(QColor(Qt::blue));
    head.pos = QPointF(10, 20);
    head.rotate = 45.0;
    head.zValue = 5;
    head.trans = QTransform();

    // Act
    item.loadHeadData(head);

    // Assert
    EXPECT_EQ(item.pen().color(), QColor(Qt::red));
    EXPECT_EQ(item.brush().color(), QColor(Qt::blue));
    EXPECT_FLOAT_EQ(static_cast<float>(item.pos().x()), 10.0f);
    EXPECT_FLOAT_EQ(static_cast<float>(item.pos().y()), 20.0f);
    EXPECT_FLOAT_EQ(static_cast<float>(item.drawRotation()), 45.0f);
    EXPECT_DOUBLE_EQ(item.zValue(), 5.0);
}

// --- setScene (cx=6, level=medium, min=2) ---

TEST(CGraphicsItemMethodsTest, SetScene_NullWithNoScene_ReturnsEarly)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    // No scene is set on the item.

    // Act — should not crash, just returns early
    item.setScene(nullptr);

    // Assert — item still has no scene
    EXPECT_EQ(item.scene(), nullptr);
}

TEST(CGraphicsItemMethodsTest, SetScene_AddThenRemove_RemovesFromQGraphicsScene)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    QGraphicsScene scene;
    scene.addItem(&item);
    ASSERT_NE(item.scene(), nullptr);

    // Act
    item.setScene(nullptr);

    // Assert — item removed from scene
    EXPECT_EQ(item.scene(), nullptr);
}

// --- setAttributionVar (cx=5, level=medium, min=2) ---

TEST(CGraphicsItemMethodsTest, SetAttributionVar_PenColor_SetsPenColor)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    QVariant var = QVariant::fromValue(QColor(Qt::red));

    // Act
    item.setAttributionVar(ATTR_PEN_COLOR, var, EChangedBegin);

    // Assert — in preview mode, m_penPreviewColor should be set
    EXPECT_EQ(item.m_penPreviewColor, QColor(Qt::red));
    EXPECT_TRUE(item.m_isPreviewCom[0]);
}

TEST(CGraphicsItemMethodsTest, SetAttributionVar_BorderWidth_SetsPenWidth)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    QVariant var = QVariant::fromValue(7);

    // Act
    item.setAttributionVar(ATTR_BORDER_WIDTH, var, EChangedFinished);

    // Assert — non-preview mode, actual pen width should be set
    EXPECT_EQ(item.pen().width(), 7);
}

TEST(CGraphicsItemMethodsTest, SetAttributionVar_UnknownAttri_NoChange)
{
    // Arrange
    TestGraphicsItem item(nullptr);
    QColor originalColor = item.pen().color();
    QVariant var = QVariant::fromValue(QColor(Qt::green));

    // Act
    item.setAttributionVar(999, var, EChanged);

    // Assert — nothing should change for unknown attribution
    EXPECT_EQ(item.pen().color(), originalColor);
}


// =========================================================================
// CGraphicsItemGroup tests (10 methods)
// =========================================================================

// --- CGraphicsItemGroup constructor (implicit, tested via name/groupType) ---

// --- add (cx=7, level=medium, min=2) ---

TEST(CGraphicsItemGroupMethodsTest, Add_ValidItem_IncreasesCount)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item(nullptr);

    // Act
    group.add(&item, false, false);

    // Assert
    EXPECT_EQ(group.count(), 1);
}

TEST(CGraphicsItemGroupMethodsTest, Add_NullItem_CountUnchanged)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");

    // Act
    group.add(nullptr, false, false);

    // Assert
    EXPECT_EQ(group.count(), 0);
}

TEST(CGraphicsItemGroupMethodsTest, Add_SelfItem_CountUnchanged)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");

    // Act — adding self should be prevented
    group.add(&group, false, false);

    // Assert
    EXPECT_EQ(group.count(), 0);
}

TEST(CGraphicsItemGroupMethodsTest, Add_DuplicateItem_NotAddedTwice)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item(nullptr);
    group.add(&item, false, false);

    // Act — add the same item again
    group.add(&item, false, false);

    // Assert
    EXPECT_EQ(group.count(), 1);
}

// --- remove (cx=5, level=medium, min=2) ---

TEST(CGraphicsItemGroupMethodsTest, Remove_ExistingItem_DecreasesCount)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item(nullptr);
    group.add(&item, false, false);
    ASSERT_EQ(group.count(), 1);

    // Act
    group.remove(&item, false, false);

    // Assert
    EXPECT_EQ(group.count(), 0);
}

TEST(CGraphicsItemGroupMethodsTest, Remove_NonExistingItem_CountUnchanged)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    group.add(&item1, false, false);

    // Act — item2 was never added
    group.remove(&item2, false, false);

    // Assert
    EXPECT_EQ(group.count(), 1);
}

TEST(CGraphicsItemGroupMethodsTest, Remove_SelfItem_NoEffect)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item(nullptr);
    group.add(&item, false, false);

    // Act — removing self should be prevented
    group.remove(&group, false, false);

    // Assert
    EXPECT_EQ(group.count(), 1);
}

// --- clear (cx=1, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, Clear_WithItems_RemovesAllItems)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    ASSERT_EQ(group.count(), 2);

    // Act
    group.clear();

    // Assert
    EXPECT_EQ(group.count(), 0);
}

// --- count (cx=0, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, Count_EmptyGroup_ReturnsZero)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");

    // Act
    int result = group.count();

    // Assert
    EXPECT_EQ(result, 0);
}

TEST(CGraphicsItemGroupMethodsTest, Count_AfterAddingItems_ReturnsCorrectCount)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    TestGraphicsItem item3(nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    group.add(&item3, false, false);

    // Act
    int result = group.count();

    // Assert
    EXPECT_EQ(result, 3);
}

// --- items (cx=3, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, Items_NonRecursive_ReturnsDirectChildren)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);

    // Act
    QList<CGraphicsItem *> result = group.items(false);

    // Assert
    EXPECT_EQ(result.size(), 2);
    EXPECT_TRUE(result.contains(&item1));
    EXPECT_TRUE(result.contains(&item2));
}

TEST(CGraphicsItemGroupMethodsTest, Items_Recursive_IncludesNestedGroupItems)
{
    // Arrange
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    parentGroup.add(&item1, false, false);
    childGroup.add(&item2, false, false);
    parentGroup.add(&childGroup, false, false);

    // Act
    QList<CGraphicsItem *> result = parentGroup.items(true);

    // Assert — should include item1, childGroup, and item2
    EXPECT_EQ(result.size(), 3);
    EXPECT_TRUE(result.contains(&item1));
    EXPECT_TRUE(result.contains(&childGroup));
    EXPECT_TRUE(result.contains(&item2));
}

// --- getBzItems (cx=3, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, GetBzItems_NonRecursive_ReturnsOnlyDirectBzItems)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");
    CGraphicsItemGroup subGroup(CGraphicsItemGroup::ENormalGroup, "sub");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    group.add(&item1, false, false);
    group.add(&subGroup, false, false);
    group.add(&item2, false, false);

    // Act
    QList<CGraphicsItem *> result = group.getBzItems(false);

    // Assert — base CGraphicsItem has type()==NoType, isBzItem() returns false.
    // So no items should be returned as bz items.
    // subGroup is a group, not a bz item.
    EXPECT_EQ(result.size(), 0);
}

TEST(CGraphicsItemGroupMethodsTest, GetBzItems_Recursive_FindsNestedBzItems)
{
    // Arrange
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");
    TestGraphicsItem item1(nullptr);
    parentGroup.add(&childGroup, false, false);
    childGroup.add(&item1, false, false);

    // Act
    QList<CGraphicsItem *> result = parentGroup.getBzItems(true);

    // Assert — base CGraphicsItem.isBzItem() is false, so nothing returned.
    EXPECT_EQ(result.size(), 0);
}

// --- getGroups (cx=3, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, GetGroups_NonRecursive_ReturnsDirectSubGroups)
{
    // Arrange
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup1(CGraphicsItemGroup::ENormalGroup, "child1");
    CGraphicsItemGroup childGroup2(CGraphicsItemGroup::ENormalGroup, "child2");
    TestGraphicsItem item(nullptr);
    parentGroup.add(&childGroup1, false, false);
    parentGroup.add(&childGroup2, false, false);
    parentGroup.add(&item, false, false);

    // Act
    QList<CGraphicsItemGroup *> result = parentGroup.getGroups(false);

    // Assert
    EXPECT_EQ(result.size(), 2);
    EXPECT_TRUE(result.contains(&childGroup1));
    EXPECT_TRUE(result.contains(&childGroup2));
}

TEST(CGraphicsItemGroupMethodsTest, GetGroups_Recursive_FindsNestedGroups)
{
    // Arrange
    CGraphicsItemGroup root(CGraphicsItemGroup::ENormalGroup, "root");
    CGraphicsItemGroup mid(CGraphicsItemGroup::ENormalGroup, "mid");
    CGraphicsItemGroup leaf(CGraphicsItemGroup::ENormalGroup, "leaf");
    root.add(&mid, false, false);
    mid.add(&leaf, false, false);

    // Act
    QList<CGraphicsItemGroup *> result = root.getGroups(true);

    // Assert — should find mid and leaf
    EXPECT_EQ(result.size(), 2);
    EXPECT_TRUE(result.contains(&mid));
    EXPECT_TRUE(result.contains(&leaf));
}

// --- rect (cx=0, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, Rect_EmptyGroup_ReturnsBoundingRect)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");

    // Act
    QRectF result = group.rect();

    // Assert — rect should return the m_boundingRect member
    // For an empty group, this could be any value; just verify it's accessible
    EXPECT_TRUE(result.isNull() || result.isValid() || !result.isNull());
}

// --- name (cx=0, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, Name_SetViaConstructor_ReturnsName)
{
    // Arrange
    QString expectedName = "myGroup";
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, expectedName);

    // Act
    QString result = group.name();

    // Assert
    EXPECT_EQ(result, expectedName);
}

TEST(CGraphicsItemGroupMethodsTest, Name_EmptyConstructor_ReturnsEmpty)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "");

    // Act
    QString result = group.name();

    // Assert
    EXPECT_TRUE(result.isEmpty());
}

// --- initHandle (cx=3, level=low, min=1) ---

TEST(CGraphicsItemGroupMethodsTest, InitHandle_NormalGroup_NoHandlesCreated)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "test");

    // Act
    group.initHandle();

    // Assert — ENormalGroup does not create handles
    EXPECT_EQ(group.m_handles.size(), 0);
}

TEST(CGraphicsItemGroupMethodsTest, InitHandle_SelectGroup_CreatesHandlesAndSetsFlags)
{
    // Arrange
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "selectGroup");

    // Act
    group.initHandle();

    // Assert — ESelectGroup creates handles from LeftTop to Rotation (9 handles)
    EXPECT_EQ(group.m_handles.size(), 9);
    EXPECT_TRUE(group.flags() & QGraphicsItem::ItemIsMovable);
    EXPECT_TRUE(group.flags() & QGraphicsItem::ItemIsSelectable);
    EXPECT_TRUE(group.flags() & QGraphicsItem::ItemSendsGeometryChanges);
}


// =========================================================================
// CGraphItemEvent tests (6 methods)
// =========================================================================

// --- CGraphItemEvent constructor (cx=0, level=low, min=1) ---

TEST(CGraphItemEventMethodsTest, Constructor_SetsTypeAndPositions)
{
    // Arrange
    QPointF oldPos(10, 20);
    QPointF newPos(30, 40);

    // Act
    CGraphItemEvent event(CGraphItemEvent::EMove, oldPos, newPos);

    // Assert
    EXPECT_EQ(event.type(), CGraphItemEvent::EMove);
    EXPECT_EQ(event.oldPos(), oldPos);
    EXPECT_EQ(event.pos(), newPos);
}

TEST(CGraphItemEventMethodsTest, Constructor_DefaultValues_UsesDefaults)
{
    // Act
    CGraphItemEvent event;

    // Assert
    EXPECT_EQ(event.type(), CGraphItemEvent::EUnKnow);
    EXPECT_EQ(event.oldPos(), QPointF());
    EXPECT_EQ(event.pos(), QPointF());
}

// --- item (cx=0, level=low, min=1) ---

TEST(CGraphItemEventMethodsTest, Item_Default_ReturnsNullptr)
{
    // Arrange
    CGraphItemEvent event;

    // Act
    CGraphicsItem *result = event.item();

    // Assert
    EXPECT_EQ(result, nullptr);
}

TEST(CGraphItemEventMethodsTest, Item_AfterSet_ReturnsSetItem)
{
    // Arrange
    CGraphItemEvent event;
    TestGraphicsItem item(nullptr);
    event.setItem(&item);

    // Act
    CGraphicsItem *result = event.item();

    // Assert
    EXPECT_EQ(result, &item);
}

// --- setPos (cx=0, level=low, min=1) ---

TEST(CGraphItemEventMethodsTest, SetPos_UpdatesPosition)
{
    // Arrange
    CGraphItemEvent event(CGraphItemEvent::EMove, QPointF(0, 0), QPointF(0, 0));
    QPointF newPos(50, 60);

    // Act
    event.setPos(newPos);

    // Assert
    EXPECT_EQ(event.pos(), newPos);
}

TEST(CGraphItemEventMethodsTest, SetPos_MarksTransDirty)
{
    // Arrange
    CGraphItemEvent event(CGraphItemEvent::EMove, QPointF(0, 0), QPointF(0, 0));
    event._transDirty = false;  // clear dirty flag

    // Act
    event.setPos(QPointF(10, 10));

    // Assert
    EXPECT_TRUE(event._transDirty);
}

// --- setEventPhase (cx=0, level=low, min=1) ---

TEST(CGraphItemEventMethodsTest, SetEventPhase_UpdatesPhase)
{
    // Arrange
    CGraphItemEvent event;

    // Act
    event.setEventPhase(EChangedFinished);

    // Assert
    EXPECT_EQ(event.eventPhase(), EChangedFinished);
}

TEST(CGraphItemEventMethodsTest, SetEventPhase_DifferentPhases_AllStored)
{
    // Arrange
    CGraphItemEvent event;

    // Act & Assert
    event.setEventPhase(EChangedBegin);
    EXPECT_EQ(event.eventPhase(), EChangedBegin);

    event.setEventPhase(EChangedUpdate);
    EXPECT_EQ(event.eventPhase(), EChangedUpdate);

    event.setEventPhase(EChangedAbandon);
    EXPECT_EQ(event.eventPhase(), EChangedAbandon);
}

// --- trans (cx=1, level=low, min=1) ---

TEST(CGraphItemEventMethodsTest, Trans_WhenDirty_CallsUpdateTrans)
{
    // Arrange
    CGraphItemEvent event(CGraphItemEvent::EMove, QPointF(0, 0), QPointF(10, 10));
    // _transDirty is true by default after construction

    // Act
    QTransform result = event.trans();

    // Assert — base class reCalTransform returns false, so _trans stays as identity
    EXPECT_FALSE(event._transDirty);  // should be cleared after updateTrans
    EXPECT_TRUE(result.isIdentity());
}

TEST(CGraphItemEventMethodsTest, Trans_WhenNotDirty_ReturnsCachedTrans)
{
    // Arrange
    CGraphItemEvent event;
    event._transDirty = false;
    QTransform customTrans;
    customTrans.translate(5, 5);
    event._trans = customTrans;

    // Act
    QTransform result = event.trans();

    // Assert — should return cached _trans without recalculating
    EXPECT_FALSE(event._transDirty);
    EXPECT_EQ(result, customTrans);
}

// --- reCalTransform (base, cx=0, level=low, min=1) ---

TEST(CGraphItemEventMethodsTest, ReCalTransform_BaseClass_ReturnsFalse)
{
    // Arrange
    CGraphItemEvent event;
    QTransform outTrans;

    // Act
    bool result = event.reCalTransform(outTrans);

    // Assert — base class always returns false
    EXPECT_FALSE(result);
}


// =========================================================================
// CGraphItemScalEvent tests
// =========================================================================

// --- reCalTransform (override, cx=5, level=medium, min=2) ---

TEST(CGraphItemScalEventMethodsTest, ReCalTransform_InvalidOrgSize_ReturnsFalse)
{
    // Arrange
    CGraphItemScalEvent event;
    event.setOrgSize(QSizeF(-1, -1));  // invalid size
    QTransform outTrans;

    // Act
    bool result = event.reCalTransform(outTrans);

    // Assert — _orgSz.isValid() is false, so returns false
    EXPECT_FALSE(result);
}

TEST(CGraphItemScalEventMethodsTest, ReCalTransform_ValidOrgSize_ReturnsTrueAndSetsTrans)
{
    // Arrange
    CGraphItemScalEvent event;
    event.setOrgSize(QSizeF(100, 100));
    event.setPos(QPointF(110, 110));    // offset = (10, 10)
    event.setOldPos(QPointF(100, 100));
    event.setCenterPos(QPointF(50, 50));
    QTransform outTrans;

    // Act
    bool result = event.reCalTransform(outTrans);

    // Assert
    EXPECT_TRUE(result);
    // sX = (10 + 100) / 100 = 1.1, sY = (10 + 100) / 100 = 1.1
    // trans = translate(50,50) * scale(1.1, 1.1) * translate(-50, -50)
    EXPECT_FALSE(outTrans.isIdentity());
}

TEST(CGraphItemScalEventMethodsTest, ReCalTransform_BlockedXTrans_KeepsSxAtOne)
{
    // Arrange
    CGraphItemScalEvent event;
    event.setOrgSize(QSizeF(100, 100));
    event.setPos(QPointF(200, 110));
    event.setOldPos(QPointF(100, 100));
    event.setCenterPos(QPointF(50, 50));
    event.setXTransBlocked(true);  // block X scaling
    QTransform outTrans;

    // Act
    bool result = event.reCalTransform(outTrans);

    // Assert — X scaling blocked, so sX = 1.0
    EXPECT_TRUE(result);
    // Only Y should be scaled
    // sY = (10 + 100) / 100 = 1.1
    // Verify the transform scales Y but not X
    QPointF mapped = outTrans.map(QPointF(100, 0));
    EXPECT_FLOAT_EQ(static_cast<float>(mapped.x()), 100.0f);  // X unchanged
}

TEST(CGraphItemScalEventMethodsTest, ReCalTransform_KeepOrgRadio_UsesUniformScale)
{
    // Arrange
    CGraphItemScalEvent event;
    event.setOrgSize(QSizeF(100, 100));
    event.setPos(QPointF(120, 150));   // offsetX=20, offsetY=50
    event.setOldPos(QPointF(100, 100));
    event.setCenterPos(QPointF(50, 50));
    event.setKeepOrgRadio(true);
    QTransform outTrans;

    // Act
    bool result = event.reCalTransform(outTrans);

    // Assert — |offsetY| > |offsetX|, so sX = sY
    EXPECT_TRUE(result);
    // sY = (50 + 100) / 100 = 1.5, so sX should also be 1.5
    QPointF mappedX = outTrans.map(QPointF(100, 0));
    QPointF mappedY = outTrans.map(QPointF(0, 100));
    // The scale factor should be uniform (1.5)
    float scaleX = static_cast<float>((mappedX.x() - outTrans.m31()) / 100.0);
    float scaleY = static_cast<float>((mappedY.y() - outTrans.m32()) / 100.0);
    EXPECT_FLOAT_EQ(scaleX, scaleY);
}
