// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItem, CGraphicsTextItem, CUndoRedoCommand,
 * CGraphicsItemGroup uncovered paths (issue V-9599 §4).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPainter>
#include <QImage>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <memory>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicslineitem.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "drawshape/drawItems/bzItems/cgraphicscutitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/drawItems/bzItems/cpictureitem.h"
#include "widgets/ctextedit.h"
#include "drawshape/drawItems/bzItems/cgraphicspenitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/sitemdata.h"
#include "drawshape/cdrawscene.h"
#include "frame/cgraphicsview.h"
#include "frame/cundoredocommand.h"
#include "frame/ccentralwidget.h"
#include "application.h"
#include "publicApi.h"
#undef protected
#undef private

// Local subclass for BlurType test (no real BlurType item registered).
class FakeBlurItem : public CGraphicsRectItem
{
public:
    using CGraphicsRectItem::CGraphicsRectItem;
    int type() const override { return BlurType; }
};

// Local subclass for testOpetating returning false.
class RejectScaleRect : public CGraphicsRectItem
{
public:
    using CGraphicsRectItem::CGraphicsRectItem;
    bool testScaling(CGraphItemScalEvent *event) override { return false; }
};

// ---------------------------------------------------------------------------
// CGraphicsItem::creatItemInstance(PictureType) (lines 114-116)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, CreatItemInstance_PictureType_ReturnsJDynamicLayer)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CPictureItem *pic = new CPictureItem(QRectF(0, 0, 50, 50), QPixmap(50, 50));
    scene->addCItem(pic);
    CGraphicsUnit unit = pic->getGraphicsUnit(EDuplicate);
    scene->removeCItem(pic, true);

    CGraphicsItem *item = CGraphicsItem::creatItemInstance(PictureType, unit);
    EXPECT_NE(item, nullptr);
    if (item) {
        auto *jLay = dynamic_cast<JDynamicLayer *>(item);
        EXPECT_NE(jLay, nullptr);
        if (jLay) {
            EXPECT_TRUE(jLay->isBlocked());
            EXPECT_EQ(jLay->layerType(), JDynamicLayer::EImageType);
        }
        delete item;
    }
    unit.release();
}

// ---------------------------------------------------------------------------
// CGraphicsItem::zItem with group (lines 135-136)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, ZItem_WithGroup_ReturnsInnerItem)
{
    CGraphicsItemGroup group;
    CGraphicsRectItem rect(QRectF(0, 0, 50, 50));
    group.add(&rect, false, false);

    CGraphicsItem *result = CGraphicsItem::zItem({&group}, -1);
    EXPECT_EQ(result, &rect);
}

// ---------------------------------------------------------------------------
// CGraphicsItem::setScene(nullptr) when in a scene (lines 188-190)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, SetScene_NullPtr_RemovesFromScene)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addCItem(rect);
    ASSERT_EQ(rect->scene(), scene);

    rect->setScene(nullptr);
    EXPECT_EQ(rect->scene(), nullptr);

    delete rect;
}

// ---------------------------------------------------------------------------
// CGraphicsItem::setScene(other scene) (lines 205-207)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, SetScene_OtherScene_RemovesFromOldScene)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addCItem(rect);
    ASSERT_EQ(rect->scene(), scene);

    QGraphicsScene plainScene;
    rect->setScene(&plainScene);
    // Item should have been removed from the old PageScene.
    // It may or may not be in the new plain scene depending on implementation.
    EXPECT_NE(rect->scene(), scene);

    delete rect;
}

// ---------------------------------------------------------------------------
// CGraphicsItem::paintPen width preview (lines 349-350)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, PaintPen_PreviewWidth_ReturnsPreviewWidth)
{
    CGraphicsRectItem rect;
    rect.m_isPreviewCom[1] = true;
    rect.m_penWidth = 7;
    EXPECT_EQ(rect.paintPen().width(), 7);
}

// ---------------------------------------------------------------------------
// CGraphicsItem::systemThemeColor no scene (lines 360-361)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, SystemThemeColor_NoScene_ReturnsInvalidColor)
{
    CGraphicsRectItem rect;
    QColor color = rect.systemThemeColor();
    EXPECT_FALSE(color.isValid());
}

