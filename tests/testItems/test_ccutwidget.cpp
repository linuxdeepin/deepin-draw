// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#define protected public
#define private public
#include "ccutwidget.h"
#include "application.h"
#undef protected
#undef private

#include <QApplication>
#include <QWindow>
#include <QSignalSpy>
#include <QEvent>
#include <QButtonGroup>
#include <QAction>
#include <QLineEdit>

// ================= CCutWidget 分支清单 (branch map) =================
// 说明: 覆盖被测方法的所有 if / switch / 早退分支。
//
// setCutType(current, emitSig, adjustSz)  (complexity=11)
//   B1  current != m_curCutType -> {m_curCutType=current; qInfo; if(emitSig) emit cutTypeChanged;
//                                   if(adjustSz) adjustSize(emitSig);}
//   B2  current == m_curCutType -> 跳过发射与尺寸调整, 但仍 clearAllChecked + 按钮选中 + setFocus
//   B3  emitSig==true  -> 发射 cutTypeChanged
//   B4  emitSig==false -> 不发射 cutTypeChanged
//   B5  adjustSz==true  -> 调用 adjustSize
//   B6  adjustSz==false -> 不调用 adjustSize
//   C1..C6 switch(m_curCutType) 6 个 case 分别 setChecked(true)
//   D1  switch 的 default: break (不选中任何按钮)
// adjustSize(emitSig)  (被 setCutType/initConnection 调用走)
//   E1  m_curCutType < cut_free -> newSize=m_defultRadioSize; width=qRound(height*Radio[t]); setCutSize
//   E2  否则 (cut_free/cut_original) -> 直接返回, 不改尺寸
// setCutSize(sz, emitSig)
//   F1  sz != m_cutCutSize -> 赋值 + setText(宽/高) + if(emitSig) emit cutSizeChanged
//   F2  sz == m_cutCutSize -> 跳过
//   F3  emitSig==true/false -> 决定是否发射 cutSizeChanged
// initConnection
//   G1 m_scaleBtnGroup idToggled/buttonToggled -> (checked) 才 setCutType(ECutType(tp),true,m_autoCal)
//   G2 m_widthEdit::editingFinished -> 读宽/高; 若 newWidth>maxPicSize().width() 钳制(blockSignals回写)
//   G3 m_heightEdit::editingFinished -> 同 G2 对高度钳制
//   G4 m_doneBtn::clicked -> emit finshed(true)
//   G5 m_cancelBtn::clicked -> emit finshed(false)
//   G6 m_SizeAddAction: 宽编辑框有焦点 +1(>4096 早退); 高编辑框有焦点 +1(>4096 早退)
//   G7 m_SizeReduceAction: 宽编辑框有焦点 -1(<10 早退); 高编辑框有焦点 -1(<10 早退)
// eventFilter(o,e)
//   H1  o 非 widget -> 直接返回基类结果
//   H2  widget 但事件不是 ParentChange -> 跳过
//   H3  widget 且 ParentChange 但 parentWidget()==nullptr -> 跳过
//   H4  parent 是窗口: H4a(ToolButton 且是 done/cancel -> setShowText(true)) H4b(其他 ToolButton)
//       H4c(非 ToolButton) -> 均 setBtnStyle(MENU_STYLE) 且 parentWidget()->setFocus()
//   H5  parent 非窗口: H5a(done/cancel ToolButton -> setShowText(false)) H5b(其他 ToolButton)
//       H5c(非 ToolButton) -> 均 setBtnStyle(BUTTON_STYLE)
//   所有分支最终 return DrawAttribution::CAttriBaseOverallWgt::eventFilter(o,e)
//   （基类只重写 showEvent/resizeEvent/event, 未重写 eventFilter -> 逐级回溯到 QObject::eventFilter -> false)
// ====================================================================

namespace {

const QSize kDefaultRadioBaseSize(800, 600);

QPushButton *buttonForType(CCutWidget *w, ECutType t)
{
    switch (t) {
    case cut_1_1:      return w->m_scaleBtn1_1;
    case cut_2_3:      return w->m_scaleBtn2_3;
    case cut_8_5:      return w->m_scaleBtn8_5;
    case cut_16_9:     return w->m_scaleBtn16_9;
    case cut_free:     return w->m_freeBtn;
    case cut_original: return w->m_originalBtn;
    default:           return nullptr;
    }
}

} // namespace

