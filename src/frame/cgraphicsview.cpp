/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cgraphicsview.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/cshapemimedata.h"
#include "bzItems/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
//#include "cundocommands.h"
#include "widgets/ctextedit.h"
#include "bzItems/cgraphicspolygonitem.h"
#include "bzItems/cgraphicspolygonalstaritem.h"
#include "drawshape/cdrawscene.h"
#include "utils/cddfmanager.h"
#include "bzItems/cgraphicsrectitem.h"
#include "bzItems/cgraphicsellipseitem.h"
#include "bzItems/cgraphicslineitem.h"
#include "bzItems/cgraphicstriangleitem.h"
#include "bzItems/cgraphicspolygonitem.h"
#include "bzItems/cgraphicspolygonalstaritem.h"
#include "bzItems/cgraphicstextitem.h"
//#include "bzItems/cgraphicsmasicoitem.h"
#include "bzItems/cgraphicspenitem.h"
#include "bzItems/cpictureitem.h"
#include "drawTools/cpicturetool.h"
#include "drawItems/cgraphicsitemselectedmgr.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "drawTools/cdrawtoolmanagersigleton.h"
#include "application.h"
#include "cundocommands.h"
#include "cundoredocommand.h"
#include "mainwindow.h"
#include "cgraphicsitemevent.h"
#include "progresslayout.h"

#include <QTimer>
#include <DMenu>
#include <DFileDialog>
#include <DDialog>

#include <QAction>
#include <QWheelEvent>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QUndoStack>
#include <QRectF>
#include <QPainter>
#include <QDesktopWidget>
#include <QClipboard>
#include <QMessageBox>
#include <QWindow>
#include <QScreen>
#include <qscrollbar.h>
#include <QTouchEvent>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneWheelEvent>

//升序排列用
//static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
//{
//    return info1->zValue() < info2->zValue();
//}

//水平等间距对齐升序排列
static bool xValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->sceneBoundingRect().left() < info2->sceneBoundingRect().left();
}

//垂直等间距对齐升序排列
static bool yValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->sceneBoundingRect().top() < info2->sceneBoundingRect().top();
}

CGraphicsView::CGraphicsView(DWidget *parent)
    : DGraphicsView(parent)
    , m_scale(1)
    , m_isShowContext(true)
    , m_isStopContinuousDrawing(false)
{
    setOptimizationFlags(IndirectPainting);
    m_pUndoStack = new QUndoStack(this);
    //设置撤销还原最大步数
    m_pUndoStack->setUndoLimit(30);
    CUndoRedoCommand::setUndoRedoStack(m_pUndoStack, this);
    m_DDFManager = new CDDFManager(this);

    initContextMenu();
    initContextMenuConnection();

    //文字右键菜单初始化
    initTextContextMenu();
    initTextContextMenuConnection();

    initConnection();

    viewport()->installEventFilter(this);

    //初始化后设置自身为焦点
    QMetaObject::invokeMethod(this, [ = ]() {
        this->setFocus();
    }, Qt::QueuedConnection);

    this->acceptDrops();
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);

    viewport()->grabGesture(Qt::PinchGesture);


    setSaveDialogMoreOption(QFileDialog::Option(0));
}

void CGraphicsView::zoomOut(EScaleCenter center, const QPoint &viewPos)
{
    qreal current_scale = m_scale;
    if (current_scale >= 2.0 && current_scale <= 20.0) {
        current_scale += 1.0;
    } else if (current_scale >= 1.0 && current_scale <= 1.99) {
        current_scale += 0.1;
    } else if (current_scale >= 0.1 && current_scale < 1.0) {
        current_scale += 0.01;
    }

    if (current_scale >= 20.0) {
        current_scale = 20.0;
    }
    scale(current_scale, center, viewPos);
}

void CGraphicsView::zoomIn(EScaleCenter center, const QPoint &viewPos)
{
    qreal current_scale = m_scale;
    if (current_scale >= 2.0 && current_scale <= 20.0) {
        current_scale -= 1.0;
    } else if (current_scale >= 1.0 && current_scale <= 1.99) {
        current_scale -= 0.1;
    } else if (current_scale >= 0.1 && current_scale < 1.0) {
        current_scale -= 0.01;
    }

    if (current_scale <= 0.1) {
        current_scale = 0.1;
    }
    scale(current_scale, center, viewPos);
}

void CGraphicsView::scale(qreal scale, EScaleCenter center, const QPoint &viewPos)
{
    qreal multiple = scale / m_scale;

    QPoint centerViewPos = viewPos;
    switch (center) {
    case EViewCenter:
        centerViewPos = viewport()->rect().center();
        break;
    case ESceneCenter:
        centerViewPos = mapFromScene(sceneRect().center());
        break;
    case EMousePos:
        centerViewPos = viewport()->mapFromGlobal(QCursor::pos());
        break;
    default:
        centerViewPos = viewPos;
        break;
    }
    scaleWithCenter(multiple, centerViewPos);
}

qreal CGraphicsView::getScale()
{
    return m_scale;
}

void CGraphicsView::scaleWithCenter(qreal factor, const QPoint viewPos)
{
    qreal wantedTotalScaled = m_scale * factor;

    if (wantedTotalScaled < 0.1) {
        wantedTotalScaled = 0.1;
    } else if (wantedTotalScaled > 20) {
        wantedTotalScaled = 20;
    }
    factor = wantedTotalScaled / m_scale;

    //最最最完美的方案！！！
    QPoint centerViewPos = viewPos.isNull() ? viewport()->mapFromGlobal(QCursor::pos()) : viewPos;

    QPointF targetScenePos = mapToScene(centerViewPos);
    ViewportAnchor oldAnchor = this->transformationAnchor();
    setTransformationAnchor(QGraphicsView::NoAnchor);

    QTransform matrix = transform();
    matrix.translate(targetScenePos.x(), targetScenePos.y())
    .scale(factor, factor)
    .translate(-targetScenePos.x(), -targetScenePos.y());

    setTransform(matrix);

    setTransformationAnchor(oldAnchor);

    //保存缩放值
    m_scale *= factor;
    getDrawParam()->setScale(m_scale);
    emit signalSetScale(m_scale);

    if (drawScene() != nullptr)
        drawScene()->selectGroup()->updateBoundingRect();
}

void CGraphicsView::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
    if (activeProxItem() != nullptr) {
        auto proxyItem = activeProxItem();
        QPointF scenePos = this->mapToScene(this->viewport()->mapFromGlobal(QCursor::pos()));
        if (proxyItem->sceneBoundingRect().contains(scenePos)) {
            QPointF p = proxyItem->mapFromScene(scenePos);
            QGraphicsSceneWheelEvent gsEvent(QEvent::GraphicsSceneWheel);
            gsEvent.setPos(p);
            gsEvent.setScenePos(scenePos);
            gsEvent.setScreenPos(QCursor::pos());
            gsEvent.setButtons(event->buttons());
            gsEvent.setModifiers(event->modifiers());
            gsEvent.setDelta(event->delta());
            gsEvent.setOrientation(event->orientation());
            drawScene()->sendEvent(proxyItem, &gsEvent);
            return;
        }
    }
    CGraphicsView *pCurView   = this;
    int            delayValue = event->delta();
    if (pCurView != nullptr) {
        if (event->modifiers() == Qt::NoModifier) {
            //滚动view的垂直scrollbar
            int curValue = pCurView->verticalScrollBar()->value();
            pCurView->verticalScrollBar()->setValue(curValue - delayValue / 12);
        } else if (event->modifiers() == Qt::ShiftModifier) {
            //滚动view的水平scrollbar
            int curValue = pCurView->horizontalScrollBar()->value();
            pCurView->horizontalScrollBar()->setValue(curValue - delayValue / 12);
        } else if (event->modifiers()& Qt::ControlModifier) {
            //如果按住CTRL那么就是放大缩小
            if (event->delta() > 0) {
                pCurView->zoomOut(CGraphicsView::EMousePos);
            } else {
                pCurView->zoomIn(CGraphicsView::EMousePos);
            }
        }
    }
    //QGraphicsView::wheelEvent(event);
}

