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
#include "bzItems/cgraphicsmasicoitem.h"
#include "bzItems/cgraphicspenitem.h"
#include "bzItems/cpictureitem.h"
#include "drawTools/cpicturetool.h"
#include "drawItems/cgraphicsitemselectedmgr.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
#include "drawTools/cdrawtoolmanagersigleton.h"
#include "application.h"
#include "cundocommands.h"
#include "cundoredocommand.h"
#include "mainwindow.h"

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

//升序排列用
static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() < info2->zValue();
}
//降序排列用
//static bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
//{
//    return info1->zValue() > info2->zValue();
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
    //viewport()->grabGesture(Qt::PanGesture);
    //viewport()->grabGesture(Qt::SwipeGesture);
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

    //    DGraphicsView::scale(multiple, multiple);
    //    m_scale = scale;
    //    getDrawParam()->setScale(m_scale);
    //    emit signalSetScale(m_scale);
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
}

void CGraphicsView::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
    //QGraphicsView::wheelEvent(event);
}

void CGraphicsView::initContextMenu()
{
    m_contextMenu = new CMenu(this);
    m_contextMenu->setFixedWidth(182);

    m_layerMenu = new CMenu(tr("Layer"), this);
    m_layerMenu->setFixedWidth(182);

    m_cutAct = new QAction(tr("Cut"), this);
    m_contextMenu->addAction(m_cutAct);
    m_cutAct->setShortcut(QKeySequence::Cut);
    this->addAction(m_cutAct);

    m_copyAct = new QAction(tr("Copy"), this);
    m_contextMenu->addAction(m_copyAct);
    m_copyAct->setShortcut(QKeySequence::Copy);
    this->addAction(m_copyAct);

    m_pasteAct = new QAction(tr("Paste"), this);
    m_contextMenu->addAction(m_pasteAct);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    this->addAction(m_pasteAct);

    m_selectAllAct = new QAction(tr("Select All"), this);
    m_contextMenu->addAction(m_selectAllAct);
    m_selectAllAct->setShortcut(QKeySequence::SelectAll);
    this->addAction(m_selectAllAct);

    m_contextMenu->addSeparator();

    m_deleteAct = new QAction(tr("Delete"), this);
    m_contextMenu->addAction(m_deleteAct);
    m_deleteAct->setShortcut(QKeySequence::Delete);
    this->addAction(m_deleteAct);

    m_undoAct = m_pUndoStack->createUndoAction(this, tr("Undo"));
    m_contextMenu->addAction(m_undoAct);
    m_undoAct->setShortcut(QKeySequence::Undo);
    this->addAction(m_undoAct);
    m_redoAct = m_pUndoStack->createRedoAction(this, tr("Redo"));
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
    m_itemsVEqulSpaceAlign = m_alignMenu->addAction(tr("Distribute vertical space")); //垂直等间距对齐

    // 添加对齐菜单
    m_contextMenu->addMenu(m_layerMenu);
}

void CGraphicsView::initContextMenuConnection()
{
    connect(m_cutAct, SIGNAL(triggered()), this, SLOT(slotOnCut()));
    connect(m_copyAct, SIGNAL(triggered()), this, SLOT(slotOnCopy()));
    connect(m_pasteAct, SIGNAL(triggered()), this, SLOT(slotOnPaste()));
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
        updateCursorShape();
    });
    connect(m_redoAct, &QAction::triggered, this, [ = ] {
        updateCursorShape();
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
        QList<CGraphicsItem *> allitems = getSelectedValidItems();

        // [1] 获取图元为空则返回
        if (!allitems.size())
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
        QRectF scence_BR = curScene->getItemsMgr()->sceneBoundingRect();

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
        QList<CGraphicsItem *> allitems = getSelectedValidItems();

        // [1] 获取图元为空则返回
        if (!allitems.size())
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
        QRectF scence_BR = curScene->getItemsMgr()->sceneBoundingRect();

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
            bool layerUp = canLayerUp();
            m_oneLayerUpAct->setEnabled(layerUp);
            m_bringToFrontAct->setEnabled(layerUp);

            bool layerDown = canLayerDown();
            m_oneLayerDownAct->setEnabled(layerDown);
            m_sendTobackAct->setEnabled(layerDown);
        }
    });
}