// ---------- setCutType: 比例->尺寸调整 (TEST_P, 4 组同类) ----------
class CCutWidgetRatioAdjustTest : public ::testing::TestWithParam<ECutType>
{
protected:
    void SetUp() override
    {
        m_wgt = new CCutWidget();
        m_spy = new QSignalSpy(m_wgt, &CCutWidget::cutSizeChanged);
        m_wgt->m_defultRadioSize = kDefaultRadioBaseSize;
    }
    void TearDown() override
    {
        delete m_spy;
        delete m_wgt;
    }
    CCutWidget *m_wgt = nullptr;
    QSignalSpy *m_spy = nullptr;
};

TEST_P(CCutWidgetRatioAdjustTest, adjustSizePerRadioRatio)
{
    // Arrange
    ECutType type = GetParam();
    QSize expected;
    switch (type) {
    case cut_1_1:  expected = QSize(600, 600); break;
    case cut_2_3:  expected = QSize(400, 600); break;
    case cut_8_5:  expected = QSize(960, 600); break;
    case cut_16_9: expected = QSize(1067, 600); break; // qRound(600*16/9)
    default:       FAIL(); break;
    }

    // Act
    m_wgt->setCutType(type, true, true);
    // Assert
    EXPECT_EQ(m_wgt->cutSize(), expected);
    EXPECT_EQ(m_wgt->m_widthEdit->text(), QString::number(expected.width()));
    EXPECT_EQ(m_wgt->m_heightEdit->text(), QString::number(expected.height()));
    EXPECT_EQ(m_spy->count(), 1); // cutSizeChanged 发射一次
}

INSTANTIATE_TEST_SUITE_P(RatioGroup, CCutWidgetRatioAdjustTest,
                         ::testing::Values(cut_1_1, cut_2_3, cut_8_5, cut_16_9));

// ---------- setCutType: 自由/原始比例不调整尺寸 (E2) ----------
TEST(CCutWidgetSetCutTypeTest, freeAndOriginalSkipAdjustSize)
{
    // Arrange
    CCutWidget w;
    w.m_defultRadioSize = kDefaultRadioBaseSize;
    // 先显式铺置一个尺寸, 以便验证 cut_free/cut_original 不改动它
    w.setCutSize(QSize(10, 10), false);

    // Act
    w.setCutType(cut_free, true, true);
    // Assert
    EXPECT_EQ(w.cutType(), cut_free);
    EXPECT_EQ(w.cutSize(), QSize(10, 10)); // cut_free 不触发 adjustSize, 尺寸保持

    // Act
    w.setCutType(cut_original, true, true);
    // Assert
    EXPECT_EQ(w.cutType(), cut_original);
    EXPECT_EQ(w.cutSize(), QSize(10, 10)); // cut_original 同样不触发 adjustSize
}

// ---------- setCutType: 同值调用不重复发射/不重复调整 (B2) ----------
TEST(CCutWidgetSetCutTypeTest, sameValueSkipsEmitAndAdjust)
{
    // Arrange
    CCutWidget w;
    w.m_defultRadioSize = kDefaultRadioBaseSize;
    w.setCutType(cut_free, false, false); // 先把状态固定到 cut_free, 不产生信号
    QSignalSpy typeSpy(&w, &CCutWidget::cutTypeChanged);

    // Act
    w.setCutType(cut_free, true, true);
    // Assert
    EXPECT_EQ(typeSpy.count(), 0);          // 同值不发射 cutTypeChanged
    EXPECT_TRUE(w.m_freeBtn->isChecked());  // clearAllChecked + 按钮选中仍会执行
    EXPECT_FALSE(w.m_scaleBtn1_1->isChecked());
}

// ---------- setCutType: emitSig=false 不发射信号 (B4) ----------
TEST(CCutWidgetSetCutTypeTest, emitSigFalseSuppressesSignal)
{
    // Arrange
    CCutWidget w;
    w.m_defultRadioSize = kDefaultRadioBaseSize;
    QSignalSpy typeSpy(&w, &CCutWidget::cutTypeChanged);
    QSignalSpy sizeSpy(&w, &CCutWidget::cutSizeChanged);

    // Act
    w.setCutType(cut_1_1, false, true);
    // Assert
    EXPECT_EQ(typeSpy.count(), 0);
    EXPECT_EQ(sizeSpy.count(), 0); // adjustSize(emitSig=false) 透传, 不发射
    EXPECT_EQ(w.cutSize(), QSize(600, 600)); // 尺寸仍被调整
}