void CGraphicsView::initContextMenu()
{
    m_contextMenu = new CMenu(this);

    //CMenu enterEvent激活全部action
    m_layerMenu = new DMenu(tr("Layer"), this);

    m_cutAct = new QAction(tr("Cut"), this);
    m_contextMenu->addAction(m_cutAct);
    m_cutAct->setShortcut(QKeySequence::Cut);
    this->addAction(m_cutAct);

    m_copyAct = new QAction(tr("Copy"), this);
    m_contextMenu->addAction(m_copyAct);
    m_copyAct->setShortcut(QKeySequence::Copy);
    this->addAction(m_copyAct);

    m_pasteAct = new QAction(tr("Paste"), this);
    m_pasteActShortCut = new QAction(this);
    m_contextMenu->addAction(m_pasteAct);
    m_pasteActShortCut->setShortcut(QKeySequence::Paste);
    this->addAction(m_pasteAct);
    this->addAction(m_pasteActShortCut);

    m_selectAllAct = new QAction(tr("Select All"), this);
    m_contextMenu->addAction(m_selectAllAct);
    m_selectAllAct->setShortcut(QKeySequence::SelectAll);
    this->addAction(m_selectAllAct);

    m_contextMenu->addSeparator();

    m_deleteAct = new QAction(tr("Delete"), this);
    m_contextMenu->addAction(m_deleteAct);
    m_deleteAct->setShortcut(QKeySequence::Delete);
    this->addAction(m_deleteAct);

    //m_undoAct = m_pUndoStack->createUndoAction(this, tr("Undo"));
    m_undoAct = new QAction(tr("Undo"), this);
    m_undoAct->setEnabled(m_pUndoStack->canUndo());
    connect(m_pUndoStack, SIGNAL(canUndoChanged(bool)),
            m_undoAct, SLOT(setEnabled(bool)));
    m_contextMenu->addAction(m_undoAct);
    m_undoAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));
    this->addAction(m_undoAct);

    //m_redoAct = m_pUndoStack->createRedoAction(this, tr("Redo"));
    m_redoAct = new QAction(tr("Redo"), this);
    m_redoAct->setEnabled(m_pUndoStack->canRedo());
    connect(m_pUndoStack, SIGNAL(canRedoChanged(bool)),
            m_redoAct, SLOT(setEnabled(bool)));
    m_contextMenu->addAction(m_redoAct);
    m_redoAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y));
    this->addAction(m_redoAct);
    m_contextMenu->addSeparator();

    m_oneLayerUpAct = new QAction(tr("Raise Layer"), this);
    m_layerMenu->addAction(m_oneLayerUpAct);
    m_oneLayerUpAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    this->addAction(m_oneLayerUpAct);

    m_oneLayerDownAct = new QAction(tr("Lower Layer"), this);
    m_layerMenu->addAction(m_oneLayerDownAct);
    m_oneLayerDownAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    this->addAction(m_oneLayerDownAct);

    m_bringToFrontAct = new QAction(tr("Layer to Top"), this);
    m_layerMenu->addAction(m_bringToFrontAct);
    m_bringToFrontAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_BracketRight));
    this->addAction(m_bringToFrontAct);

    m_sendTobackAct = new QAction(tr("Layer to Bottom"), this);
    m_layerMenu->addAction(m_sendTobackAct);
    m_sendTobackAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_BracketLeft));
    this->addAction(m_sendTobackAct);

    m_cutScence = new QAction(this);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::Key_Return));
    shortcuts.append(QKeySequence(Qt::Key_Enter));
    m_cutScence->setShortcuts(shortcuts);
    this->addAction(m_cutScence);

    m_viewZoomInAction = new QAction(this);
    m_viewZoomInAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    this->addAction(m_viewZoomInAction);

    m_viewZoomOutAction = new QAction(this);
    m_viewZoomOutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    this->addAction(m_viewZoomOutAction);

    // Qt 无法直接使用 ctrl + (+/=) 这个按键组合
    m_viewZoomOutAction1 = new QAction(this);
    m_viewZoomOutAction1->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal));
    this->addAction(m_viewZoomOutAction1);

    m_viewOriginalAction = new QAction(this);
    m_viewOriginalAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    this->addAction(m_viewOriginalAction);

    m_group = new QAction(tr("Group"), this);
    m_group->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    this->addAction(m_group);
    m_contextMenu->addAction(m_group);

    m_unGroup = new QAction(tr("Ungroup"), this);
    m_unGroup->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_G));
    this->addAction(m_unGroup);
    m_contextMenu->addAction(m_unGroup);

    // 右键菜单添加对齐方式
    m_alignMenu = new DMenu(tr("Align"), this);
    m_contextMenu->addMenu(m_alignMenu);

    m_itemsLeftAlign = new QAction(tr("Align left"), this); //左对齐
    m_itemsLeftAlign->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_L));
    m_alignMenu->addAction(m_itemsLeftAlign);
    this->addAction(m_itemsLeftAlign);

    m_itemsHCenterAlign = new QAction(tr("Horizontal centers"), this); //水平居中对齐
    m_itemsHCenterAlign->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));
    m_alignMenu->addAction(m_itemsHCenterAlign);
    this->addAction(m_itemsHCenterAlign);

    m_itemsRightAlign = new QAction(tr("Align right"), this); //右对齐
    m_itemsRightAlign->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    m_alignMenu->addAction(m_itemsRightAlign);
    this->addAction(m_itemsRightAlign);

    m_itemsTopAlign = new QAction(tr("Align top"), this); //顶对齐
    m_itemsTopAlign->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
    m_alignMenu->addAction(m_itemsTopAlign);
    this->addAction(m_itemsTopAlign);

    m_itemsVCenterAlign = new QAction(tr("Vertical centers"), this); //垂直居中对齐
    m_itemsVCenterAlign->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_V));
    m_alignMenu->addAction(m_itemsVCenterAlign);
    this->addAction(m_itemsVCenterAlign);

    m_itemsBottomAlign = new QAction(tr("Align bottom"), this); //底对齐
    m_itemsBottomAlign->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_B));
    m_alignMenu->addAction(m_itemsBottomAlign);
    this->addAction(m_itemsBottomAlign);

    m_itemsHEqulSpaceAlign = m_alignMenu->addAction(tr("Distribute horizontal space")); //水平等间距对齐
    m_itemsHEqulSpaceAlign->setObjectName("DistributeHorizontalSpace");
    m_itemsVEqulSpaceAlign = m_alignMenu->addAction(tr("Distribute vertical space")); //垂直等间距对齐
    m_itemsVEqulSpaceAlign->setObjectName("DistributeVerticalSpace");

    // 添加对齐菜单
    m_contextMenu->addMenu(m_layerMenu);
}