void CGraphicsView::initTextContextMenu()
{
    m_textMenu = new DMenu(this);
    m_textMenu->setFixedWidth(182);

    m_textCutAction = new QAction(tr("Cut"));
    m_textCopyAction = new QAction(tr("Copy"));
    m_textPasteAction = new QAction(tr("Paste"));
    m_textSelectAllAction = new QAction(tr("Select All"));
    m_textUndoAct = new QAction(tr("Undo"));
    m_textRedoAct = new QAction(tr("Redo"));
    m_textLeftAlignAct = new QAction(tr("Text Align Left"));                  // 左对齐
    m_textRightAlignAct = new QAction(tr("Text Align Right"));             // 右对齐
    m_textCenterAlignAct = new QAction(tr("Text Align Center"));      //  水平垂直居中对齐
    m_textDeleteAction = new QAction(tr("Delete"));

//    QAction *fakeRaiseLayerAct = new QAction(tr("Raise Layer"));
//    fakeRaiseLayerAct->setEnabled(false);
//    QAction *fakeLowerLayerAct = new QAction(tr("Lower Layer"));
//    fakeLowerLayerAct->setEnabled(false);
//    QAction *fakeLayerToTopAct = new QAction(tr("Layer to Top"));
//    fakeLayerToTopAct->setEnabled(false);
//    QAction *fakeLayerToBottomAct = new QAction(tr("Layer to Bottom"));
//    fakeLayerToBottomAct->setEnabled(false);

    m_textMenu->addAction(m_textCutAction);
    m_textMenu->addAction(m_textCopyAction);
    m_textMenu->addAction(m_textPasteAction);
    m_textMenu->addAction(m_textSelectAllAction);
    m_textMenu->addAction(m_textDeleteAction);
    m_textMenu->addSeparator();
    m_textMenu->addAction(m_textUndoAct);
    m_textMenu->addAction(m_textRedoAct);
    m_textMenu->addSeparator();
//    m_textMenu->addAction(fakeRaiseLayerAct);
//    m_textMenu->addAction(fakeLowerLayerAct);
//    m_textMenu->addAction(fakeLayerToTopAct);
//    m_textMenu->addAction(fakeLayerToBottomAct);
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
    connect(m_DDFManager, SIGNAL(signalClearSceneBeforLoadDDF()), this, SLOT(clearScene()));
    connect(m_DDFManager, SIGNAL(signalStartLoadDDF(QRectF)), this, SLOT(slotStartLoadDDF(QRectF)));
    connect(m_DDFManager, SIGNAL(signalAddItem(QGraphicsItem *, bool)), this, SLOT(slotAddItemFromDDF(QGraphicsItem *, bool)));
    connect(m_DDFManager, &CDDFManager::signalAddTextItem, this, [ = ](const SGraphicsTextUnitData & data,
    const SGraphicsUnitHead & head, bool pushToStack) {
        CGraphicsTextItem *item = new CGraphicsTextItem(data, head);
        slotAddItemFromDDF(item, pushToStack);
    });
    connect(m_DDFManager, &CDDFManager::signalSaveFileFinished, this, &CGraphicsView::signalSaveFileStatus);
    connect(m_DDFManager, &CDDFManager::singalEndLoadDDF, this, [ = ]() {

        auto curScene = dynamic_cast<CDrawScene *>(scene());
        qreal tempZ = curScene->getMaxZValue();

        this->drawScene()->blockUpdateBlurItem(true);
        for (QGraphicsItem *item : m_loadFromDDF) {
            item->setZValue(tempZ + 1);
            tempZ++;
        }
        this->drawScene()->blockUpdateBlurItem(false);

        this->drawScene()->updateBlurItem();

        m_loadFromDDF.clear();
        curScene->setMaxZValue(tempZ);

        emit singalTransmitEndLoadDDF();
    });

    // 连接undo redo改变的信号
    connect(m_pUndoStack, &QUndoStack::canUndoChanged, this, [ = ](bool undo) {
        this->setModify(undo);
    });
}

void CGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    //    if (qApp->mouseButtons() & Qt::LeftButton) {
    //        return QGraphicsView::contextMenuEvent(event);
    //    }

    QPointF pos = this->mapToScene(event->pos());
    QRectF rect = this->scene()->sceneRect();

    if (!rect.contains(pos)) {
        return;
    }

    if (!m_isShowContext) {
        return;
    }

    //获取右键菜单的显示位置，左边工具栏宽度为162，顶端参数配置栏高度为50，右键菜单三种分别为224\350\480.
    QPoint menuPos;
    int rx;
    int ry;
    //qDebug() << cursor().pos() << m_contextMenu->rect()  << this->rect() << endl;
    if (cursor().pos().rx() - 50 > this->width() - 182) {
        rx = this->width() - 182 + 50;
    } else {
        rx = cursor().pos().rx();
    }
    int temp;

    if (!scene()->selectedItems().isEmpty()) {
        //如果是文字图元则显示其自己的右键菜单
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsItem *tmpitem = static_cast<CGraphicsItem *>(item);
        if (TextType == item->type() &&  static_cast<CGraphicsTextItem *>(tmpitem)->isEditable()) {
            temp = 480;
            if (cursor().pos().ry() - 50 > this->height() - temp) {
                ry = this->height() - temp + 45;
            } else {
                ry = cursor().pos().ry();
            }

            // 根据是否是两点情况显示对齐
            if (!static_cast<CGraphicsTextItem *>(tmpitem)->getManResizeFlag()) {
                m_textLeftAlignAct->setEnabled(false);
                m_textRightAlignAct->setEnabled(false);
                m_textCenterAlignAct->setEnabled(false);
            } else {
                m_textLeftAlignAct->setEnabled(true);
                m_textRightAlignAct->setEnabled(true);
                m_textCenterAlignAct->setEnabled(true);
            }

            menuPos = QPoint(rx, ry);
            m_textMenu->move(menuPos);
            m_textMenu->show();
            m_visible = true;
            showMenu(m_textMenu);
            return;
        } else {
            m_copyAct->setEnabled(true);
            m_cutAct->setEnabled(true);
            m_deleteAct->setEnabled(true);

            m_layerMenu->setEnabled(true);
            m_bringToFrontAct->setVisible(true);
            m_sendTobackAct->setVisible(true);
            m_oneLayerUpAct->setVisible(true);
            m_oneLayerDownAct->setVisible(true);

            m_alignMenu->setEnabled(true);
            m_itemsLeftAlign->setVisible(true);      //左对齐
            m_itemsHCenterAlign->setVisible(true);   //水平居中对齐
            m_itemsRightAlign->setVisible(true);     //右对齐
            m_itemsTopAlign->setVisible(true);       //顶对齐
            m_itemsVCenterAlign->setVisible(true);   //垂直居中对齐
            m_itemsBottomAlign->setVisible(true);    //底对齐
            m_itemsVEqulSpaceAlign->setVisible(true);//水平等间距对齐
            m_itemsHEqulSpaceAlign->setVisible(true);//垂直等间距对齐
        }
    } else {
        m_copyAct->setEnabled(false);
        m_cutAct->setEnabled(false);
        m_deleteAct->setEnabled(false);

        m_layerMenu->setEnabled(false);
        m_bringToFrontAct->setVisible(false);
        m_sendTobackAct->setVisible(false);
        m_oneLayerUpAct->setVisible(false);
        m_oneLayerDownAct->setVisible(false);

        m_alignMenu->setEnabled(false);
        m_itemsLeftAlign->setVisible(false);      //左对齐
        m_itemsHCenterAlign->setVisible(false);   //水平居中对齐
        m_itemsRightAlign->setVisible(false);     //右对齐
        m_itemsTopAlign->setVisible(false);       //顶对齐
        m_itemsVCenterAlign->setVisible(false);   //垂直居中对齐
        m_itemsBottomAlign->setVisible(false);    //底对齐
        m_itemsVEqulSpaceAlign->setVisible(false);//水平等间距对齐
        m_itemsHEqulSpaceAlign->setVisible(false);//垂直等间距对齐
        m_visible = false;
    }

    // 根据选择的图元个数显示能进行的对齐操作
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    const int selectItemsCount = curScene->getItemsMgr()->getItems().size();
    m_itemsVEqulSpaceAlign->setEnabled(false);//水平等间距对齐
    m_itemsHEqulSpaceAlign->setEnabled(false);//垂直等间距对齐
    if (selectItemsCount >= 3) {
        m_itemsVEqulSpaceAlign->setEnabled(true);//水平等间距对齐
        m_itemsHEqulSpaceAlign->setEnabled(true);//垂直等间距对齐
    }


    //判定是长右键菜单还是短右键菜单;
    if (m_visible) {
        temp = 350;
    } else {
        temp = 224;
    }

    if (cursor().pos().ry() - 50 > this->height() - temp) {
        ry = this->height() - temp + 45;
    } else {
        ry = cursor().pos().ry();
    }
    menuPos = QPoint(rx, ry);

    bool selectAllActflag = false;
    for (auto item : scene()->items()) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            selectAllActflag  = true;
            break;
        }
    }
    m_selectAllAct->setEnabled(selectAllActflag);

    bool layerUp = canLayerUp();
    m_oneLayerUpAct->setEnabled(layerUp);
    m_bringToFrontAct->setEnabled(layerUp);

    bool layerDown = canLayerDown();
    m_oneLayerDownAct->setEnabled(layerDown);
    m_sendTobackAct->setEnabled(layerDown);

    QPixmap map = QApplication::clipboard()->pixmap();
    QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
    QString filePath = mp->text();
    bool pasteFlag = false;
    if (!map.isNull()) {
        pasteFlag = true;
    }
    if (filePath.isEmpty()) {
        CShapeMimeData *data = dynamic_cast< CShapeMimeData *>(mp);
        if (data) {
            pasteFlag = true;
        }
    }

    m_pasteAct->setEnabled(pasteFlag);
    if (!pasteFlag) {
        m_pasteAct->setEnabled(getCouldPaste());
    }

    m_contextMenu->move(menuPos);
    m_undoAct->setEnabled(m_pUndoStack->canUndo());
    m_redoAct->setEnabled(m_pUndoStack->canRedo());

    showMenu(m_contextMenu);
}