// ---------- setCutType: adjustSz=false 不调整尺寸 (B6) ----------
TEST(CCutWidgetSetCutTypeTest, adjustSzFalseSkipsSize)
{
    // Arrange
    CCutWidget w;
    w.m_defultRadioSize = kDefaultRadioBaseSize;
    // 显式铺置尺寸, 以便验证 adjustSize 不被调用时它保持不变
    w.setCutSize(QSize(55, 66), false);
    QSignalSpy typeSpy(&w, &CCutWidget::cutTypeChanged);

    // Act
    w.setCutType(cut_1_1, true, false);
    // Assert
    EXPECT_EQ(typeSpy.count(), 1);
    EXPECT_EQ(w.cutSize(), QSize(55, 66)); // adjustSize 未调用, 尺寸保持 55x66
    EXPECT_TRUE(w.m_scaleBtn1_1->isChecked());
}

// ---------- setCutType: 6 种类型的按钮选中 (C1..C6) ----------
TEST(CCutWidgetSetCutTypeTest, eachTypeChecksMatchingButton)
{
    // Arrange
    CCutWidget w;
    w.m_defultRadioSize = kDefaultRadioBaseSize;

    for (int i = 0; i < cut_count; ++i) {
        ECutType t = ECutType(i);
        // Act
        w.setCutType(t, true, true);
        // Assert
        for (int j = 0; j < cut_count; ++j) {
            ECutType other = ECutType(j);
            QPushButton *btn = buttonForType(&w, other);
            ASSERT_NE(btn, nullptr);
            EXPECT_EQ(btn->isChecked(), (i == j)) << "type i=" << i << " j=" << j;
        }
    }
}

// ---------- setCutSize: F1/F2/F3 ----------
TEST(CCutWidgetSetCutSizeTest, setAndSkipSame)
{
    // Arrange
    CCutWidget w;
    QSignalSpy sizeSpy(&w, &CCutWidget::cutSizeChanged);

    // Act (F1: 新值 -> 赋值 + setText + 发射)
    w.setCutSize(QSize(100, 200), true);
    // Assert
    EXPECT_EQ(w.cutSize(), QSize(100, 200));
    EXPECT_EQ(w.m_widthEdit->text(), QString("100"));
    EXPECT_EQ(w.m_heightEdit->text(), QString("200"));
    EXPECT_EQ(sizeSpy.count(), 1);

    // Act (F2: 同值 -> 跳过)
    w.setCutSize(QSize(100, 200), true);
    // Assert
    EXPECT_EQ(sizeSpy.count(), 1);
    EXPECT_EQ(w.m_widthEdit->text(), QString("100"));

    // Act (F3: emitSig=false -> 不发射)
    w.setCutSize(QSize(300, 400), false);
    // Assert
    EXPECT_EQ(w.cutSize(), QSize(300, 400));
    EXPECT_EQ(sizeSpy.count(), 1);
}

// ---------- initUI ----------
TEST(CCutWidgetInitUITest, widgetsAreConfigured)
{
    // Arrange
    CCutWidget w;

    // Assert: 宽/高编辑框
    EXPECT_EQ(w.m_widthEdit->objectName(), QString("CutWidthLineEdit"));
    EXPECT_EQ(w.m_widthEdit->text(), QString("800"));
    EXPECT_NE(w.m_widthEdit->lineEdit()->validator(), nullptr);
    EXPECT_EQ(w.m_heightEdit->objectName(), QString("CutHeightLineEdit"));
    EXPECT_EQ(w.m_heightEdit->text(), QString("600"));

    // Assert: 比例按钮 (ToolButton, 可勾选)
    struct BtnExpect {
        QPushButton *btn;
        QString text;
    };
    const BtnExpect expects[] = {
        {w.m_scaleBtn1_1, QString("1:1")},
        {w.m_scaleBtn2_3, QString("2:3")},
        {w.m_scaleBtn8_5, QString("8:5")},
        {w.m_scaleBtn16_9, QString("16:9")},
        {w.m_freeBtn, QObject::tr("Free")},
        {w.m_originalBtn, QObject::tr("Original")},
    };
    for (const BtnExpect &e : expects) {
        EXPECT_EQ(e.btn->text(), e.text) << "btn text";
        EXPECT_TRUE(e.btn->isCheckable()) << "btn checkable";
    }

    // Assert: 比例按钮组 (互斥 + 映射 id)
    ASSERT_NE(w.m_scaleBtnGroup, nullptr);
    EXPECT_TRUE(w.m_scaleBtnGroup->exclusive());
    EXPECT_EQ(w.m_scaleBtnGroup->id(w.m_scaleBtn1_1), int(cut_1_1));
    EXPECT_EQ(w.m_scaleBtnGroup->id(w.m_scaleBtn2_3), int(cut_2_3));
    EXPECT_EQ(w.m_scaleBtnGroup->id(w.m_scaleBtn8_5), int(cut_8_5));
    EXPECT_EQ(w.m_scaleBtnGroup->id(w.m_scaleBtn16_9), int(cut_16_9));
    EXPECT_EQ(w.m_scaleBtnGroup->id(w.m_freeBtn), int(cut_free));
    EXPECT_EQ(w.m_scaleBtnGroup->id(w.m_originalBtn), int(cut_original));

    // Assert: 尺寸加/减 Action
    EXPECT_EQ(w.m_SizeAddAction->objectName(), QString("CutSizeKeyUp"));
    EXPECT_EQ(w.m_SizeReduceAction->objectName(), QString("CutSizeKeyDown"));
}