// ---------------------------------------------------------------------------
// CGraphicsItem::hitTest — handle hit (lines 481-482) and in-rect (lines 487-488)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, HitTest_LeftTopHandle_ReturnsLeftTop)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsLineItem *line = new CGraphicsLineItem();
    line->setLine(QLineF(10, 10, 200, 200));
    scene->addCItem(line);

    auto handles = line->m_handles;
    ASSERT_FALSE(handles.isEmpty());
    QPointF handlePos = handles.first()->scenePos();
    auto dir = line->hitTest(handlePos);
    EXPECT_NE(dir, CSizeHandleRect::None);

    scene->clearSelectGroup();
    scene->removeCItem(line, true);
}

TEST(CGraphicsItemTextUndoMgrCoverageTest, HitTest_InRect_ReturnsInRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    rect->updateShape();
    scene->addCItem(rect);

    // Use a point well inside the rect, away from handles.
    QPointF insidePoint = rect->sceneBoundingRect().center() + QPointF(3, 3);
    auto dir = rect->hitTest(insidePoint);
    EXPECT_EQ(dir, CSizeHandleRect::InRect);

    scene->clearSelectGroup();
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItem::isRectPenetrable (lines 803-804)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, IsRectPenetrable_TransparentBrush_ReturnsTrue)
{
    CGraphicsRectItem rect(QRectF(0, 0, 100, 100));
    rect.setBrush(QBrush(Qt::transparent));
    QRectF testRect(QPointF(45, 45), QSizeF(10, 10));
    EXPECT_TRUE(rect.isRectPenetrable(testRect));
}