void CGraphicsView::showMenu(DMenu *pMenu)
{
    QPoint curPos = QCursor::pos();

    QSize menSz = pMenu->size();

    QRect menuRect = QRect(curPos, menSz);

    QScreen *pCurScren = dApp->topMainWindow()->windowHandle()->screen();

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
    //QTime ttt;
    //ttt.start();
    if (doPaint)
        DGraphicsView::paintEvent(event);
    else {
        QPainter painter(this->viewport());
        painter.drawPixmap(QPoint(0, 0), pix);
        //绘制额外的前景显示，如框选等
        EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);

        if (pTool != nullptr) {
            pTool->drawMore(&painter, mapToScene(QRect(QPoint(0, 0), pix.size())).boundingRect(), drawScene());
        }
    }
    //qDebug() << "useeee ============== " << ttt.elapsed();
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
    dApp->setApplicationCursor(Qt::ArrowCursor);
}

//void CGraphicsView::itemMoved(QGraphicsItem *item, const QPointF &newPosition)
//{
//    Q_UNUSED(item)
//    Q_UNUSED(newPosition)
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    if (item != nullptr) {
//        //QUndoCommand *moveCommand = new CMoveShapeCommand(curScene, item, newPosition);
//        //this->pushUndoStack(moveCommand);
//    } else {
//        //QUndoCommand *moveCommand = new CMoveShapeCommand(curScene, newPosition);
//        //this->pushUndoStack(moveCommand);
//    }
//}

void CGraphicsView::itemAdded(QGraphicsItem *item, bool pushToStack)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<QGraphicsItem *> addItems;
    addItems.clear();
    addItems.append(item);
    item->setZValue(curScene->getMaxZValue() + 1);
    curScene->setMaxZValue(curScene->getMaxZValue() + 1);
    if (pushToStack) {
        //QUndoCommand *addCommand = new CAddShapeCommand(curScene, addItems);
        //this->pushUndoStack(addCommand);
    }
}

//void CGraphicsView::itemRotate(QGraphicsItem *item, const qreal newAngle)
//{
//    Q_UNUSED(item)
//    Q_UNUSED(newAngle)
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *rotateCommand = new CRotateShapeCommand(curScene, item, newAngle);
//    //this->pushUndoStack(rotateCommand);
//}

//void CGraphicsView::itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QRectF beginRect, QPointF endPos, bool bShiftPress, bool bALtPress)
//{
//    Q_UNUSED(item)
//    Q_UNUSED(handle)
//    Q_UNUSED(beginRect)

//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *resizeCommand = new CResizeShapeCommand(curScene, item, handle, beginRect,  endPos, bShiftPress, bALtPress);
//    //this->pushUndoStack(resizeCommand);
//}

//void CGraphicsView::itemPropertyChange(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *setPropertyCommand = new CSetPropertyCommand(curScene, item, pen, brush, bPenChange, bBrushChange);
//    //this->pushUndoStack(setPropertyCommand);
//}

//void CGraphicsView::itemRectXRediusChange(CGraphicsRectItem *item, int xRedius, bool bChange)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *setRectXRediusCommand = new CSetRectXRediusCommand(curScene, item, xRedius, bChange);
//    //this->pushUndoStack(setRectXRediusCommand);
//}

//void CGraphicsView::itemPolygonPointChange(CGraphicsPolygonItem *item, int newNum)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *command = new CSetPolygonAttributeCommand(curScene, item, newNum);
//    //this->pushUndoStack(command);
//}

//void CGraphicsView::itemPenTypeChange(CGraphicsPenItem *item, bool isStart, ELineType newOldType)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *command = new CSetPenAttributeCommand(curScene, item, isStart, newOldType);
//    //m_pUndoStack->push(command);
//}