// ---------- initConnection: 比例按钮点击 -> cutTypeChanged (G1) ----------
TEST(CCutWidgetInitConnectionTest, scaleButtonClickSetsCutType)
{
    // Arrange
    CCutWidget w;
    QSignalSpy typeSpy(&w, &CCutWidget::cutTypeChanged);

    // Act
    w.m_scaleBtn1_1->click();
    // Assert
    ASSERT_EQ(typeSpy.count(), 1);
    EXPECT_EQ(typeSpy.at(0).at(0).toInt(), int(cut_1_1));
    EXPECT_EQ(w.cutType(), cut_1_1);

    // Act: 同组其他按钮
    w.m_scaleBtn16_9->click();
    // Assert
    EXPECT_EQ(w.cutType(), cut_16_9);
}

// ---------- initConnection: 宽编辑框 editingFinished (G2 正常路径) ----------
TEST(CCutWidgetInitConnectionTest, widthEditingFinishedSetsCutSize)
{
    // Arrange
    CCutWidget w; // 默认 宽=800, 高=600
    QSignalSpy sizeSpy(&w, &CCutWidget::cutSizeChanged);

    // Act: 正常路径 (100 < maxPicSize().width())
    w.m_widthEdit->setText("100");
    emit w.m_widthEdit->editingFinished();
    // Assert
    EXPECT_EQ(w.cutSize(), QSize(100, 600));
    EXPECT_EQ(w.cutType(), cut_free); // 循环未命中比例, 归一 cut_free
    EXPECT_GE(sizeSpy.count(), 1);
}

// ---------- initConnection: 宽编辑框钳制 (G2 clamp) ----------
TEST(CCutWidgetInitConnectionTest, widthOverMaxIsClamped)
{
    // Arrange
    CCutWidget w; // 默认 高=600
    w.m_widthEdit->setText("99999");

    // Act: 99999 > maxPicSize().width()
    emit w.m_widthEdit->editingFinished();
    // Assert
    const QSize maxSize = Application::drawApplication()->maxPicSize();
    EXPECT_EQ(w.m_widthEdit->text(), QString::number(maxSize.width()));
    EXPECT_EQ(w.cutSize(), QSize(maxSize.width(), 600));
}

// ---------- initConnection: 高编辑框钳制 (G3 clamp) ----------
TEST(CCutWidgetInitConnectionTest, heightOverMaxIsClamped)
{
    // Arrange
    CCutWidget w; // 默认 宽=800
    w.m_heightEdit->setText("99999");

    // Act: 99999 > maxPicSize().height()
    emit w.m_heightEdit->editingFinished();
    // Assert
    const QSize maxSize = Application::drawApplication()->maxPicSize();
    EXPECT_EQ(w.m_heightEdit->text(), QString::number(maxSize.height()));
    EXPECT_EQ(w.cutSize(), QSize(800, maxSize.height()));
}

// ---------- initConnection: doneBtn / cancelBtn (G4/G5) ----------
TEST(CCutWidgetInitConnectionTest, doneAndCancelEmitFinshed)
{
    // Arrange
    CCutWidget w;
    QSignalSpy spy(&w, &CCutWidget::finshed);

    // Act
    emit w.m_doneBtn->clicked();
    // Assert
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(spy.at(0).at(0).toBool());

    // Act
    emit w.m_cancelBtn->clicked();
    // Assert
    ASSERT_EQ(spy.count(), 2);
    EXPECT_FALSE(spy.at(1).at(0).toBool());
}