void CGraphicsView::initContextMenuConnection()
{
    connect(m_cutAct, SIGNAL(triggered()), this, SLOT(slotOnCut()));
    connect(m_copyAct, SIGNAL(triggered()), this, SLOT(slotOnCopy()));
    connect(m_pasteAct, &QAction::triggered, this, [ = ]() {
        slotOnPaste(false);
    });
    connect(m_pasteActShortCut, &QAction::triggered, this, [ = ]() {
        slotOnPaste();
    });
    connect(m_selectAllAct, SIGNAL(triggered()), this, SLOT(slotOnSelectAll()));
    connect(m_deleteAct, SIGNAL(triggered()), this, SLOT(slotOnDelete()));
    connect(m_bringToFrontAct, SIGNAL(triggered()), this, SLOT(slotBringToFront()));
    connect(m_sendTobackAct, SIGNAL(triggered()), this, SLOT(slotSendTobackAct()));
    connect(m_oneLayerUpAct, SIGNAL(triggered()), this, SLOT(slotOneLayerUp()));
    connect(m_oneLayerDownAct, SIGNAL(triggered()), this, SLOT(slotOneLayerDown()));
    connect(m_cutScence, SIGNAL(triggered()), this, SLOT(slotDoCutScene()));

    connect(m_viewZoomInAction, SIGNAL(triggered()), this, SLOT(slotViewZoomIn()));
    connect(m_viewZoomOutAction, SIGNAL(triggered()), this, SLOT(slotViewZoomOut()));
    connect(m_viewZoomOutAction1, SIGNAL(triggered()), this, SLOT(slotViewZoomOut()));
    connect(m_viewOriginalAction, SIGNAL(triggered()), this, SLOT(slotViewOrignal()));

    connect(m_undoAct, &QAction::triggered, this, [ = ] {
        CHECK_MOSUEACTIVE_RETURN
        m_pUndoStack->undo();
        updateCursorShape();
    });
    connect(m_redoAct, &QAction::triggered, this, [ = ] {
        CHECK_MOSUEACTIVE_RETURN
        m_pUndoStack->redo();
        updateCursorShape();
    });

    connect(m_group, &QAction::triggered, this, [ = ] {
        CHECK_CURRENTTOOL_RETURN(this)
        CGraphicsItem *pBaseItem = drawScene()->selectGroup()->getLogicFirst();
        drawScene()->creatGroup(QList<CGraphicsItem *>(), CGraphicsItemGroup::ENormalGroup,
                                true, pBaseItem, true);
    });
    connect(m_unGroup, &QAction::triggered, this, [ = ] {
        CHECK_CURRENTTOOL_RETURN(this)
        drawScene()->cancelGroup(nullptr, true);
    });

    // 连接图元对齐信号
    connect(m_itemsLeftAlign, &QAction::triggered, this, [ = ] {
        this->updateSelectedItemsAlignment(Qt::AlignLeft);
    });
    connect(m_itemsHCenterAlign, &QAction::triggered, this, [ = ] {
        this->updateSelectedItemsAlignment(Qt::AlignHCenter);
    });
    connect(m_itemsRightAlign, &QAction::triggered, this, [ = ] {
        this->updateSelectedItemsAlignment(Qt::AlignRight);
    });
    connect(m_itemsTopAlign, &QAction::triggered, this, [ = ] {
        this->updateSelectedItemsAlignment(Qt::AlignTop);
    });
    connect(m_itemsVCenterAlign, &QAction::triggered, this, [ = ] {
        this->updateSelectedItemsAlignment(Qt::AlignVCenter);
    });
    connect(m_itemsBottomAlign, &QAction::triggered, this, [ = ] {
        this->updateSelectedItemsAlignment(Qt::AlignBottom);
    });
    connect(m_itemsVEqulSpaceAlign, &QAction::triggered, this, [ = ] {
        // [0] 获取选中的图元
        // QList<CGraphicsItem *> allitems = getSelectedValidItems();
        auto currScene = dynamic_cast<CDrawScene *>(scene());
        QList<CGraphicsItem *> allitems = currScene->selectGroup()->items();

        // [1] 图元个数大于3个才可以进行对齐
        if (allitems.size() < 3)
        {
            return ;
        }

        // [2] 对图元X进行从小到大排序
        qSort(allitems.begin(), allitems.end(), yValueSortDES);

        // [3] 统计所有图元占有的高度
        qreal sum_items_height = 0;
        for (int i = 0; i < allitems.size(); i++)
        {
            sum_items_height += allitems.at(i)->sceneBoundingRect().height();
        }

        // [3] 计算每两个之间的间隔距离
        auto curScene = dynamic_cast<CDrawScene *>(scene());
        QRectF scence_BR = curScene->selectGroup()->sceneBoundingRect();

        // [4] 用于记录保存图元的位置，便于撤销和返回
        QMap<CGraphicsItem *, QPointF> startPos;
        QMap<CGraphicsItem *, QPointF> endPos;

        if (sum_items_height > scence_BR.height())
        {
            // [5] 按照相邻进行移动位置
            for (int i = 1; i < allitems.size(); i++) {
                startPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
                QPointF endPoint(allitems.at(i)->sceneBoundingRect().left()
                                 , allitems.at(i - 1)->sceneBoundingRect().bottom());
                allitems.at(i)->move(allitems.at(i)->sceneBoundingRect().topLeft(), endPoint);
                endPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
            }
        } else
        {
            qreal space_height = (scence_BR.height() - sum_items_height)
                                 / (allitems.size() - 1);

            // [6] 按照y值进行移动位置
            for (int i = 1; i < allitems.size() - 1; i++) {
                startPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
                QPointF endPoint(allitems.at(i)->sceneBoundingRect().left()
                                 , allitems.at(i - 1)->sceneBoundingRect().bottom());
                endPoint = endPoint + QPointF(0, space_height);
                allitems.at(i)->move(allitems.at(i)->sceneBoundingRect().topLeft(), endPoint);
                endPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
            }
        }

        // [7] 设置出入栈
        QUndoCommand *addCommand = new CItemsAlignCommand(static_cast<CDrawScene *>(scene()), startPos, endPos);
        pushUndoStack(addCommand);
    });

    connect(m_itemsHEqulSpaceAlign, &QAction::triggered, this, [ = ] {
        // [0] 获取选中的图元
        // QList<CGraphicsItem *> allitems = getSelectedValidItems();
        auto currScene = dynamic_cast<CDrawScene *>(scene());
        QList<CGraphicsItem *> allitems = currScene->selectGroup()->items();

        // [1] 图元个数大于3个才可以进行对齐
        if (allitems.size() < 3)
        {
            return ;
        }

        // [2] 对图元X进行从小到大排序
        qSort(allitems.begin(), allitems.end(), xValueSortDES);

        // [3] 统计所有图元占有的宽度
        qreal sum_items_width = 0;
        for (int i = 0; i < allitems.size(); i++)
        {
            sum_items_width += allitems.at(i)->sceneBoundingRect().width();
        }

        // [4] 计算每两个之间的间隔距离
        auto curScene = dynamic_cast<CDrawScene *>(scene());
        QRectF scence_BR = curScene->selectGroup()->sceneBoundingRect();

        // [5] 用于记录保存图元的位置，便于撤销和返回
        QMap<CGraphicsItem *, QPointF> startPos;
        QMap<CGraphicsItem *, QPointF> endPos;

        if (sum_items_width > scence_BR.width())
        {
            // [6] 按照相邻进行移动位置
            for (int i = 1; i < allitems.size(); i++) {
                startPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
                QPointF endPoint(allitems.at(i - 1)->sceneBoundingRect().right()
                                 , allitems.at(i)->sceneBoundingRect().top());
                allitems.at(i)->move(allitems.at(i)->sceneBoundingRect().topLeft(), endPoint);
                endPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
            }
        } else
        {
            qreal space_width = (scence_BR.width() - sum_items_width)
                                / (allitems.size() - 1);
            // [7] 按照x值的间隔进行移动位置
            for (int i = 1; i < allitems.size() - 1; i++) {
                startPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
                QPointF endPoint(allitems.at(i - 1)->sceneBoundingRect().right()
                                 , allitems.at(i)->sceneBoundingRect().top());
                endPoint = endPoint + QPointF(space_width, 0);
                allitems.at(i)->move(allitems.at(i)->sceneBoundingRect().topLeft(), endPoint);
                endPos.insert(allitems.at(i), allitems.at(i)->sceneBoundingRect().topLeft());
            }
        }

        // [8] 设置出入栈
        QUndoCommand *addCommand = new CItemsAlignCommand(static_cast<CDrawScene *>(scene()), startPos, endPos);
        pushUndoStack(addCommand);
    });

    // 连接鼠标hovered子菜单刷新图层状态
    connect(m_contextMenu, &QMenu::hovered, this, [ = ](QAction * action) {
        if (action->text() == tr("Layer") && m_layerMenu->isHidden()) {
            // 此处由于图层菜单显示较慢，以下代码会重复执行几次，待后期进一步优化
//            bool layerUp = canLayerUp();
//            m_oneLayerUpAct->setEnabled(layerUp);
//            m_bringToFrontAct->setEnabled(layerUp);

//            bool layerDown = canLayerDown();
//            m_oneLayerDownAct->setEnabled(layerDown);
//            m_sendTobackAct->setEnabled(layerDown);
        }
    });
}

void CGraphicsView::initTextContextMenu()
{
    m_textMenu = new DMenu(this);

    m_textCutAction = new QAction(tr("Cut"), m_textMenu);
    m_textCopyAction = new QAction(tr("Copy"), m_textMenu);
    m_textPasteAction = new QAction(tr("Paste"), m_textMenu);
    m_textSelectAllAction = new QAction(tr("Select All"), m_textMenu);
    m_textUndoAct = new QAction(tr("Undo"), m_textMenu);
    m_textRedoAct = new QAction(tr("Redo"), m_textMenu);
    m_textLeftAlignAct = new QAction(tr("Text Align Left"), m_textMenu);                 // 左对齐
    m_textRightAlignAct = new QAction(tr("Text Align Right"), m_textMenu);            // 右对齐
    m_textCenterAlignAct = new QAction(tr("Text Align Center"), m_textMenu);     //  水平垂直居中对齐
    m_textDeleteAction = new QAction(tr("Delete"), m_textMenu);

    m_textMenu->addAction(m_textCutAction);
    m_textMenu->addAction(m_textCopyAction);
    m_textMenu->addAction(m_textPasteAction);
    m_textMenu->addAction(m_textSelectAllAction);
    m_textMenu->addAction(m_textDeleteAction);
    m_textMenu->addSeparator();
    m_textMenu->addAction(m_textUndoAct);
    m_textMenu->addAction(m_textRedoAct);
    m_textMenu->addSeparator();

    m_textMenu->addAction(m_textLeftAlignAct);
    m_textMenu->addAction(m_textRightAlignAct);
    m_textMenu->addAction(m_textCenterAlignAct);
}

void CGraphicsView::initTextContextMenuConnection()
{
    connect(m_textCutAction, SIGNAL(triggered()), this, SLOT(slotOnTextCut()));
    connect(m_textCopyAction, SIGNAL(triggered()), this, SLOT(slotOnTextCopy()));
    connect(m_textPasteAction, SIGNAL(triggered()), this, SLOT(slotOnTextPaste()));
    connect(m_textSelectAllAction, SIGNAL(triggered()), this, SLOT(slotOnTextSelectAll()));
    connect(m_textUndoAct, SIGNAL(triggered()), this, SLOT(slotOnTextUndo()));
    connect(m_textRedoAct, SIGNAL(triggered()), this, SLOT(slotOnTextRedo()));
    connect(m_textDeleteAction, SIGNAL(triggered()), this, SLOT(slotOnTextDelete()));

    //  设置文字图元内部对齐方式
    connect(m_textLeftAlignAct, &QAction::triggered, this, [ = ]() {
        slotSetTextAlignment(Qt::AlignLeft);
    });
    connect(m_textRightAlignAct, &QAction::triggered, this, [ = ]() {
        slotSetTextAlignment(Qt::AlignRight);
    });
    connect(m_textCenterAlignAct, &QAction::triggered, this, [ = ]() {
        slotSetTextAlignment(Qt::AlignCenter);
    });
}