//void CGraphicsView::itemLineTypeChange(CGraphicsLineItem *item, bool isStart, ELineType newOldType)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *command = new CSetLineAttributeCommand(curScene, item, isStart, newOldType);
//    //m_pUndoStack->push(command);
//}

//void CGraphicsView::itemBlurChange(CGraphicsMasicoItem *item, int effect, int blurWidth)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *command = new CSetBlurAttributeCommand(curScene, item, effect, blurWidth);
//    //this->pushUndoStack(command);
//}

//void CGraphicsView::itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *item, int newNum, int newRadius)
//{
//    //auto curScene = dynamic_cast<CDrawScene *>(scene());
//    //QUndoCommand *command = new CSetPolygonStarAttributeCommand(curScene, item, newNum, newRadius);
//    //this->pushUndoStack(command);
//}

//void CGraphicsView::slotStopContinuousDrawing()
//{
//    m_isStopContinuousDrawing = true;
//}

void CGraphicsView::slotStartLoadDDF(QRectF rect)
{
    scene()->setSceneRect(rect);
}

void CGraphicsView::slotAddItemFromDDF(QGraphicsItem *item, bool pushToStack)
{
    Q_UNUSED(pushToStack)
    scene()->addItem(item);
    m_loadFromDDF.append(item);
}

void CGraphicsView::slotOnCut()
{
    //    QList<QGraphicsItem *> allItems;
    //    auto curScene = dynamic_cast<CDrawScene *>(scene());
    //    QList<CGraphicsItem *> seleteItems = curScene->getItemsMgr()->getItems();
    //    if (!seleteItems.isEmpty()) {
    //        for (QGraphicsItem *item : seleteItems) {
    //            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
    //                allItems.push_back(item);
    //            }
    //        }
    //        curScene->getItemsMgr()->hide();
    //    } else {
    //        QList<QGraphicsItem *> curSeleteItems = scene()->selectedItems();

    //        if (curSeleteItems.isEmpty()) {
    //            return;
    //        }

    //        for (QGraphicsItem *item : scene()->items(Qt::AscendingOrder)) {
    //            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
    //                allItems.push_back(item);
    //            }
    //        }

    //        if (allItems.count() != curSeleteItems.count()) {
    //            if (seleteItems.count() > 1) {
    //                return;
    //            }
    //            allItems.clear();
    //            allItems.push_back(curSeleteItems.first());
    //        }
    //    }
    //    //升序排列
    //    qSort(allItems.begin(), allItems.end(), zValueSortASC);

    //    QUndoCommand *deleteCommand = new CDeleteShapeCommand(curScene, allItems);
    //    this->pushUndoStack(deleteCommand);

    //    CShapeMimeData *data = new CShapeMimeData(allItems);
    //    data->setText("");
    //    QApplication::clipboard()->setMimeData(data);

    //    if (!m_pasteAct->isEnabled()) {
    //        m_pasteAct->setEnabled(true);
    //    }
    //    updateCursorShape();

    //    curScene->getItemsMgr()->updateBoundingRect();

    //记录还原点
    QList<QGraphicsItem *> allItems = scene()->selectedItems();
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(scene());
    for (QGraphicsItem *pItem : allItems) {
        if (drawScene()->isBussizeItem(pItem)) {
            vars << reinterpret_cast<long long>(pItem);
        }
    }
    CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                        CSceneUndoRedoCommand::EItemRemoved, vars, true);

    CUndoRedoCommand::finishRecord(true);

    //将数据复制到粘贴板
    CShapeMimeData *data = new CShapeMimeData(allItems);

    data->setText("");
    QApplication::clipboard()->setMimeData(data);

    m_pasteAct->setEnabled(true);

    //刷新多选图元
    //drawScene()->getItemsMgr()->updateBoundingRect();

    //drawScene()->refreshLook();
}

void CGraphicsView::slotOnCopy()
{
    QList<QGraphicsItem *> allItems;
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> seleteItems = curScene->getItemsMgr()->getItems();
    if (!seleteItems.isEmpty()) {
        for (QGraphicsItem *item : seleteItems) {
            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
                allItems.push_back(item);
            }
        }
    } else {
        QList<QGraphicsItem *> curSeleteItems = scene()->selectedItems();

        if (curSeleteItems.isEmpty()) {
            return;
        }

        for (QGraphicsItem *item : scene()->items(Qt::AscendingOrder)) {
            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
                allItems.push_back(item);
            }
        }

        if (allItems.count() != curSeleteItems.count()) {
            if (seleteItems.count() > 1) {
                return;
            }
            allItems.clear();
            allItems.push_back(curSeleteItems.first());
        }
    }

    CShapeMimeData *data = new CShapeMimeData(allItems);
    data->setText("");
    QApplication::clipboard()->setMimeData(data);

    if (!m_pasteAct->isEnabled()) {
        m_pasteAct->setEnabled(true);
    }
}

void CGraphicsView::slotOnPaste()
{
    QPixmap map = QApplication::clipboard()->pixmap();
    QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
    QString filePath = mp->text();
    //qDebug() << "slotOnPaste"  << endl;

    if (!map.isNull()) {
        //粘贴剪切板中的图片
        QVariant imageData = mp->imageData();
        QPixmap pixmap = imageData.value<QPixmap>();

        qDebug() << "entered mp->hasImage()"  << endl;
        if (!pixmap.isNull()) {
            QByteArray src = CManageViewSigleton::GetInstance()->getFileSrcData(filePath);
            emit signalPastePixmap(pixmap, src);
        }
        qDebug() << "imageData" << imageData << endl;
    } else if (filePath != "") {
        //粘贴文件路径
        emit signalLoadDragOrPasteFile(filePath);
        //qDebug() << "filePath" << filePath << endl;
    } else {
        qDebug() << "mp->hasImage()"  << mp->hasImage() << endl;

        //粘贴画板内部图元
        CShapeMimeData *data = dynamic_cast< CShapeMimeData *>(mp);
        if (data != nullptr) {
            drawScene()->clearMrSelection();

            QList<CGraphicsItem *> allItems = data->creatCopyItems();

            QList<QVariant> vars;
            vars << reinterpret_cast<long long>(scene());

            foreach (CGraphicsItem *item, allItems) {
                vars << reinterpret_cast<long long>(item);

                drawScene()->addItem(item);

                item->moveBy(10, 10);

                drawScene()->selectItem(item, true, true, false);

                qreal newZ = this->drawScene()->getMaxZValue() + 1;
                item->setZValue(newZ);
                this->drawScene()->setMaxZValue(newZ);
            }

            CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                CSceneUndoRedoCommand::EItemAdded, vars, true, true);

            CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                CSceneUndoRedoCommand::EItemAdded, vars);

            CUndoRedoCommand::finishRecord();

            drawScene()->getItemsMgr()->updateAttributes();
            drawScene()->getItemsMgr()->updateBoundingRect();
        }
    }
}