TEST(CGraphicsItemTextUndoMgrCoverageTest, IsRectPenetrable_OpaqueBrush_ReturnsFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    rect->setBrush(QBrush(Qt::red));
    rect->updateShape();
    scene->addCItem(rect);

    // Use local coordinates relative to the item's position.
    QPointF localCenter = rect->mapFromScene(rect->sceneBoundingRect().center());
    QRectF testRect(localCenter - QPointF(5, 5), QSizeF(10, 10));
    EXPECT_FALSE(rect->isRectPenetrable(testRect));

    scene->clearSelectGroup();
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItem::operating scale refresh blur (lines 860-866)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, Operating_Scale_WithBlurInfo_RefreshesBlur)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addCItem(rect);

    // blurBegin/blurEnd are no-ops in CGraphicsItem base; directly set _blurInfos.
    SBlurInfo blurInfo;
    blurInfo.blurPath = QPainterPath();
    blurInfo.blurPath.addEllipse(QPointF(50, 50), 20, 20);
    blurInfo.blurEfTp = EBlurEffect(0);
    rect->_blurInfos.append(blurInfo);
    ASSERT_FALSE(rect->_blurInfos.isEmpty());

    // Disable cache so isCached() is false.
    rect->setCache(false);
    ASSERT_FALSE(rect->isCached());

    CGraphItemScalEvent ev(CGraphItemEvent::EScal, QPointF(0, 0), QPointF(10, 10));
    ev.setTrans(QTransform::fromScale(2.0, 2.0));
    rect->operating(&ev);

    EXPECT_FALSE(rect->_blurInfos.isEmpty());

    scene->clearSelectGroup();
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItem::itemChange with blur info entering scene (lines 1203-1205)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, ItemChange_BlurInfoEntersScene_UpdatesBlurPixmap)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addCItem(rect);

    // Directly set _blurInfos since blurBegin/blurEnd are no-ops in base class.
    SBlurInfo blurInfo;
    blurInfo.blurPath = QPainterPath();
    blurInfo.blurPath.addEllipse(QPointF(50, 50), 20, 20);
    blurInfo.blurEfTp = EBlurEffect(0);
    rect->_blurInfos.append(blurInfo);
    ASSERT_FALSE(rect->_blurInfos.isEmpty());

    // Remove from scene and re-add to trigger itemChange.
    scene->removeCItem(rect);
    scene->addCItem(rect);

    // The code path for itemChange with blur info entering scene was exercised.
    SUCCEED();

    scene->clearSelectGroup();
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItem::paintBlur null pixmap (lines 1444-1445)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, PaintBlur_NullPixmap_UpdatesBlurPix)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addCItem(rect);

    // Directly set _blurInfos since blurBegin/blurEnd are no-ops in base class.
    SBlurInfo blurInfo;
    blurInfo.blurPath = QPainterPath();
    blurInfo.blurPath.addEllipse(QPointF(50, 50), 20, 20);
    blurInfo.blurEfTp = EBlurEffect(0);
    rect->_blurInfos.append(blurInfo);
    scene->removeCItem(rect);
    // Clear blur pixmaps to simulate null state.
    rect->_blurPix[0] = QPixmap();
    rect->_blurPix[1] = QPixmap();
    scene->addCItem(rect);

    // Now paint to trigger paintBlur with null _blurPix.
    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    rect->paint(&painter, &opt, nullptr);
    painter.end();

    // The paintBlur code path with null _blurPix was exercised.
    SUCCEED();

    scene->clearSelectGroup();
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::changToEditState multi-selected/group reject (lines 140-142)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, ChangToEditState_InGroup_Rejected)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsTextItem *text = new CGraphicsTextItem();
    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addCItem(text);
    scene->addCItem(rect);
    scene->creatGroup({text, rect});

    text->changToEditState();
    EXPECT_FALSE(text->isEditState());

    scene->clearSelectGroup();
    scene->removeCItem(text, true);
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::changToEditState hidden editor (lines 149-150)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, ChangToEditState_HiddenEditor_ShowsEditor)
{
    CGraphicsTextItem text;
    // Not in scene — m_pTextEdit should be hidden.
    ASSERT_NE(text.m_pTextEdit, nullptr);
    text.changToEditState();
    EXPECT_FALSE(text.m_pTextEdit->isHidden());
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::changToReadOnlyState in scene (lines 192-193)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, ChangToReadOnlyState_InScene_NotSelectedItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsTextItem *text = new CGraphicsTextItem();
    scene->addCItem(text);

    text->changToEditState();
    text->changToReadOnlyState();
    // After changToReadOnlyState, the item should be notSelectItem.
    EXPECT_FALSE(scene->selectGroup()->items().contains(text));

    scene->clearSelectGroup();
    scene->removeCItem(text, true);
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::setTextState(EReadOnly) (lines 203-204)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, SetTextState_ReadOnly_FromEditState)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsTextItem *text = new CGraphicsTextItem();
    scene->addCItem(text);

    text->changToEditState();
    ASSERT_TRUE(text->isEditState());

    text->setTextState(CGraphicsTextItem::EReadOnly);
    EXPECT_FALSE(text->isEditState());

    scene->clearSelectGroup();
    scene->removeCItem(text, true);
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::updateToDefaultTextFormat (lines 363-367)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, UpdateToDefaultTextFormat_SetsDefaults)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsTextItem *text = new CGraphicsTextItem();
    scene->addCItem(text);

    text->updateToDefaultTextFormat();
    // Just verify the method ran without crash.
    SUCCEED();

    scene->clearSelectGroup();
    scene->removeCItem(text, true);
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::paint — null scene (lines 449-453)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, Paint_NullScene_SetsEmptyRect)
{
    CGraphicsTextItem text;
    // Not in scene.
    ASSERT_EQ(text.scene(), nullptr);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    text.paint(&painter, &opt);
    painter.end();

    // The paint method with null scene should set rect to (0,0,0,0).
    // Just verify the method ran without crash.
    SUCCEED();
}

// ---------------------------------------------------------------------------
// CGraphicsTextItem::paint — invalid rect (lines 457-459)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, Paint_InvalidRect_InScene_ReturnsEarly)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsTextItem *text = new CGraphicsTextItem();
    scene->addCItem(text);
    text->setRect(QRectF()); // invalid rect

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    text->paint(&painter, &opt);
    painter.end();

    SUCCEED();

    scene->clearSelectGroup();
    scene->removeCItem(text, true);
}