void CGraphicsView::initConnection()
{
    qRegisterMetaType<SGraphicsTextUnitData>("SGraphicsTextUnitData");
    qRegisterMetaType<SGraphicsUnitHead>("SGraphicsUnitHead");
    qRegisterMetaType<CGraphicsUnit>("CGraphicsUnit&");
    connect(m_DDFManager, SIGNAL(signalClearSceneBeforLoadDDF()), this, SLOT(clearScene()));
    connect(m_DDFManager, SIGNAL(signalStartLoadDDF(QRectF)), this, SLOT(slotStartLoadDDF(QRectF)));
    connect(m_DDFManager, SIGNAL(signalAddItem(QGraphicsItem *, bool)), this, SLOT(slotAddItemFromDDF(QGraphicsItem *, bool)));
    connect(m_DDFManager, &CDDFManager::signalAddTextItem, this, [ = ](CGraphicsUnit & data, bool pushToStack) {
        CGraphicsTextItem *item = new CGraphicsTextItem;
        item->loadGraphicsUnit(data);
        slotAddItemFromDDF(item, pushToStack);
        data.release();
    });
    connect(m_DDFManager, &CDDFManager::signalSaveFileFinished, this, &CGraphicsView::signalSaveFileStatus);
    connect(m_DDFManager, &CDDFManager::singalEndLoadDDF, this, [ = ]() {
        m_loadFromDDF.clear();
        emit singalTransmitEndLoadDDF();
    });

    // 连接undo redo改变的信号
    connect(m_pUndoStack, &QUndoStack::canUndoChanged, this, [ = ](bool undo) {
        this->setModify(undo);
    });
}

void CGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    CHECK_CURRENTTOOL_RETURN(this)
    QPointF pos = this->mapToScene(event->pos());
    QRectF rect = this->scene()->sceneRect();

    enum {EShowTxtMenu, EShowCommonMenu};
    int menuTp = EShowCommonMenu;

    // 0.规避显示条件
    if (!rect.contains(pos) || !m_isShowContext) {
        return;
    }

    // 1.判断确定要显示的菜单
    auto pProxDrawItem = activeProxDrawItem();
    CGraphicsItem *tmpitem = pProxDrawItem;
    QList<CGraphicsItem *> selectItems = drawScene()->selectGroup()->items();
    QMenu *pMenu = m_contextMenu;
    if (selectItems.count() > 0) {
        if (pProxDrawItem != nullptr && pProxDrawItem->isSelected()) {
            // 1.1 显示文字图元右键菜单
            if (TextType == pProxDrawItem->type() &&  static_cast<CGraphicsTextItem *>(tmpitem)->isEditState()) {
                menuTp = EShowTxtMenu;
                pMenu = m_textMenu;
            }
        }
    }
    // 2.设置菜单中action的状态
    if (menuTp == EShowTxtMenu) {

        // 2.1 文本图元对齐显示状态
        setTextAlignMenuActionStatus(tmpitem);

    } else {
        // 2.3 一般图元的cut，copy，delete，paste操作的状态
        setCcdpMenuActionStatus(selectItems.count() > 0);

        m_selectAllAct->setEnabled(drawScene()->getBzItems().count() > 0);
        m_group->setEnabled(drawScene()->isGroupable());
        m_unGroup->setEnabled(drawScene()->isUnGroupable());

        // 2.4 设置选择的图元个数显示能进行的对齐状态
        setAlignMenuActionStatus(selectItems.count() > 0);

        // 2.5 设置右键菜单图层选项状态
        setLayerMenuActionStatus(selectItems.count() > 0);
    }

    //判断剪切板状态是否可粘贴
    setClipboardStatus();

    //显示菜单
    showMenu(pMenu);

    DGraphicsView::contextMenuEvent(event);
}

void CGraphicsView::showMenu(DMenu *pMenu)
{
    //判断当前状态是否屏蔽右键菜单
    //CHECK_CURRENTTOOL_RETURN(this)

    QPoint curPos = QCursor::pos();
    //保存当前鼠标位置为文字粘贴的位置
    letfMenuPopPos = this->mapToScene(this->viewport()->mapFromGlobal(QCursor::pos()));

    QSize menSz = pMenu->sizeHint();

    QRect menuRect = QRect(curPos, menSz);

    QScreen *pCurScren = drawApp->topMainWindow()->windowHandle()->screen();

    if (pCurScren != nullptr) {
        QRect geomeRect = pCurScren->geometry();
        if (!geomeRect.contains(menuRect)) {
            if (menuRect.right() > geomeRect.right()) {
                int move = menuRect.right() - geomeRect.right();
                menuRect.adjust(-move, 0, -move, 0);
            }

            if (menuRect.bottom() > geomeRect.bottom()) {
                int move = menuRect.bottom() - geomeRect.bottom();
                menuRect.adjust(0, -move, 0, -move);
            }
        }
    }

    pMenu->move(menuRect.topLeft());

    pMenu->show();
}


void CGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
}

void CGraphicsView::paintEvent(QPaintEvent *event)
{
    if (!_cacheEnable)
        DGraphicsView::paintEvent(event);
    else {

        QPainter painter(this->viewport());

        painter.drawPixmap(QPoint(0, 0), _cachePixmap);

        //绘制缓冲时的额外的前景显示
        EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);

        if (pTool != nullptr) {
            pTool->drawMore(&painter, mapToScene(QRect(QPoint(0, 0), _cachePixmap.size())).boundingRect(), drawScene());
        }
    }
}

void CGraphicsView::drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[])
{
    QRectF rect = scene()->sceneRect();
    painter->setClipping(true);
    painter->setClipRect(rect);
    DGraphicsView::drawItems(painter, numItems, items, options);
}

void CGraphicsView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    drawApp->setApplicationCursor(Qt::ArrowCursor);
}

//void CGraphicsView::itemAdded(QGraphicsItem *item, bool pushToStack)
//{
//    auto curScene = dynamic_cast<CDrawScene *>(scene());
//    QList<QGraphicsItem *> addItems;
//    addItems.clear();
//    addItems.append(item);
//    item->setZValue(curScene->getMaxZValue() + 1);
//    curScene->setMaxZValue(curScene->getMaxZValue() + 1);
//    if (pushToStack) {
//        //QUndoCommand *addCommand = new CAddShapeCommand(curScene, addItems);
//        //this->pushUndoStack(addCommand);
//    }
//}

void CGraphicsView::slotStartLoadDDF(QRectF rect)
{
    scene()->setSceneRect(rect);
}

void CGraphicsView::slotAddItemFromDDF(QGraphicsItem *item, bool pushToStack)
{
    Q_UNUSED(pushToStack)
    if (drawScene() != nullptr && drawScene()->isBussizeItem(item)) {
        //从ddf还原 不需要自动设置z值
        //drawScene()->addCItem(item);
        drawScene()->addItem(item);
        m_loadFromDDF.append(item);
    }
}

void CGraphicsView::slotOnCut()
{
    CHECK_CURRENTTOOL_RETURN(this)

    //1.将数据复制到粘贴板
    slotOnCopy();

    //2.删除当前选中的图元(直接调用删除响应槽即可)
    slotOnDelete();
}

void CGraphicsView::slotOnCopy()
{
    CHECK_CURRENTTOOL_RETURN(this)
    CShapeMimeData *data = new CShapeMimeData(drawScene()->getGroupTreeInfo(drawScene()->selectGroup()));
    data->setText("");
    QApplication::clipboard()->setMimeData(data);
    m_pasteAct->setEnabled(true);
}