void CGraphicsView::slotOnSelectAll()
{
    //    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    //    if (currentMode != selection) {
    //        return;
    //    }
    //    scene()->clearSelection();

    //    auto curScene = dynamic_cast<CDrawScene *>(scene());
    //    foreach (QGraphicsItem *item, scene()->items()) {
    //        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
    //            auto curItem = static_cast<CGraphicsItem *>(item);
    //            curScene->getItemsMgr()->add(curItem);
    //        }
    //    }
    //    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
    //    if (curScene->getItemsMgr()->getItems().isEmpty()) {
    //        return;
    //    }
    //    curScene->clearSelection();
    //    curScene->getItemsMgr()->show();
    //    curScene->getItemsMgr()->setSelected(true);

    //    if (nullptr != scene()) {
    //        auto curScene = static_cast<CDrawScene *>(scene());
    //        curScene->updateBlurItem();

    //        CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
    //    }

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    if (currentMode != selection) {
        return;
    }
    drawScene()->selectItemsByRect(sceneRect());
}

void CGraphicsView::slotOnDelete()
{
    //    QList<QGraphicsItem *> allItems;
    //    auto curScene = dynamic_cast<CDrawScene *>(scene());
    //    QList<CGraphicsItem *> seleteItems = curScene->getItemsMgr()->getItems();
    //    if (!seleteItems.isEmpty()) {
    //        for (QGraphicsItem *item : seleteItems) {
    //            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
    //                allItems.push_back(item);
    //            }
    //        }
    //        curScene->getItemsMgr()->hide();
    //    } else {
    //        QList<QGraphicsItem *> curSeleteItems = scene()->selectedItems();

    //        if (curSeleteItems.isEmpty()) {
    //            return;
    //        }

    //        for (QGraphicsItem *item : scene()->items(Qt::AscendingOrder)) {
    //            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
    //                allItems.push_back(item);
    //            }
    //        }

    //        if (allItems.count() != curSeleteItems.count()) {
    //            if (seleteItems.count() > 1) {
    //                return;
    //            }
    //            allItems.clear();
    //            allItems.push_back(curSeleteItems.first());
    //        }
    //    }

    //    QUndoCommand *deleteCommand = new CDeleteShapeCommand(curScene, allItems);
    //    this->pushUndoStack(deleteCommand);

    //    updateCursorShape();

    //记录还原点
    QList<QGraphicsItem *> allItems = scene()->selectedItems();
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(scene());
    for (QGraphicsItem *pItem : allItems) {
        if (drawScene()->isBussizeItem(pItem)) {
            vars << reinterpret_cast<long long>(pItem);
        }
    }
    CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                        CSceneUndoRedoCommand::EItemRemoved, vars, true);

    CUndoRedoCommand::finishRecord(true);

    //刷新多选图元
    drawScene()->getItemsMgr()->updateBoundingRect();
}

void CGraphicsView::slotOneLayerUp()
{
    if (!canLayerUp()) {
        return;
    }

    auto curScene = dynamic_cast<CDrawScene *>(scene());
    auto itemsMgr = curScene->getItemsMgr();
    QList<QGraphicsItem *> allItems = scene()->items();

    QList<QGraphicsItem *> selectedItems;
    if (itemsMgr->isVisible() && !itemsMgr->getItems().isEmpty()) {
        for (auto curItem : itemsMgr->getItems()) {
            selectedItems << curItem;
        }
    } else {
        QList<QGraphicsItem *> selectedList = scene()->selectedItems();
        QGraphicsItem *selectedItem = selectedList.first();
        selectedItems << selectedItem;
    }

    //防止未选择图元
    if (!selectedItems.isEmpty()) {
        QUndoCommand *command = new COneLayerUpCommand(curScene, selectedItems);
        this->pushUndoStack(command);
        updateCursorShape();
    }
}

void CGraphicsView::slotOneLayerDown()
{
    if (!canLayerDown()) {
        return;
    }
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    auto itemsMgr = curScene->getItemsMgr();
    QList<QGraphicsItem *> allItems = scene()->items();

    QList<QGraphicsItem *> selectedItems;
    if (itemsMgr->isVisible() && !itemsMgr->getItems().isEmpty()) {
        for (auto curItem : itemsMgr->getItems()) {
            selectedItems << curItem;
        }
    } else {
        QList<QGraphicsItem *> selectedList = scene()->selectedItems();
        QGraphicsItem *selectedItem = selectedList.first();
        selectedItems << selectedItem;
    }

    //防止未选择图元
    if (!selectedItems.isEmpty()) {
        QUndoCommand *command = new COneLayerDownCommand(curScene, selectedItems);
        this->pushUndoStack(command);

        updateCursorShape();
    }
}