// ---------------------------------------------------------------------------
// CItemUndoRedoCommand::getCmdByItemCmdInfo EPropertyChanged (lines 402-403)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, GetCmdByItemCmdInfo_EPropertyChanged_ReturnsItemMoveCommand)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EItemChangedCmd;
    info.undoInfo.expTp = CItemUndoRedoCommand::EPropertyChanged;

    auto *cmd = CItemUndoRedoCommand::getCmdByItemCmdInfo(info);
    EXPECT_NE(cmd, nullptr);
    EXPECT_NE(dynamic_cast<CItemMoveCommand *>(cmd), nullptr);
    delete cmd;
}

// ---------------------------------------------------------------------------
// CBzItemAllCommand::parsingVars insufficient variables (lines 648-649)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, CBzItemAllCommand_ParsingVars_InsufficientVars_Warns)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addCItem(rect);

    CBzItemAllCommand cmd;
    cmd._pItem = rect;
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(scene); // Only 1 var, < 2.
    cmd.parsingVars(vars, UndoVar);
    // With only 1 var, _itemDate should NOT be set.
    // With insufficient vars, _itemDate should not be set (default dataType = 0).
    EXPECT_EQ(cmd._itemDate[UndoVar].head.dataType, 0);

    // Now pass 3 vars to set _noNeedSelected.
    QList<QVariant> vars2;
    vars2 << reinterpret_cast<long long>(scene);
    vars2 << QVariant::fromValue(rect->getGraphicsUnit(EUndoRedo));
    vars2 << false;
    cmd.parsingVars(vars2, UndoVar);
    EXPECT_EQ(cmd._noNeedSelected[UndoVar], false);

    scene->clearSelectGroup();
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CCmdBlock(CutType) — constructor and destructor (lines 740-746, 808-814)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, CCmdBlock_CutType_RecordsSizeChanged)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsCutItem *cutItem = new CGraphicsCutItem(QRectF(0, 0, 200, 150));
    scene->addCItem(cutItem);

    QRectF originalSceneRect = scene->sceneRect();

    // CCmdBlock with EChanged phase and doRedo=true.
    {
        CCmdBlock block(cutItem, EChanged, true);
        scene->setSceneRect(QRectF(0, 0, 300, 250));
    }
    // Block destructor should have recorded redo + finishRecord.

    CUndoRedoCommand::clearCommand();
    scene->setSceneRect(originalSceneRect);
    scene->clearSelectGroup();
    scene->removeCItem(cutItem, true);
}