void CGraphicsView::slotOnPaste(bool textItemInCenter)
{
    CHECK_MOSUEACTIVE_RETURN
    QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());

    if (mp->hasImage()) {
        // 粘贴图片弹窗提示
        getProgressLayout()->setRange(0, 1);
        getProgressLayout()->setProgressValue(0);
        getProgressLayout()->show();
        getProgressLayout()->raise();

        QTimer::singleShot(100, nullptr, [ = ] {
            //粘贴剪切板中的图片
            QVariant imageData = mp->imageData();
            QPixmap pixmap = imageData.value<QPixmap>();

            qDebug() << "entered mp->hasImage()"  << endl;
            if (!pixmap.isNull())
            {
                emit signalPastePixmap(pixmap, QByteArray());
            }

            getProgressLayout()->hide();
        });

    } else if (mp->hasText()) {

        QString filePath = mp->text();
        // [0] 验证正确的图片路径
        Application *pApp = drawApp;
        if (pApp != nullptr) {
            QStringList pathlist = filePath.split("\n");
            bool rightPath = true;
            for (int i = 0; i < pathlist.size() ; i++) {
                if (pApp->isFileExist(pathlist[i])) {
                    emit signalLoadDragOrPasteFile(pathlist[i]);
                } else {
                    rightPath = false;
                    break;
                }
            }

            if (!rightPath) {
                // clean selection
                this->drawScene()->clearSelectGroup();

                // add text item
                CGraphicsItem *item = CGraphicsItem::creatItemInstance(TextType);
                if (item) {
                    CGraphicsTextItem *textItem = static_cast<CGraphicsTextItem *>(item);
                    if (textItem) {
                        IDrawTool::setViewToSelectionTool(this);
                        textItem->textEditor()->setPlainText(filePath);
                        textItem->updateTextFormat();
                        QList<QVariant> vars;
                        vars << reinterpret_cast<long long>(scene());
                        vars << reinterpret_cast<long long>(item);
                        drawScene()->addItem(item);
                        if (textItemInCenter) {
                            item->setPos(this->mapToScene(viewport()->rect().center()) - QPointF(item->boundingRect().width(), item->boundingRect().height()) / 2);
                        } else {
                            item->setPos(letfMenuPopPos);
                        }

                        qreal newZ = this->drawScene()->getMaxZValue() + 1;
                        item->setZValue(newZ);
                        //this->drawScene()->setMaxZValue(newZ);
                        CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                            CSceneUndoRedoCommand::EItemAdded, vars, true, true);
                        CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                            CSceneUndoRedoCommand::EItemAdded, vars);
                        CUndoRedoCommand::finishRecord();
                        drawScene()->selectItem(item, true, true, true);
                        textItem->setTextState(CGraphicsTextItem::EInEdit, false)/*changToEditState(false)*/;

                        //粘贴板复制进来的文字，设置焦点在文本最后，方便继续编辑
                        QTextCursor cursor = textItem->textEditor()->textCursor();
                        cursor.movePosition(QTextCursor::End);
                        textItem->textEditor()->setTextCursor(cursor);
                    }
                }
            }
        }
    } else {
        qDebug() << "mp->hasImage()"  << mp->hasImage() << endl;

        //粘贴画板内部图元
        CShapeMimeData *data = qobject_cast<CShapeMimeData *>(mp);
        if (data != nullptr) {
            drawScene()->clearSelectGroup();

            //0.复制前记录当前场景的组合快照(用于撤销)
            drawScene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, UndoVar);

            //1.复制生成新的图元并添加到场景中去
            CGraphicsItemGroup *pGroup = drawScene()->loadGroupTreeInfo(data->itemsTreeInfo(), true);

            QList<CGraphicsItem *> needSelected;
            if (pGroup != nullptr) {
                needSelected = pGroup->items();
                QList<CGraphicsItem *> allItems = pGroup->getBzItems(true);

                //2.调整复制生成图元的位置,并记录他们添加到场景动作用于撤销还原
                QList<QVariant> vars;
                vars << reinterpret_cast<long long>(scene());
                foreach (CGraphicsItem *item, allItems) {
                    vars << reinterpret_cast<long long>(item);
                    //item->moveBy(10, 10);
                }
                CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                    CSceneUndoRedoCommand::EItemAdded, vars, false, true);

                if (pGroup->groupType() == CGraphicsItemGroup::EVirRootGroup) {
                    drawScene()->cancelGroup(pGroup);
                }
            }

            //3.复制后记录当前场景的组合快照(用于还原)
            drawScene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, RedoVar);

            //4.撤销还原入栈
            CUndoRedoCommand::finishRecord();

            for (auto p : needSelected) {
                drawScene()->selectItem(p, true, false, false);

                CGraphItemMoveEvent event(CGraphItemEvent::EMove);
                event.setEventPhase(EChanged);
                event._oldScenePos = p->scenePos();
                event._scenePos = event._oldScenePos + QPointF(10, 10);
                p->operating(&event);
            }
            drawScene()->selectGroup()->updateBoundingRect();
            drawScene()->selectGroup()->updateAttributes();
        }
    }
    //5.粘贴数据进入画板,鼠标设置为选择工具
    //CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
    drawApp->setViewCurrentTool(this, selection);
}

void CGraphicsView::slotOnSelectAll()
{
    CHECK_CURRENTTOOL_RETURN(this)
    drawScene()->selectItemsByRect(sceneRect());
}

void CGraphicsView::slotOnDelete()
{
    CHECK_CURRENTTOOL_RETURN(this)
    // 得到要被删除的基本业务图元
    QList<CGraphicsItem *> allItems = drawScene()->selectGroup()->items(true);

    // 如果当前有多选图元并且当前不是连笔绘制图元才执行删除
    if (allItems.count() > 0 && getDrawParam()->getCurrentDrawToolMode() != EDrawToolMode::pen) {

        //获取到涉及到的组合图元(删除基本业务图元前要先取消这些组合)
        QList<CGraphicsItemGroup *> groups = drawScene()->selectGroup()->getGroups(true);

        QList<QVariant> vars;
        vars << reinterpret_cast<long long>(scene());
        for (CGraphicsItem *pItem : allItems) {
            if (pItem->isBzItem()) {
                vars << reinterpret_cast<long long>(pItem);
            }
        }
        //保证在删除之前执行保存当前组合的快照情况
        drawScene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, UndoVar);


        //删除放在保存当前组合的快照情况的后面,才能保证重做时先添加回来图元再还原组合快照情况
        CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                            CSceneUndoRedoCommand::EItemRemoved, vars, false, true);


        //删除基本图元前先清除他们涉及到的组合
        for (auto gp : groups)
            drawScene()->cancelGroup(gp);

        //执行删除操作
        for (CGraphicsItem *pItem : allItems) {
            drawScene()->removeCItem(pItem);
        }

        //记录在删除之后的组合的快照情况,用于重做
        drawScene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, RedoVar);


        //结束改次撤销还原信息的收集,生成undoredo对象以响应之后的撤销还原操作
        CUndoRedoCommand::finishRecord();

        //刷新多选图元
        drawScene()->selectGroup()->updateBoundingRect();

        drawScene()->refreshLook();
    }
}

void CGraphicsView::slotOneLayerUp()
{
    CHECK_CURRENTTOOL_RETURN(this)
    drawScene()->moveBzItemsLayer(drawScene()->selectGroup()->items(), EUpLayer, 1, nullptr, true);
}

void CGraphicsView::slotOneLayerDown()
{
    CHECK_CURRENTTOOL_RETURN(this)
    drawScene()->moveBzItemsLayer(drawScene()->selectGroup()->items(), EDownLayer, 1, nullptr, true);
}

void CGraphicsView::slotBringToFront()
{
    CHECK_CURRENTTOOL_RETURN(this)
    drawScene()->moveBzItemsLayer(drawScene()->selectGroup()->items(), EUpLayer, -1, nullptr, true);
}

void CGraphicsView::slotSendTobackAct()
{
    CHECK_CURRENTTOOL_RETURN(this)
    drawScene()->moveBzItemsLayer(drawScene()->selectGroup()->items(), EDownLayer, -1, nullptr, true);
}

void CGraphicsView::slotQuitCutMode()
{
    static_cast<CDrawScene *>(scene())->quitCutMode();
}

void CGraphicsView::slotDoCutScene()
{
    // [42259] 解决处于裁剪的时候编辑输入框裁剪大小回车不响应输入框，因为view设置了全局的快捷键

    if (getDrawParam()->getCurrentDrawToolMode() == cut) {
        QLineEdit *foucsLIneedit = qobject_cast<QLineEdit *>(dApp->focusObject());
        if (foucsLIneedit != nullptr) {
            m_cutScence->setEnabled(false);
            QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, dApp->keyboardModifiers());
            dApp->sendEvent(foucsLIneedit, &event);
            m_cutScence->setEnabled(true);
        } else {
            static_cast<CDrawScene *>(scene())->doCutScene();
            this->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::selection);
        }
    } else {
        if (dApp->focusObject() != nullptr) {
            m_cutScence->setEnabled(false);
            QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, dApp->keyboardModifiers());
            dApp->sendEvent(dApp->focusObject(), &event);
            m_cutScence->setEnabled(true);
        }
    }
}

void CGraphicsView::slotRestContextMenuAfterQuitCut()
{
    setContextMenuAndActionEnable(true);
}

void CGraphicsView::slotViewZoomIn()
{
    CHECK_MOSUEACTIVE_RETURN
    zoomIn(EViewCenter);
}

void CGraphicsView::slotViewZoomOut()
{
    CHECK_MOSUEACTIVE_RETURN
    zoomOut(EViewCenter);
}

void CGraphicsView::slotViewOrignal()
{
    CHECK_MOSUEACTIVE_RETURN
    this->scale(1.0);
    emit signalSetScale(m_scale);
}

void CGraphicsView::slotOnTextCut()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doCut();
        }
    }
}

void CGraphicsView::slotOnTextCopy()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doCopy();
        }
    }
}

void CGraphicsView::slotOnTextPaste()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doPaste();
        }
    }
}

void CGraphicsView::slotOnTextSelectAll()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doSelectAll();
        }
    }
}

void CGraphicsView::slotSetTextAlignment(const Qt::Alignment &align)
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->setSelectTextBlockAlign(align);
        }
    }
}

void CGraphicsView::slotOnTextUndo()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doUndo();
        }
    }
}

void CGraphicsView::slotOnTextRedo()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doRedo();
        }
    }
}

void CGraphicsView::slotOnTextDelete()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditState()) {
            tmpitem->doDelete();
        }
    }
}

void CGraphicsView::clearScene()
{
    ///清除场景选中
    scene()->clearSelection();

    //清空撤销栈
    m_pUndoStack->clear();

    //清空场景
    //scene()->clear();

    auto curScene = static_cast<CDrawScene *>(scene());

    curScene->resetScene();
}

void CGraphicsView::itemSceneCut(QRectF newRect)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *sceneCutCommand = new CSceneCutCommand(curScene, newRect);
    this->pushUndoStack(sceneCutCommand);
}