void CGraphicsView::slotBringToFront()
{
    if (!canLayerUp()) {
        return;
    }

    auto curScene = dynamic_cast<CDrawScene *>(scene());
    auto itemsMgr = curScene->getItemsMgr();
    QList<QGraphicsItem *> allItems = scene()->items();

    QList<QGraphicsItem *> selectedItems;
    if (itemsMgr->isVisible() && !itemsMgr->getItems().isEmpty()) {
        for (auto curItem : itemsMgr->getItems()) {
            selectedItems << curItem;
        }
    } else {
        QList<QGraphicsItem *> selectedList = scene()->selectedItems();
        QGraphicsItem *selectedItem = selectedList.first();
        selectedItems << selectedItem;
    }

    //防止未选择图元
    if (!selectedItems.isEmpty()) {
        QUndoCommand *command = new CBringToFrontCommand(curScene, selectedItems);
        this->pushUndoStack(command);

        updateCursorShape();
    }
}

void CGraphicsView::slotSendTobackAct()
{
    if (!canLayerDown()) {
        return;
    }
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    auto itemsMgr = curScene->getItemsMgr();
    QList<QGraphicsItem *> allItems = scene()->items();

    QList<QGraphicsItem *> selectedItems;
    if (itemsMgr->isVisible() && !itemsMgr->getItems().isEmpty()) {
        for (auto curItem : itemsMgr->getItems()) {
            selectedItems << curItem;
        }
    } else {
        QList<QGraphicsItem *> selectedList = scene()->selectedItems();
        QGraphicsItem *selectedItem = selectedList.first();
        selectedItems << selectedItem;
    }

    //防止未选择图元
    if (!selectedItems.isEmpty()) {
        QUndoCommand *command = new CSendToBackCommand(curScene, selectedItems);
        this->pushUndoStack(command);
        updateCursorShape();
    }
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
//    QLineEdit *foucsLIneedit = qobject_cast<QLineEdit *>(dApp->focusObject());
//    if (foucsLIneedit != nullptr) {
//        m_cutScence->setEnabled(false);
//        QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, dApp->keyboardModifiers());
//        dApp->sendEvent(dApp->focusObject(), &event);
//        m_cutScence->setEnabled(true);
//    } else {
//        static_cast<CDrawScene *>(scene())->doCutScene();
//        this->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::selection);
//    }
}

void CGraphicsView::slotRestContextMenuAfterQuitCut()
{
    setContextMenuAndActionEnable(true);
}

void CGraphicsView::slotViewZoomIn()
{
    zoomIn(EViewCenter);
}

void CGraphicsView::slotViewZoomOut()
{
    zoomOut(EViewCenter);
}

void CGraphicsView::slotViewOrignal()
{
    this->scale(1.0);
    emit signalSetScale(m_scale);
}

void CGraphicsView::slotOnTextCut()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doCut();
        }
    }
}

void CGraphicsView::slotOnTextCopy()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doCopy();
        }
    }
}

void CGraphicsView::slotOnTextPaste()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doPaste();
        }
    }
}

void CGraphicsView::slotOnTextSelectAll()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doSelectAll();
        }
    }
}

void CGraphicsView::slotSetTextAlignment(const Qt::Alignment &align)
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->setSelectTextBlockAlign(align);
        }
    }
}

void CGraphicsView::slotOnTextUndo()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doUndo();
        }
    }
}

void CGraphicsView::slotOnTextRedo()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doRedo();
        }
    }
}

void CGraphicsView::slotOnTextDelete()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
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
    scene()->clear();
    auto curScene = static_cast<CDrawScene *>(scene());
    curScene->initScene();
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
    DFileDialog dialog(this);
    if (type) {
        dialog.setWindowTitle(tr("Save"));
    } else {
        dialog.setWindowTitle(tr("Save as"));
    }//设置文件保存对话框的标题
    dialog.setAcceptMode(QFileDialog::AcceptSave);//设置文件对话框为保存模式
    dialog.setOptions(QFileDialog::DontResolveSymlinks);//只显示文件夹
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
            if (!dApp->isFileNameLegal(path)) {
                //不支持的文件名
                DDialog dia(this);

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

void CGraphicsView::importData(const QString &path, bool isOpenByDDF)
{
    m_DDFManager->loadDDF(path, isOpenByDDF);
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

//void CGraphicsView::cleanUndoStack()
//{
//    m_pUndoStack->clear();
//}

bool CGraphicsView::getModify() const
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    return curScene->getModify();
}

void CGraphicsView::setModify(bool isModify)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    curScene->setModify(isModify);
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
    //    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
    //    CSelectTool *pSelectTool = dynamic_cast<CSelectTool *>(pTool);
    //    if (pSelectTool != nullptr) {
    //        pSelectTool->updateCursorShape();
    //    }
    drawScene()->refreshLook();
}

void CGraphicsView::setPaintEnable(bool b)
{
    doPaint = b;
    if (!b) {
        pix = QPixmap(this->viewport()->size() * devicePixelRatioF());
        pix.fill(QColor(0, 0, 0, 0));
        QPainter painter(&pix);
        pix.setDevicePixelRatio(devicePixelRatioF());
        painter.setPen(Qt::NoPen);
        painter.setRenderHint(QPainter::Antialiasing);
        this->scene()->render(&painter, QRectF(0, 0, pix.width(), pix.height()),
                              QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(pix.size().width(), pix.size().height()))), Qt::IgnoreAspectRatio);

        painter.drawRect(viewport()->rect());
    }
    viewport()->update();
}

bool CGraphicsView::isPaintEnable()
{
    return doPaint;
}

QPixmap &CGraphicsView::cachPixMap()
{
    return pix;
}

void CGraphicsView::showEvent(QShowEvent *event)
{
    this->setTransformationAnchor(AnchorViewCenter);
    QGraphicsView::showEvent(event);
}