// ---------- initConnection: SizeAdd / SizeReduce (G6/G7) ----------
TEST(CCutWidgetInitConnectionTest, sizeAddReduceAdjustFocusedEdit)
{
    // Arrange
    CCutWidget w;
    w.m_widthEdit->setText("100");

    // The G6/G7 slots branch on hasFocus(), which only becomes true once the
    // widget's window is shown and active. Under the offscreen platform the
    // QWindow can also lose its active state mid-test (deferred setFocus()),
    // so re-activate before focusing an edit.
    w.show();
    auto focusEdit = [&w](DLineEdit *edit) {
        if (w.windowHandle() && !w.windowHandle()->isActive()) {
            w.windowHandle()->requestActivate();
            qApp->processEvents();
        }
        edit->lineEdit()->setFocus();
    };

    // Act: 宽编辑框获得焦点后触发 SizeAdd
    focusEdit(w.m_widthEdit);
    ASSERT_TRUE(w.m_widthEdit->lineEdit()->hasFocus());
    w.m_SizeAddAction->trigger();
    // Assert
    EXPECT_EQ(w.m_widthEdit->lineEdit()->text(), QString("101"));

    // Act: 宽编辑框获得焦点后触发 SizeReduce
    focusEdit(w.m_widthEdit);
    ASSERT_TRUE(w.m_widthEdit->lineEdit()->hasFocus());
    w.m_SizeAddAction->trigger();  // 101 -> 102
    w.m_SizeReduceAction->trigger(); // 102 -> 101
    // Assert
    EXPECT_EQ(w.m_widthEdit->lineEdit()->text(), QString("101"));

    // Act: 高度编辑框获得焦点后 trigger
    w.m_heightEdit->setText("200");
    // 宽编辑框失焦时其 editingFinished 处理器会 this->setFocus() 抢回焦点,
    // 第一次 focusEdit 让该失焦副作用完成, 第二次再真正聚焦高度编辑框。
    focusEdit(w.m_heightEdit);
    focusEdit(w.m_heightEdit);
    ASSERT_TRUE(w.m_heightEdit->lineEdit()->hasFocus());
    w.m_SizeAddAction->trigger();
    // Assert
    EXPECT_EQ(w.m_heightEdit->lineEdit()->text(), QString("201"));
}

// ---------- eventFilter: 所有分支 (H1-H5) ----------
class CCutWidgetEventFilterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_wgt = new CCutWidget();      // 无父 -> 自身是窗口
        m_ev = new QEvent(QEvent::ParentChange);
        m_evOther = new QEvent(QEvent::None);
    }
    void TearDown() override
    {
        delete m_ev;
        delete m_evOther;
        delete m_wgt;
    }
    CCutWidget *m_wgt = nullptr;
    QEvent *m_ev = nullptr;
    QEvent *m_evOther = nullptr;
};

TEST_F(CCutWidgetEventFilterTest, nonWidgetObjectNoOp)      // H1
{
    // Arrange
    QObject obj;
    // Act
    bool ret = m_wgt->eventFilter(&obj, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, nonParentChangeEventNoOp) // H2
{
    // Arrange
    QObject obj(m_wgt);
    // Act
    bool ret = m_wgt->eventFilter(&obj, m_evOther);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, noParentNoOp)             // H3
{
    // Arrange
    QWidget orphan; // 无父 (parentWidget()==nullptr)
    // Act
    bool ret = m_wgt->eventFilter(&orphan, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, windowParentDoneToolBtn)  // H4a
{
    // Arrange: m_doneBtn 的 parent 是 m_wgt(自身窗口)
    // Act
    bool ret = m_wgt->eventFilter(m_wgt->m_doneBtn, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, windowParentScaleToolBtn) // H4b
{
    // Arrange: m_scaleBtn1_1 为 ToolButton 且非 done/cancel
    // Act
    bool ret = m_wgt->eventFilter(m_wgt->m_scaleBtn1_1, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, windowParentNonToolBtn)   // H4c
{
    // Arrange: m_sepLine 为普通 widget (非 ToolButton), parent 是窗口
    // Act
    bool ret = m_wgt->eventFilter(m_wgt->m_sepLine, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, nonWindowParentDoneToolBtn) // H5a
{
    // Arrange: 让 done 按钮的 parent 变为非窗口 (host)
    QWidget host;
    m_wgt->m_doneBtn->setParent(&host);
    // Act
    bool ret = m_wgt->eventFilter(m_wgt->m_doneBtn, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, nonWindowParentScaleToolBtn) // H5b
{
    // Arrange: 让 scale 按钮的 parent 变为非窗口 (host)
    QWidget host;
    m_wgt->m_scaleBtn1_1->setParent(&host);
    // Act
    bool ret = m_wgt->eventFilter(m_wgt->m_scaleBtn1_1, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}

TEST_F(CCutWidgetEventFilterTest, nonWindowParentNonToolBtn) // H5c
{
    // Arrange: m_widthEdit 的 parent 是 m_sizeWidget (非窗口, 非 ToolButton)
    // Act
    bool ret = m_wgt->eventFilter(m_wgt->m_widthEdit, m_ev);
    // Assert
    EXPECT_FALSE(ret);
}