void CGraphicsView::doSaveDDF(bool finishClose)
{
    QString ddfPath = getDrawParam()->getDdfSavePath();
    QFileInfo fInfo(ddfPath);
    if (ddfPath.isEmpty() || ddfPath == "" || !fInfo.exists()) {
        showSaveDDFDialog(true, finishClose);
    } else if (!fInfo.isWritable()) {
        //如果文件不可写入那么先弹出提示对话框然后再由用户决定是否要另存为或取消
        DDialog dia(this);
        dia.setFixedSize(404, 163);
        dia.setModal(true);
        dia.setMessage(tr("This file is read-only, please save with another name")/*.arg(QFileInfo(ddfPath).fileName())*/);
        dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

        int yes  = dia.addButton(tr("OK"), false, DDialog::ButtonNormal);
        //dia.addButton(tr("No"), true, DDialog::ButtonNormal);
        int ret = dia.exec();

        if (ret == yes) {
            //弹出保存文件的框
            QString newBaseName = tr("Unnamed");
            QString newFile = fInfo.absolutePath() + "/" + newBaseName + "." + fInfo.suffix();
            showSaveDDFDialog(true, finishClose, newFile);

        } else {
            //取消保存
            //do nothing
            if (finishClose) {
                emit signalSaveFileStatus(ddfPath, false, "file is not writable", QFileDevice::WriteError, true);
            }
        }

    } else {
        m_DDFManager->saveToDDF(ddfPath, scene(), finishClose);
        // 保存是否成功均等待信号触发后续事件
    }
}

void CGraphicsView::showSaveDDFDialog(bool type, bool finishClose, const QString &saveFilePath)
{
    // 保存为静态变量的目的是为了单元测试，实际上是因为DFileDialog内部自身的问题
    // 调用done后，DFileDialog没有exec返回
    /*static*/ DFileDialog dialog(this);
    dialog.setObjectName("DDFSaveDialog");
    if (type) {
        dialog.setWindowTitle(tr("Save"));
    } else {
        dialog.setWindowTitle(tr("Save as"));
    }//设置文件保存对话框的标题
    dialog.setAcceptMode(QFileDialog::AcceptSave);//设置文件对话框为保存模式
    dialog.setOptions(QFileDialog::DontResolveSymlinks | QFileDialog::Option(_moreOpForSaveDialog)); //只显示文件夹
    dialog.setViewMode(DFileDialog::List);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    //dialog.selectFile(tr("Unnamed.ddf"));//设置默认的文件名
    dialog.selectFile(saveFilePath.isEmpty() ? (getDrawParam()->viewName() + ".ddf") : QFileInfo(saveFilePath).fileName()); //设置默认的文件名
    QStringList nameFilters;
    nameFilters << "*.ddf";
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    if (dialog.exec()) {
        QString path = dialog.selectedFiles().first();
        if (!path.isEmpty()) {
            if (!drawApp->isFileNameLegal(path)) {
                //不支持的文件名
                DDialog dia(this);
                dia.setObjectName("ErrorNameDialog");

                dia.setFixedSize(404, 163);

                dia.setModal(true);
                dia.setMessage(tr("The file name must not contain \\/:*?\"<>|"));
                dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

                int OK = dia.addButton(tr("OK"), false, DDialog::ButtonNormal);

                int result = dia.exec();

                if (OK == result) {
                    QMetaObject::invokeMethod(this, [ = ]() {
                        showSaveDDFDialog(type, finishClose, saveFilePath);
                    },
                    Qt::QueuedConnection);
                }
                return;
            }

            if (path.split("/").last() == ".ddf" || QFileInfo(path).suffix().toLower() != ("ddf")) {
                path = path + ".ddf";
            }

            //再判断该文件是否正在被打开着的如果是那么就要提示不能覆盖
            if (CManageViewSigleton::GetInstance()->isDdfFileOpened(path)) {
                DDialog dia(this);
                dia.setObjectName("OpenedDialog");

                dia.setFixedSize(404, 183);

                dia.setModal(true);
                dia.setMessage(tr("Cannot save it as %1, since the file in that name is open now."
                                  "\nPlease save it in another name or close that file and try again.")
                               .arg(QFileInfo(path).fileName()));
                dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

                dia.addButton(tr("OK"), false, DDialog::ButtonNormal);

                dia.exec();

                return;
            }


            m_DDFManager->saveToDDF(path, scene(), finishClose);
            // 保存是否成功均等待信号触发后续事件
        }
    }
}

void CGraphicsView::setSaveDialogMoreOption(QFileDialog::Option op)
{
    _moreOpForSaveDialog = op;
}

bool CGraphicsView::importData(const QString &path, bool isOpenByDDF)
{
    return m_DDFManager->loadDDF(path, isOpenByDDF);
}

void CGraphicsView::disableCutShortcut(bool isFocus)
{
    m_cutScence->setEnabled(!isFocus);
    //    qDebug() << "m_cutScence->isEnabled()=" << m_cutScence->isEnabled();
}

CDrawParamSigleton *CGraphicsView::getDrawParam()
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    return curScene->getDrawParam();
}

void CGraphicsView::pushUndoStack(QUndoCommand *cmd)
{
    m_pUndoStack->beginMacro("");
    m_pUndoStack->push(cmd);
    m_pUndoStack->endMacro();
}

bool CGraphicsView::isModified() const
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    return curScene->isModified();
}

void CGraphicsView::setModify(bool isModify)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    curScene->setModify(isModify);
}

bool CGraphicsView::isModifyStashEmpty()
{
    return (!m_pUndoStack->canRedo() && !m_pUndoStack->canUndo());
}

bool CGraphicsView::isKeySpacePressed()
{
    return _spaceKeyPressed;
}
CDrawScene *CGraphicsView::drawScene()
{
    return dynamic_cast<CDrawScene *>(scene());
}

void CGraphicsView::updateCursorShape()
{
    drawScene()->refreshLook();
}

void CGraphicsView::setCacheEnable(bool enable, bool fruzzCurFrame)
{
    _cacheEnable = enable;
    if (_cacheEnable) {
        _cachePixmap = QPixmap(this->viewport()->size() * devicePixelRatioF());
        _cachePixmap.setDevicePixelRatio(devicePixelRatioF());
        _cachePixmap.fill(QColor(0, 0, 0, 0));


        if (fruzzCurFrame) {
            QPainter painter(&_cachePixmap);
            painter.setPen(Qt::NoPen);
            painter.setRenderHint(QPainter::Antialiasing);
            this->scene()->render(&painter, QRectF(0, 0, _cachePixmap.width(), _cachePixmap.height()),
                                  QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(_cachePixmap.size().width(), _cachePixmap.size().height()))), Qt::IgnoreAspectRatio);

            painter.drawRect(viewport()->rect());
        }
    }
    viewport()->update();
}

bool CGraphicsView::isCacheEnabled()
{
    return _cacheEnable;
}

QPixmap &CGraphicsView::cachedPixmap()
{
    return _cachePixmap;
}

QWidget *CGraphicsView::activeProxWidget()
{
    if (activeProxItem() != nullptr)
        return activeProxItem()->widget();
    return nullptr;
}

QGraphicsProxyWidget *CGraphicsView::activeProxItem()
{
    if (scene() == nullptr || scene()->focusItem() == nullptr)
        return nullptr;

    if (scene()->focusItem()->type() == QGraphicsProxyWidget::Type) {
        auto pProxyIt = static_cast<QGraphicsProxyWidget *>(scene()->focusItem());
        if (pProxyIt->flags()&QGraphicsItem::ItemHasNoContents) {
            return nullptr;
        }
        return pProxyIt;
    }
    return nullptr;
}

CGraphicsItem *CGraphicsView::activeProxDrawItem()
{
    if (activeProxItem() != nullptr) {
        return dynamic_cast<CGraphicsItem *>(activeProxItem()->parentItem());
    }
    return nullptr;
}

void CGraphicsView::showEvent(QShowEvent *event)
{
    this->setTransformationAnchor(AnchorViewCenter);
    QGraphicsView::showEvent(event);
}