void CGraphicsView::updateSelectedItemsAlignment(Qt::AlignmentFlag align)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> allItems = getSelectedValidItems();

    // [0] 没有选中的图元直接返回
    if (!allItems.size()) {
        return;
    }

    // [1] 用于记录保存图元的位置，便于撤销和返回
    QMap<CGraphicsItem *, QPointF> startPos;
    QMap<CGraphicsItem *, QPointF> endPos;

    // [2] 多个图元选中设置对齐方式
    if (allItems.size() > 1) {
        // [1] 筛选出预设置值,需要针对不同对齐方式设置不一样的初始值进行比较
        qreal leftTopAlignValue = 1000000;
        qreal rightBottompAlignValue = -1000000;
        qreal centerAlignValue = 0;
        for (int i = 0; i < allItems.size(); i++) {
            switch (align) {
            case Qt::AlignLeft: {
                leftTopAlignValue = leftTopAlignValue > allItems.at(i)->sceneBoundingRect().topLeft().x()
                                    ? allItems.at(i)->sceneBoundingRect().topLeft().x() : leftTopAlignValue;
                break;
            }
            case Qt::AlignHCenter: {
                // 获取水平中心点的位置
                centerAlignValue = curScene->getItemsMgr()->sceneBoundingRect().center().x();
                break;
            }
            case Qt::AlignRight: {
                rightBottompAlignValue = rightBottompAlignValue < allItems.at(i)->sceneBoundingRect().topRight().x()
                                         ? allItems.at(i)->sceneBoundingRect().topRight().x() : rightBottompAlignValue;
                break;
            }
            case Qt::AlignTop: {
                leftTopAlignValue = leftTopAlignValue > allItems.at(i)->sceneBoundingRect().topLeft().y()
                                    ? allItems.at(i)->sceneBoundingRect().topLeft().y() : leftTopAlignValue;
                break;
            }
            case Qt::AlignVCenter: {
                // 获取垂直中心点的位置
                centerAlignValue = curScene->getItemsMgr()->sceneBoundingRect().center().y();
                break;
            }
            case Qt::AlignBottom: {
                rightBottompAlignValue = rightBottompAlignValue < allItems.at(i)->sceneBoundingRect().bottomRight().y()
                                         ? allItems.at(i)->sceneBoundingRect().bottomRight().y() : rightBottompAlignValue;

                break;
            }
            default: {
                break;
            }
            }
        }

        // [1] 设置对齐坐标
        for (int i = 0; i < allItems.size(); i++) {
            startPos.insert(allItems.at(i), allItems.at(i)->sceneBoundingRect().topLeft());
            switch (align) {
            case Qt::AlignLeft: {
                qreal dx = allItems.at(i)->sceneBoundingRect().topLeft().x() - leftTopAlignValue;
                allItems.at(i)->moveBy(-dx, 0);
                break;
            }
            case Qt::AlignHCenter: {
                qreal dx = centerAlignValue - allItems.at(i)->sceneBoundingRect().center().x();
                allItems.at(i)->moveBy(dx, 0);
                break;
            }
            case Qt::AlignRight: {
                qreal dx = rightBottompAlignValue - allItems.at(i)->sceneBoundingRect().topRight().x();
                allItems.at(i)->moveBy(dx, 0);
                break;
            }
            case Qt::AlignTop: {
                qreal dy = allItems.at(i)->sceneBoundingRect().topLeft().y() - leftTopAlignValue;
                allItems.at(i)->moveBy(0, -dy);
                break;
            }
            case Qt::AlignVCenter: {
                qreal dy = centerAlignValue - allItems.at(i)->sceneBoundingRect().center().y();
                allItems.at(i)->moveBy(0, dy);
                break;
            }
            case Qt::AlignBottom: {
                qreal dy = rightBottompAlignValue - allItems.at(i)->sceneBoundingRect().bottomRight().y();
                allItems.at(i)->moveBy(0, dy);
                break;
            }
            default: {
                break;
            }
            }
            endPos.insert(allItems.at(i), allItems.at(i)->sceneBoundingRect().topLeft());
        }
    }

    // [3] 单个图元对齐方式
    if (allItems.size() == 1) {
        QPointF topLeft  = allItems.at(0)->scenRect().topLeft();
        QSizeF  size     = allItems.at(0)->sceneBoundingRect().size();
        QPointF topRight = topLeft + QPointF(size.width(), 0);
        QPointF botRight =  topLeft + QPointF(size.width(), size.height());
        startPos.insert(allItems.at(0), topLeft);
        switch (align) {
        case Qt::AlignLeft: {
            qreal dx = topLeft.x();
            allItems.at(0)->moveBy(-dx, 0);
            break;
        }
        case Qt::AlignHCenter: {
            qreal dx = scene()->width() / 2 - (topLeft.x()
                                               + size.width() / 2);
            allItems.at(0)->moveBy(dx, 0);
            break;
        }
        case Qt::AlignRight: {
            qreal dx = scene()->width() - topRight.x();
            allItems.at(0)->moveBy(dx, 0);
            break;
        }
        case Qt::AlignTop: {
            qreal dy = topLeft.y();
            allItems.at(0)->moveBy(0, -dy);
            break;
        }
        case Qt::AlignVCenter: {
            qreal dy = scene()->height() / 2 - (topLeft.y()
                                                + size.height() / 2);
            allItems.at(0)->moveBy(0, dy);
            break;
        }
        case Qt::AlignBottom: {
            qreal dy = scene()->height() - botRight.y();
            allItems.at(0)->moveBy(0, dy);
            break;
        }
        default: {
            break;
        }
        }
        endPos.insert(allItems.at(0), allItems.at(0)->scenRect().topLeft());
    }

    // [4] 设置出入栈
    QUndoCommand *addCommand = new CItemsAlignCommand(static_cast<CDrawScene *>(scene()), startPos, endPos);
    pushUndoStack(addCommand);
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