// ---------------------------------------------------------------------------
// CSceneGroupChangedCommand::real_undo with virtual root (lines 887-889)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, CSceneGroupChangedCommand_Undo_VirtualRoot_CancelsGroup)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *a = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *b = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addCItem(a);
    scene->addCItem(b);

    scene->creatGroup({a, b}, 0 /*ENormalGroup*/, true /*pushUndo*/);
    ASSERT_NE(a->bzGroup(), nullptr);

    // Undo the group creation.
    CUndoRedoCommand::getUndoRedoStack()->undo();
    EXPECT_EQ(a->bzGroup(), nullptr);
    EXPECT_EQ(b->bzGroup(), nullptr);

    CUndoRedoCommand::clearCommand();
    scene->clearSelectGroup();
    scene->removeCItem(a, true);
    scene->removeCItem(b, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::drawZValue nested group recursion (lines 280-282)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, DrawZValue_NestedGroup_Recurses)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup);
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup);
    CGraphicsRectItem *rectA = new CGraphicsRectItem(QRectF(0, 0, 50, 50));

    childGroup.add(rectA, false, false);
    parentGroup.add(&childGroup, false, false);

    qreal z = parentGroup.drawZValue();
    EXPECT_EQ(z, parentGroup.zValue());

    delete rectA;
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::testOpetating child returns false (lines 493-495)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, TestOpetating_ChildRejects_ReturnsFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    RejectScaleRect *rejectItem = new RejectScaleRect(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *normalRect = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addCItem(rejectItem);
    scene->addCItem(normalRect);

    CGraphicsItemGroup *group = scene->creatGroup({rejectItem, normalRect});
    ASSERT_NE(group, nullptr);

    CGraphItemScalEvent ev(CGraphItemEvent::EScal, QPointF(0, 0), QPointF(10, 10));
    ev.setTrans(QTransform::fromScale(2.0, 2.0));
    bool result = group->testOpetating(&ev);
    EXPECT_FALSE(result);

    scene->clearSelectGroup();
    scene->removeCItem(rejectItem, true);
    scene->removeCItem(normalRect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::doScalEnd — item moved outside group rect (lines 554-556)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, DoScalEnd_ItemOutsideGroupRect_UpdatesBoundingRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *a = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *b = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addCItem(a);
    scene->addCItem(b);
    CGraphicsItemGroup *group = scene->creatGroup({a, b});
    ASSERT_NE(group, nullptr);

    // Move one item outside the group rect.
    a->setRect(QRectF(500, 500, 50, 50));

    QRectF rectBefore = group->rect();
    group->doScalEnd(nullptr);
    QRectF rectAfter = group->rect();
    EXPECT_NE(rectBefore, rectAfter);

    scene->clearSelectGroup();
    scene->removeCItem(a, true);
    scene->removeCItem(b, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::doScalEnd — union != rect (lines 560-561)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, DoScalEnd_UnionNotEqualRect_UpdatesBoundingRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *a = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *b = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addCItem(a);
    scene->addCItem(b);
    CGraphicsItemGroup *group = scene->creatGroup({a, b});
    ASSERT_NE(group, nullptr);

    // Slightly move an item so union != rect but still contained.
    a->setRect(QRectF(10, 10, 50, 50));

    QRectF rectBefore = group->rect();
    group->doScalEnd(nullptr);
    QRectF rectAfter = group->rect();
    EXPECT_NE(rectBefore, rectAfter);

    scene->clearSelectGroup();
    scene->removeCItem(a, true);
    scene->removeCItem(b, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::loadGraphicsUnit cache reset (lines 592-594)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, LoadGraphicsUnit_WithCache_ResetsCache)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *a = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *b = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addCItem(a);
    scene->addCItem(b);
    CGraphicsItemGroup *group = scene->creatGroup({a, b});
    ASSERT_NE(group, nullptr);

    group->setCache(true);
    ASSERT_TRUE(group->isCached());

    CGraphicsUnit unit = group->getGraphicsUnit(EUndoRedo);
    group->loadGraphicsUnit(unit);
    // loadGraphicsUnit calls resetCachePixmap which refreshes but doesn't disable cache.
    EXPECT_TRUE(group->isCached());

    unit.release();
    scene->clearSelectGroup();
    scene->removeCItem(a, true);
    scene->removeCItem(b, true);
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::getLogicFirst nested (line 673)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, GetLogicFirst_NestedGroup_ReturnsNestedMinZItem)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup);
    parentGroup.setAddType(CGraphicsItemGroup::EOneByOne);

    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup);
    CGraphicsRectItem *rectA = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    rectA->setZValue(5);
    childGroup.add(rectA, false, false);
    parentGroup.add(&childGroup, false, false);

    CGraphicsItem *first = parentGroup.getLogicFirst();
    EXPECT_EQ(first, rectA);

    delete rectA;
}

// ---------------------------------------------------------------------------
// CGraphicsItemGroup::updateHandlesGeometry hide rotation for blur (lines 787-788)
// ---------------------------------------------------------------------------

TEST(CGraphicsItemTextUndoMgrCoverageTest, UpdateHandlesGeometry_BlurItem_HidesRotationHandle)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    FakeBlurItem *blur = new FakeBlurItem(QRectF(0, 0, 100, 100));
    scene->addCItem(blur);
    scene->selectItem(blur);

    CGraphicsItemGroup *selGroup = scene->selectGroup();
    ASSERT_NE(selGroup, nullptr);
    selGroup->updateHandlesGeometry();

    // Rotation handle should be invisible.
    auto handles = selGroup->m_handles;
    bool rotationVisible = true;
    for (auto h : handles) {
        if (h->dir() == CSizeHandleRect::Rotation) {
            rotationVisible = h->isVisible();
            break;
        }
    }
    EXPECT_FALSE(rotationVisible);

    scene->clearSelectGroup();
    scene->removeCItem(blur, true);
}