void CGraphicsView::updateSelectedItemsAlignment(Qt::AlignmentFlag align)
{
    CHECK_CURRENTTOOL_RETURN(this)

    // 获取选择的组合中全部图元
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> allItems = curScene->selectGroup()->items();

    // 模拟图元移动事件
    CGraphItemMoveEvent event(CGraphItemEvent::EMove);
    event.setEventPhase(EChanged);
    // 移动的起点相对位置
    event._oldScenePos = QPointF(0, 0);

    // [0] 没有选中的图元直接返回
    if (!allItems.size()) {
        return;
    }

    // [1] 用于记录保存图元的位置，便于撤销和返回
    QMap<CGraphicsItem *, QPointF> startPos;
    QMap<CGraphicsItem *, QPointF> endPos;

    // 获取多个图元和单个图元的边界值
    QRectF currSceneRect;
    if (allItems.size() > 1) {
        currSceneRect = curScene->selectGroup()->mapRectToScene(curScene->selectGroup()->boundingRectTruly());
    } else {
        currSceneRect = sceneRect();
    }

    // [1] 设置对齐坐标
    for (int i = 0; i < allItems.size(); i++) {
        QRectF itemRect = allItems.at(i)->mapRectToScene(allItems.at(i)->boundingRectTruly());

        startPos.insert(allItems.at(i), allItems.at(i)->sceneBoundingRect().topLeft());

        switch (align) {
        case Qt::AlignLeft: {
            qreal dx = alignmentMovPos(currSceneRect, itemRect, align);
            event._scenePos = QPointF(-dx, 0);
            allItems.at(i)->operating(&event);
            break;
        }
        case Qt::AlignHCenter: {
            qreal dy = alignmentMovPos(currSceneRect, itemRect, align);
            event._scenePos = QPointF(0, dy);
            allItems.at(i)->operating(&event);
            break;
        }
        case Qt::AlignRight: {
            qreal dx = alignmentMovPos(currSceneRect, itemRect, align);
            event._scenePos = QPointF(dx, 0);
            allItems.at(i)->operating(&event);
            break;
        }
        case Qt::AlignTop: {
            qreal dy = alignmentMovPos(currSceneRect, itemRect, align);
            event._scenePos = QPointF(0, -dy);
            allItems.at(i)->operating(&event);
            break;
        }
        case Qt::AlignVCenter: {
            qreal dx = alignmentMovPos(currSceneRect, itemRect, align);
            event._scenePos = QPointF(dx, 0);
            allItems.at(i)->operating(&event);
            break;
        }
        case Qt::AlignBottom: {
            qreal dy = alignmentMovPos(currSceneRect, itemRect, align);
            event._scenePos = QPointF(0, dy);
            allItems.at(i)->operating(&event);
            break;
        }
        default: {
            break;
        }

        }

        endPos.insert(allItems.at(i), allItems.at(i)->sceneBoundingRect().topLeft());
    }


    // [4] 设置出入栈
    QUndoCommand *addCommand = new CItemsAlignCommand(static_cast<CDrawScene *>(scene()), startPos, endPos);
    pushUndoStack(addCommand);
}

qreal CGraphicsView::alignmentMovPos(QRectF currSceneRect, QRectF itemRect, Qt::AlignmentFlag align)
{
    qDebug() << "currSceneRect   " << currSceneRect;
    qDebug() << "itemRect   " << itemRect;
    qreal movPos;
    switch (align) {
    case Qt::AlignLeft: {
        movPos = itemRect.x() - currSceneRect.x();
        break;
    }
    case Qt::AlignHCenter: {
        movPos = currSceneRect.height() / 2 - ((itemRect.y() - currSceneRect.y())
                                               + itemRect.height() / 2);
        break;
    }
    case Qt::AlignRight: {
        movPos = currSceneRect.width() - (itemRect.x() - currSceneRect.x())  - itemRect.width();

        break;
    }
    case Qt::AlignTop: {
        movPos =  itemRect.y() - currSceneRect.y();

        break;
    }
    case Qt::AlignVCenter: {
        movPos = currSceneRect.width() / 2 - ((itemRect.x() - currSceneRect.x())
                                              + itemRect.width() / 2);
        break;
    }
    case Qt::AlignBottom: {
        movPos = currSceneRect.height() - (itemRect.y() - currSceneRect.y()) - itemRect.height();
        break;
    }
    default: {
        movPos = 0.0;
        break;
    }
    }
    return  movPos;
}

void CGraphicsView::setContextMenuAndActionEnable(bool enable)
{
    m_isShowContext = enable;
    m_cutAct->setEnabled(enable);
    m_copyAct->setEnabled(enable);
    //m_pasteAct->setEnabled(enable);
    m_deleteAct->setEnabled(enable);
    m_selectAllAct->setEnabled(enable);
    m_undoAct->setEnabled(enable);
    m_redoAct->setEnabled(enable);
    m_oneLayerUpAct->setEnabled(enable);
    m_oneLayerDownAct->setEnabled(enable);
    m_bringToFrontAct->setEnabled(enable);
    m_sendTobackAct->setEnabled(enable);
}

//bool CGraphicsView::canLayerUp()
//{
//#if 0
//    auto curScene = dynamic_cast<CDrawScene *>(scene());
//    auto itemsMgr = curScene->selectGroup();

//    if (itemsMgr->isVisible() && !itemsMgr->items().isEmpty()) {
//        auto selectedItems = itemsMgr->items();
//        qSort(selectedItems.begin(), selectedItems.end(), zValueSortASC);

//        QList<QGraphicsItem *> allItems = scene()->items();
//        for (int i = allItems.size() - 1; i >= 0; i--) {
//            QGraphicsItem *allItem = allItems.at(i);
//            if (allItem->zValue() == 0.0) {
//                allItems.removeAt(i);
//                continue;
//            }
//            if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
//                allItems.removeAt(i);
//                continue;
//            }
//            for (int j = 0; j < selectedItems.size(); j++) {
//                QGraphicsItem *selectItem = selectedItems.at(j);
//                if (allItem == selectItem) {
//                    allItems.removeAt(i);
//                    break;
//                }
//            }
//        }

//        if (allItems.size() < 1) {
//            return false;
//        }

//        qSort(allItems.begin(), allItems.end(), zValueSortASC);
//        if (selectedItems.last()->zValue() >= allItems.last()->zValue()) {
//            return false;
//        }

//        return true;
//    } else {
//        QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
//        QList<QGraphicsItem *> allItems = scene()->items(Qt::AscendingOrder);

//        for (int i = allItems.size() - 1; i >= 0; i--) {
//            if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
//                allItems.removeAt(i);
//            }
//        }
//        int trueItemCount = allItems.size();

//        if (selectedItems.count() > 1 || selectedItems.count() <= 0 || trueItemCount <= 1) {
//            return false;
//        }
//        QGraphicsItem *selectItem = selectedItems.first();
//        qDebug() << "selectItem z = " << selectItem->zValue();
//        if (selectItem->zValue() >= dynamic_cast<CDrawScene *>(scene())->getMaxZValue()) {
//            return false;
//        }

//        return true;
//    }
//#endif
//    return false;
//}

//bool CGraphicsView::canLayerDown()
//{
//#if 0
//    auto curScene = dynamic_cast<CDrawScene *>(scene());
//    auto itemsMgr = curScene->selectGroup();
//    if (itemsMgr->isVisible() && !itemsMgr->items().isEmpty()) {
//        auto selectedItems = itemsMgr->items();
//        qSort(selectedItems.begin(), selectedItems.end(), zValueSortASC);

//        QList<QGraphicsItem *> allItems = scene()->items();
//        for (int i = allItems.size() - 1; i >= 0; i--) {
//            QGraphicsItem *allItem = allItems.at(i);
//            if (allItem->zValue() == 0.0) {
//                allItems.removeAt(i);
//                continue;
//            }
//            if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
//                allItems.removeAt(i);
//                continue;
//            }
//            for (int j = 0; j < selectedItems.size(); j++) {
//                QGraphicsItem *selectItem = selectedItems.at(j);
//                if (allItem == selectItem) {
//                    allItems.removeAt(i);
//                    break;
//                }
//            }
//        }

//        if (allItems.size() < 1) {
//            return false;
//        }

//        qSort(allItems.begin(), allItems.end(), zValueSortASC);
//        if (allItems.first()->zValue() >= selectedItems.first()->zValue()) {
//            return false;
//        }
//        return true;
//    } else {
//        QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
//        QList<QGraphicsItem *> allItems = scene()->items(Qt::AscendingOrder);
//        for (int i = allItems.size() - 1; i >= 0; i--) {
//            if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
//                allItems.removeAt(i);
//            }
//        }

//        int trueItemCount = allItems.size();

//        if (selectedItems.count() > 1 || selectedItems.count() <= 0 || trueItemCount <= 1) {
//            return false;
//        }

//        qSort(&allItems.first(), &allItems.last(), zValueSortASC);
//        QGraphicsItem *selectItem = selectedItems.first();
//        if (selectItem->zValue() <= allItems.first()->zValue()) {
//            return false;
//        }

//        return true;
//    }
//#endif
//    return  false;
//}

//QList<CGraphicsItem *> CGraphicsView::getSelectedValidItems()
//{
//    auto curScene = dynamic_cast<CDrawScene *>(scene());
//    QList<CGraphicsItem *> validItems = curScene->selectGroup()->items();
//    // [0] 过滤错误图元
//    for (int i = 0; i < validItems.size(); i++) {
//        QGraphicsItem *allItem = validItems.at(i);
//        if (allItem->zValue() == 0.0) {
//            validItems.removeAt(i);
//            continue;
//        }
//        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
//            validItems.removeAt(i);
//            continue;
//        }
//    }

//    // [1] 判断当前是如果不是多选则是单选，后期优化多选策略后，此处代码可以删除掉
//    if (!validItems.size()) {
//        QList<QGraphicsItem *> items = scene()->selectedItems();
//        // [2] 过滤错误图元
//        for (int i = 0; i < items.size(); i++) {
//            QGraphicsItem *item = items.at(i);
//            if (item->zValue() == 0.0) {
//                items.removeAt(i);
//                continue;
//            }
//            if (item->type() <= QGraphicsItem::UserType || item->type() >= EGraphicUserType::MgrType) {
//                items.removeAt(i);
//                continue;
//            }
//            validItems.append(static_cast<CGraphicsItem *>(item));
//        }
//    }

//    return validItems;
//}