bool CGraphicsView::canLayerUp()
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    auto itemsMgr = curScene->getItemsMgr();

    if (itemsMgr->isVisible() && !itemsMgr->getItems().isEmpty()) {
        auto selectedItems = itemsMgr->getItems();
        qSort(selectedItems.begin(), selectedItems.end(), zValueSortASC);

        QList<QGraphicsItem *> allItems = scene()->items();
        for (int i = allItems.size() - 1; i >= 0; i--) {
            QGraphicsItem *allItem = allItems.at(i);
            if (allItem->zValue() == 0.0) {
                allItems.removeAt(i);
                continue;
            }
            if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
                allItems.removeAt(i);
                continue;
            }
            for (int j = 0; j < selectedItems.size(); j++) {
                QGraphicsItem *selectItem = selectedItems.at(j);
                if (allItem == selectItem) {
                    allItems.removeAt(i);
                    break;
                }
            }
        }

        if (allItems.size() < 1) {
            return false;
        }

        qSort(allItems.begin(), allItems.end(), zValueSortASC);
        if (selectedItems.last()->zValue() >= allItems.last()->zValue()) {
            return false;
        }

        return true;
    } else {
        QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
        QList<QGraphicsItem *> allItems = scene()->items(Qt::AscendingOrder);

        for (int i = allItems.size() - 1; i >= 0; i--) {
            if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
                allItems.removeAt(i);
            }
        }
        int trueItemCount = allItems.size();

        if (selectedItems.count() > 1 || selectedItems.count() <= 0 || trueItemCount <= 1) {
            return false;
        }
        QGraphicsItem *selectItem = selectedItems.first();
        qDebug() << "selectItem z = " << selectItem->zValue();
        if (selectItem->zValue() >= dynamic_cast<CDrawScene *>(scene())->getMaxZValue()) {
            return false;
        }

        return true;
    }
}

bool CGraphicsView::canLayerDown()
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    auto itemsMgr = curScene->getItemsMgr();
    if (itemsMgr->isVisible() && !itemsMgr->getItems().isEmpty()) {
        auto selectedItems = itemsMgr->getItems();
        qSort(selectedItems.begin(), selectedItems.end(), zValueSortASC);

        QList<QGraphicsItem *> allItems = scene()->items();
        for (int i = allItems.size() - 1; i >= 0; i--) {
            QGraphicsItem *allItem = allItems.at(i);
            if (allItem->zValue() == 0.0) {
                allItems.removeAt(i);
                continue;
            }
            if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
                allItems.removeAt(i);
                continue;
            }
            for (int j = 0; j < selectedItems.size(); j++) {
                QGraphicsItem *selectItem = selectedItems.at(j);
                if (allItem == selectItem) {
                    allItems.removeAt(i);
                    break;
                }
            }
        }

        if (allItems.size() < 1) {
            return false;
        }

        qSort(allItems.begin(), allItems.end(), zValueSortASC);
        if (allItems.first()->zValue() >= selectedItems.first()->zValue()) {
            return false;
        }
        return true;
    } else {
        QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
        QList<QGraphicsItem *> allItems = scene()->items(Qt::AscendingOrder);
        for (int i = allItems.size() - 1; i >= 0; i--) {
            if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
                allItems.removeAt(i);
            }
        }

        int trueItemCount = allItems.size();

        if (selectedItems.count() > 1 || selectedItems.count() <= 0 || trueItemCount <= 1) {
            return false;
        }

        qSort(&allItems.first(), &allItems.last(), zValueSortASC);
        QGraphicsItem *selectItem = selectedItems.first();
        if (selectItem->zValue() <= allItems.first()->zValue()) {
            return false;
        }

        return true;
    }
}

QList<CGraphicsItem *> CGraphicsView::getSelectedValidItems()
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> validItems = curScene->getItemsMgr()->getItems();
    // [0] 过滤错误图元
    for (int i = 0; i < validItems.size(); i++) {
        QGraphicsItem *allItem = validItems.at(i);
        if (allItem->zValue() == 0.0) {
            validItems.removeAt(i);
            continue;
        }
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            validItems.removeAt(i);
            continue;
        }
    }

    // [1] 判断当前是如果不是多选则是单选，后期优化多选策略后，此处代码可以删除掉
    if (!validItems.size()) {
        QList<QGraphicsItem *> items = scene()->selectedItems();
        // [2] 过滤错误图元
        for (int i = 0; i < items.size(); i++) {
            QGraphicsItem *item = items.at(i);
            if (item->zValue() == 0.0) {
                items.removeAt(i);
                continue;
            }
            if (item->type() <= QGraphicsItem::UserType || item->type() >= EGraphicUserType::MgrType) {
                items.removeAt(i);
                continue;
            }
            validItems.append(static_cast<CGraphicsItem *>(item));
        }
    }

    return validItems;
}

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
    }
}

void CGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
}

void CGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
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
            dApp->setApplicationCursor(Qt::ClosedHandCursor);
        }
    }
}

void CGraphicsView::mousePressEvent(QMouseEvent *event)
{
//    _pressBeginPos = event->pos();
//    _recordMovePos = _pressBeginPos;
    QGraphicsView::mousePressEvent(event);
}

void CGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void CGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void CGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (!event->isAutoRepeat()) {
            _spaceKeyPressed = true;
            _tempCursor = *qApp->overrideCursor();
            dApp->setApplicationCursor(Qt::ClosedHandCursor, true);
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void CGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (!event->isAutoRepeat()) {
            _spaceKeyPressed = false;
            updateCursorShape();
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
                        pScene->clearSelection();
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
    /*    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
            swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        else if (QGesture *pan = event->gesture(Qt::PanGesture))
            panTriggered(static_cast<QPanGesture *>(pan));
        else */if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}
void CGraphicsView::panTriggered(QPanGesture *gesture)
{
#ifndef QT_NO_CURSOR
    switch (gesture->state()) {
    case Qt::GestureStarted:
    case Qt::GestureUpdated:
        //setCursor(Qt::SizeAllCursor);
        break;
    default:
        //setCursor(Qt::ArrowCursor);
        break;
    }
#endif
    QPointF delta = gesture->delta();

    Q_UNUSED(delta);

    //horizontalScrollBar()->setValue(horizontalScrollBar()->value() + qRound(delta.x()));
    //verticalScrollBar()->setValue(verticalScrollBar()->value() + qRound(delta.x()));

    update();
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

void CGraphicsView::swipeTriggered(QSwipeGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished) {
        if (gesture->horizontalDirection() == QSwipeGesture::Left
                || gesture->verticalDirection() == QSwipeGesture::Up) {
            qDebug() << "swipeTriggered(): swipe to previous";
            //goPrevImage();
        } else {
            qDebug() << "swipeTriggered(): swipe to next";
            //goNextImage();
        }
        update();
    }
}