bool CGraphicsView::getCouldPaste()
{
    QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
    QString filePath = mp->text();
    QStringList tempfilePathList = filePath.split("\n");


    bool couldPaste = false;
    QString ddfPath = "";
    for (int i = 0; i < tempfilePathList.size(); i++) {
        QFileInfo info(tempfilePathList[i]);
        if (info.suffix().toLower() == ("ddf")) {
            ddfPath = tempfilePathList[i].replace("file://", "");
            if (!ddfPath.isEmpty()) {
                bool isOpened = CManageViewSigleton::GetInstance()->isDdfFileOpened(ddfPath);
                if (isOpened)
                    continue;
                couldPaste = true;
            }
        } else if (tempfilePathList[i].endsWith(".png") || tempfilePathList[i].endsWith(".jpg")
                   || tempfilePathList[i].endsWith(".bmp") || tempfilePathList[i].endsWith(".tif")) {
            //图片格式："*.png *.jpg *.bmp *.tif"
            couldPaste = true;
        }

    }
    return couldPaste;
}

void CGraphicsView::setCcdpMenuActionStatus(bool enable)
{
    m_copyAct->setEnabled(enable);
    m_cutAct->setEnabled(enable);
    m_deleteAct->setEnabled(enable);
}

void CGraphicsView::setClipboardStatus()
{
    bool pasteFlag = false;
    QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
    // 判断剪切板数据是否为文字
    if (mp->hasText()) {
        pasteFlag = true;
    } else if (mp->hasImage()) {
        pasteFlag = true;
    } else if (mp->hasFormat("drawItems")) {
        pasteFlag = true;
    }

    m_pasteAct->setEnabled(pasteFlag);
    m_undoAct->setEnabled(m_pUndoStack->canUndo());
    m_redoAct->setEnabled(m_pUndoStack->canRedo());
}

void CGraphicsView::setLayerMenuActionStatus(bool layervistual)
{
    m_layerMenu->setEnabled(layervistual);

    bool layerUp = drawScene()->isCurrentZMovable(EUpLayer);
    m_oneLayerUpAct->setEnabled(layerUp);
    m_bringToFrontAct->setEnabled(layerUp);

    bool layerDown = drawScene()->isCurrentZMovable(EDownLayer);
    m_oneLayerDownAct->setEnabled(layerDown);
    m_sendTobackAct->setEnabled(layerDown);
}

void CGraphicsView::setAlignMenuActionStatus(bool acticonvistual)
{
    m_alignMenu->setEnabled(acticonvistual);

    m_itemsLeftAlign->setEnabled(acticonvistual);      //左对齐
    m_itemsHCenterAlign->setEnabled(acticonvistual);   //水平居中对齐
    m_itemsRightAlign->setEnabled(acticonvistual);     //右对齐
    m_itemsTopAlign->setEnabled(acticonvistual);       //顶对齐
    m_itemsVCenterAlign->setEnabled(acticonvistual);   //垂直居中对齐
    m_itemsBottomAlign->setEnabled(acticonvistual);    //底对齐

    auto curScene = dynamic_cast<CDrawScene *>(scene());
    const int selectItemsCount = curScene->selectGroup()->items().size();
    m_itemsVEqulSpaceAlign->setEnabled(selectItemsCount >= 3);//水平等间距对齐
    m_itemsHEqulSpaceAlign->setEnabled(selectItemsCount >= 3);//垂直等间距对齐
}

void CGraphicsView::setTextAlignMenuActionStatus(CGraphicsItem *tmpitem)
{
    // 根据是否是两点情况显示对齐
    if (static_cast<CGraphicsTextItem *>(tmpitem)->isAutoAdjustSize()) {
        m_textLeftAlignAct->setEnabled(false);
        m_textRightAlignAct->setEnabled(false);
        m_textCenterAlignAct->setEnabled(false);
    } else {
        m_textLeftAlignAct->setEnabled(true);
        m_textRightAlignAct->setEnabled(true);
        m_textCenterAlignAct->setEnabled(true);
    }
}

ProgressLayout *CGraphicsView::getProgressLayout(bool firstShow)
{
    if (m_progressLayout == nullptr) {
        m_progressLayout = new ProgressLayout(drawApp->topMainWindowWidget());

        if (firstShow) {
            QMetaObject::invokeMethod(this, [ = ]() {
                QRect rct = drawApp->topMainWindowWidget()->geometry();
                getProgressLayout()->move(rct.topLeft() + QPoint((rct.width() - m_progressLayout->width()) / 2,
                                                                 (rct.height() - m_progressLayout->height()) / 2));
                m_progressLayout->raise();
                m_progressLayout->show();
            }, Qt::QueuedConnection);
        }
    }
    return m_progressLayout;
}

//拖曳加载文件
void CGraphicsView::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasText()) {
        CCentralwidget *pWidget = qobject_cast<CCentralwidget *>(parentWidget());
        if (pWidget != nullptr) {
            QList<QUrl> urls  = e->mimeData()->urls();
            QStringList paths;
            for (auto url : urls) {
                QString filePath = url.path();
                if (!filePath.isEmpty())
                    paths.append(filePath);
            }
            this->setFocus();
            pWidget->openFiles(paths, false, true, false);
        }
        return;
    }

    DGraphicsView::dropEvent(e);
}

void CGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;
    }
    DGraphicsView::dragEnterEvent(event);
}

void CGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;
    }
    DGraphicsView::dragMoveEvent(event);
}

void CGraphicsView::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    if (nullptr != scene()) {
        if (!_spaceKeyPressed) {
            auto curScene = static_cast<CDrawScene *>(scene());
            curScene->changeMouseShape(currentMode);
        } else {
            drawApp->setApplicationCursor(Qt::ClosedHandCursor);
        }
    }
}

void CGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (!event->isAutoRepeat()) {
            QGraphicsItem *pFocusItem = drawScene()->focusItem();
            bool isTextEditable = (pFocusItem != nullptr &&
                                   pFocusItem->type() == QGraphicsProxyWidget::Type);

            if (!isTextEditable && dApp->mouseButtons() == Qt::NoButton) {
                _spaceKeyPressed = true;
                _tempCursor = *qApp->overrideCursor();
                drawApp->setApplicationCursor(Qt::ClosedHandCursor, true);
            }
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void CGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (!event->isAutoRepeat()) {
            if (_spaceKeyPressed) {
                _spaceKeyPressed = false;
                if (getDrawParam()->getCurrentDrawToolMode() == selection)
                    updateCursorShape();
                else {
                    drawApp->setApplicationCursor(_tempCursor);
                }
            }
        }
    }
    QGraphicsView::keyReleaseEvent(event);
}

bool CGraphicsView::eventFilter(QObject *o, QEvent *e)
{
    if (viewport() == o) {
        bool finished = false;
        if (e->type() == QEvent::MouseButtonPress) {
            QMouseEvent *event = dynamic_cast<QMouseEvent *>(e);
            _pressBeginPos = event->pos();
            _recordMovePos = _pressBeginPos;
            if (_spaceKeyPressed && event->button() == Qt::LeftButton) {
                finished       = true;
            }
        } else if (e->type() == QEvent::MouseMove) {
            QMouseEvent *event = dynamic_cast<QMouseEvent *>(e);
            {
                if (_spaceKeyPressed && event->buttons() == Qt::LeftButton) {
                    //移动卷轴
                    CDrawScene *pScene = qobject_cast<CDrawScene *>(scene());
                    if (pScene != nullptr) {
                        //pScene->clearSelection();
                        pScene->blockMouseMoveEvent(true);
                    }
                    QPointF mov = event->pos() - _recordMovePos;
                    int horValue = this->horizontalScrollBar()->value() - qRound(mov.x());
                    //qDebug() << "old hor value = " << this->horizontalScrollBar()->value() << "new hor value = " << horValue;
                    this->horizontalScrollBar()->setValue(qMin(qMax(this->horizontalScrollBar()->minimum(), horValue), this->horizontalScrollBar()->maximum()));

                    int verValue = this->verticalScrollBar()->value() - qRound(mov.y());
                    //qDebug() << "mov.y() = " << mov.y() << "cur value = " << this->verticalScrollBar()->value() << "wanted value = " << verValue << "max = " << this->verticalScrollBar()->maximum();
                    this->verticalScrollBar()->setValue(qMin(qMax(this->verticalScrollBar()->minimum(), verValue), this->verticalScrollBar()->maximum()));

                    if (pScene != nullptr) {
                        pScene->blockMouseMoveEvent(false);
                    }

                    finished = true;
                }
                _recordMovePos = event->pos();
            }
        } else if (e->type() == QEvent::Leave) {
            if (drawScene() != nullptr) {
                drawScene()->doLeave();
            }
        }
        return finished;
    }
    return DGraphicsView::eventFilter(o, e);
}

bool CGraphicsView::viewportEvent(QEvent *event)
{
    return DGraphicsView::viewportEvent(event);
}

bool CGraphicsView::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

void CGraphicsView::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::RotationAngleChanged) {
        qreal rotationDelta = gesture->rotationAngle() - gesture->lastRotationAngle();
        Q_UNUSED(rotationDelta);
    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        scaleWithCenter(gesture->scaleFactor(), viewport()->mapFromGlobal(gesture->centerPoint().toPoint()));
    }
    if (gesture->state() == Qt::GestureFinished) {
    }
    update();
}





