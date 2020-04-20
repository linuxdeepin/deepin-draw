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
#include "drawshape/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "cundocommands.h"
#include "widgets/ctextedit.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include "drawshape/cdrawscene.h"
#include "utils/cddfmanager.h"
#include "drawshape/cgraphicsrectitem.h"
#include "drawshape/cgraphicsellipseitem.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicstriangleitem.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cgraphicsmasicoitem.h"
#include "drawshape/cgraphicspenitem.h"
#include "drawshape/cpictureitem.h"
#include "drawshape/cpicturetool.h"
#include "drawshape/cgraphicsitemselectedmgr.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"

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

//升序排列用
static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() < info2->zValue();
}
//降序排列用
static bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() > info2->zValue();
}

CGraphicsView::CGraphicsView(DWidget *parent)
    : DGraphicsView (parent)
    , m_scale(1)
    , m_isShowContext(true)
    , m_isStopContinuousDrawing(false)
{
    setOptimizationFlags(IndirectPainting);
    m_pUndoStack = new QUndoStack(this);
    m_DDFManager = new CDDFManager(this);

    initContextMenu();
    initContextMenuConnection();

    //文字右键菜单初始化
    initTextContextMenu();
    initTextContextMenuConnection();

    initConnection();
}

void CGraphicsView::zoomOut()
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
    scale(current_scale);
}

void CGraphicsView::zoomIn()
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
    scale(current_scale);
}

void CGraphicsView::scale(qreal scale)
{
    qreal multiple = scale / m_scale;
    DGraphicsView::scale(multiple, multiple);
    m_scale = scale;
    getDrawParam()->setScale(m_scale);
    emit signalSetScale(m_scale);
}

qreal CGraphicsView::getScale()
{
    return m_scale;
}


void CGraphicsView::wheelEvent(QWheelEvent *event)
{
    /*if (CDrawParamSigleton::GetInstance()->getCtlKeyStatus()) {
        if (event->delta() > 0) {
            zoomOut();

        } else {
            zoomIn();
        }
    }*/
}

void CGraphicsView::initContextMenu()
{
    m_contextMenu = new CMenu(this);
    m_contextMenu->setFixedWidth(182);

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

    //m_undoAct = m_contextMenu->addAction(tr("Undo"));
    m_undoAct = m_pUndoStack->createUndoAction(this, tr("Undo"));
    m_contextMenu->addAction(m_undoAct);
    m_undoAct->setShortcut(QKeySequence::Undo);
    this->addAction(m_undoAct);
    m_redoAct = m_pUndoStack->createRedoAction(this, tr("Redo"));
    m_contextMenu->addAction(m_redoAct);
    m_redoAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z));
    this->addAction(m_redoAct);
    m_contextMenu->addSeparator();

    m_oneLayerUpAct = new QAction(tr("Raise Layer"), this);
    m_contextMenu->addAction(m_oneLayerUpAct);
    m_oneLayerUpAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    this->addAction(m_oneLayerUpAct);

    m_oneLayerDownAct = new QAction(tr("Lower Layer"), this);
    m_contextMenu->addAction(m_oneLayerDownAct);
    m_oneLayerDownAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    this->addAction(m_oneLayerDownAct);

    m_bringToFrontAct = new QAction(tr("Layer to Top"), this);
    m_contextMenu->addAction(m_bringToFrontAct);
    m_bringToFrontAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_BracketRight));
    this->addAction(m_bringToFrontAct);

    m_sendTobackAct = new QAction(tr("Layer to Bottom"), this);
    m_contextMenu->addAction(m_sendTobackAct);
    m_sendTobackAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_BracketLeft));
    this->addAction(m_sendTobackAct);

    //    m_leftAlignAct = m_contextMenu->addAction(tr("Left align"));
    //    m_topAlignAct = m_contextMenu->addAction(tr("Top align"));
    //    m_rightAlignAct = m_contextMenu->addAction(tr("Right align"));
    //    m_centerAlignAct = m_contextMenu->addAction(tr("Center align"));


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

    //右键菜单隐藏时更新菜单选项层位操作可用，方便快捷键使用
    //    connect(m_contextMenu, &DMenu::aboutToHide, this, [ = ]() {
    //        m_bringToFrontAct->setEnabled(true);
    //        m_sendTobackAct->setEnabled(true);
    //        m_oneLayerUpAct->setEnabled(true);
    //        m_oneLayerDownAct->setEnabled(true);
    //    });
}

void CGraphicsView::initTextContextMenu()
{
    m_textMenu = new DMenu(this);
    m_textMenu->setFixedWidth(182);

    m_textCutAction = new QAction(tr("Cut"));
    m_textCopyAction = new QAction(tr("Copy"));
    m_textPasteAction = new QAction(tr("Paste"));
    m_textSelectAllAction = new QAction(tr("Select All"));

    QAction *deleteAct = new QAction(tr("Delete"));
    deleteAct->setEnabled(false);
    QAction *undoAct = new QAction(tr("Undo"));
    undoAct->setEnabled(false);

    QAction *fakeRaiseLayerAct = new QAction(tr("Raise Layer"));
    fakeRaiseLayerAct->setEnabled(false);
    QAction *fakeLowerLayerAct = new QAction(tr("Lower Layer"));
    fakeLowerLayerAct->setEnabled(false);
    QAction *fakeLayerToTopAct = new QAction(tr("Layer to Top"));
    fakeLayerToTopAct->setEnabled(false);
    QAction *fakeLayerToBottomAct = new QAction(tr("Layer to Bottom"));
    fakeLayerToBottomAct->setEnabled(false);

    m_textLeftAlignAct = new QAction(tr("Text Align Left"));
    //m_textTopAlignAct = new QAction(tr("Top Alignment"));
    m_textRightAlignAct = new QAction(tr("Text Align Right" ));
    m_textCenterAlignAct = new QAction(tr("Text Align Center"));

    m_textUndoAct = new QAction(tr("Undo"));
    m_textRedoAct = new QAction(tr("Redo"));

    m_textMenu->addAction(m_textCutAction);
    m_textMenu->addAction(m_textCopyAction);
    m_textMenu->addAction(m_textPasteAction);
    m_textMenu->addAction(m_textSelectAllAction);
    m_textMenu->addSeparator();

    m_textMenu->addAction(deleteAct);
    //m_textMenu->addAction(undoAct);
    m_textMenu->addAction(m_textUndoAct);
    m_textMenu->addAction(m_textRedoAct);

    m_textMenu->addSeparator();

    m_textMenu->addAction(fakeRaiseLayerAct);
    m_textMenu->addAction(fakeLowerLayerAct);
    m_textMenu->addAction(fakeLayerToTopAct);
    m_textMenu->addAction(fakeLayerToBottomAct);

    m_textMenu->addAction(m_textLeftAlignAct);
    //m_textMenu->addAction(m_textTopAlignAct);
    m_textMenu->addAction(m_textRightAlignAct);
    m_textMenu->addAction(m_textCenterAlignAct);
}

void CGraphicsView::initTextContextMenuConnection()
{
    connect(m_textCutAction, SIGNAL(triggered()), this, SLOT(slotOnTextCut()));
    connect(m_textCopyAction, SIGNAL(triggered()), this, SLOT(slotOnTextCopy()));
    connect(m_textPasteAction, SIGNAL(triggered()), this, SLOT(slotOnTextPaste()));
    connect(m_textSelectAllAction, SIGNAL(triggered()), this, SLOT(slotOnTextSelectAll()));


    connect(m_textLeftAlignAct, SIGNAL(triggered()), this, SLOT(slotOnTextLeftAlignment()));
    //connect(m_textTopAlignAct, SIGNAL(triggered()), this, SLOT(slotOnTextTopAlignment()));
    connect(m_textRightAlignAct, SIGNAL(triggered()), this, SLOT(slotOnTextRightAlignment()));
    connect(m_textCenterAlignAct, SIGNAL(triggered()), this, SLOT(slotOnTextCenterAlignment()));

    connect(m_textUndoAct, SIGNAL(triggered()), this, SLOT(slotOnTextUndo()));
    connect(m_textRedoAct, SIGNAL(triggered()), this, SLOT(slotOnTextRedo()));
}

void CGraphicsView::initConnection()
{
    qRegisterMetaType<SGraphicsTextUnitData>("SGraphicsTextUnitData");
    qRegisterMetaType<SGraphicsUnitHead>("SGraphicsUnitHead");
    connect(m_DDFManager, SIGNAL(signalClearSceneBeforLoadDDF()), this, SLOT(clearScene()));
    connect(m_DDFManager, SIGNAL(signalStartLoadDDF(QRectF)), this, SLOT(slotStartLoadDDF(QRectF)));
    connect(m_DDFManager, SIGNAL(signalAddItem(QGraphicsItem *)), this, SLOT(slotAddItemFromDDF(QGraphicsItem *)));
    connect(m_DDFManager, &CDDFManager::signalAddTextItem, this, [ = ](const SGraphicsTextUnitData & data,
    const SGraphicsUnitHead & head) {
        CGraphicsTextItem *item = new CGraphicsTextItem(data, head);
        slotAddItemFromDDF(item);
    });
    connect(m_DDFManager, &CDDFManager::signalSaveFileFinished, this, &CGraphicsView::signalSaveFileStatus);
    connect(m_DDFManager, SIGNAL(singalEndLoadDDF()), this, SIGNAL(singalTransmitEndLoadDDF()));
}



void CGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
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
            menuPos = QPoint(rx, ry);
            m_textMenu->move(menuPos);
            m_textMenu->show();
            m_visible = true;
            return;
        } else {
            m_copyAct->setEnabled(true);
            m_cutAct->setEnabled(true);
            m_deleteAct->setEnabled(true);
            m_bringToFrontAct->setVisible(true);
            m_sendTobackAct->setVisible(true);
            m_oneLayerUpAct->setVisible(true);
            m_oneLayerDownAct->setVisible(true);
        }
    } else {
        m_copyAct->setEnabled(false);
        m_cutAct->setEnabled(false);
        m_deleteAct->setEnabled(false);

        m_bringToFrontAct->setVisible(false);
        m_sendTobackAct->setVisible(false);
        m_oneLayerUpAct->setVisible(false);
        m_oneLayerDownAct->setVisible(false);
        m_visible = false;
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
        CShapeMimeData *data = dynamic_cast< CShapeMimeData *>( mp );
        if (data) {
            pasteFlag = true;
        }
    }

    m_pasteAct->setEnabled(pasteFlag);

    m_contextMenu->move(menuPos);
    m_undoAct->setEnabled(m_pUndoStack->canUndo());
    m_redoAct->setEnabled(m_pUndoStack->canRedo());
    //m_pasteAct->setEnabled(QApplication::clipboard()->ownsClipboard());
    //m_pasteAct->setEnabled(true);
    m_contextMenu->show();
}

void CGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
}

void CGraphicsView::paintEvent(QPaintEvent *event)
{
    DGraphicsView::paintEvent(event);
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
    qApp->setOverrideCursor(Qt::ArrowCursor);
}

//QPainter *CGraphicsView::sharedPainter() const
//{
//    return m_painter;
//}

void CGraphicsView::itemMoved(QGraphicsItem *item, const QPointF &newPosition)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    if ( item != nullptr) {
        QUndoCommand *moveCommand = new CMoveShapeCommand(curScene, item, newPosition);
        this->pushUndoStack(moveCommand);
    } else {
        QUndoCommand *moveCommand = new CMoveShapeCommand(curScene, newPosition);
        this->pushUndoStack(moveCommand);
    }
}

void CGraphicsView::itemAdded(QGraphicsItem *item)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<QGraphicsItem *> addItems;
    addItems.clear();
    addItems.append(item);
    item->setZValue(curScene->getMaxZValue() + 1);
    curScene->setMaxZValue(curScene->getMaxZValue() + 1);
    qDebug() << "CGraphicsView::itemAdded";
    QUndoCommand *addCommand = new CAddShapeCommand(curScene, addItems);
    this->pushUndoStack(addCommand);
}

void CGraphicsView::itemRotate(QGraphicsItem *item, const qreal newAngle)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *rotateCommand = new CRotateShapeCommand(curScene, item, newAngle);
    this->pushUndoStack(rotateCommand);
}

void CGraphicsView::itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *resizeCommand = new CResizeShapeCommand(curScene, item, handle, beginPos,  endPos, bShiftPress, bALtPress);
    this->pushUndoStack(resizeCommand);
}

void CGraphicsView::itemPropertyChange(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *setPropertyCommand = new CSetPropertyCommand(curScene, item, pen, brush, bPenChange, bBrushChange);
    this->pushUndoStack(setPropertyCommand);
}

void CGraphicsView::itemRectXRediusChange(CGraphicsRectItem *item, int xRedius, bool bChange)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *setRectXRediusCommand = new CSetRectXRediusCommand(curScene, item, xRedius, bChange);
    this->pushUndoStack(setRectXRediusCommand);
}

void CGraphicsView::itemPolygonPointChange(CGraphicsPolygonItem *item, int newNum)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *command = new CSetPolygonAttributeCommand(curScene, item, newNum);
    this->pushUndoStack(command);
}

void CGraphicsView::itemPenTypeChange(CGraphicsPenItem *item, bool isStart, ELineType newOldType)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *command = new CSetPenAttributeCommand(curScene, item, isStart, newOldType);
    m_pUndoStack->push(command);
}

void CGraphicsView::itemLineTypeChange(CGraphicsLineItem *item, bool isStart, ELineType newOldType)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *command = new CSetLineAttributeCommand(curScene, item, isStart, newOldType);
    m_pUndoStack->push(command);
}

void CGraphicsView::itemBlurChange(CGraphicsMasicoItem *item, int effect, int blurWidth)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *command = new CSetBlurAttributeCommand(curScene, item, effect, blurWidth);
    this->pushUndoStack(command);
}

void CGraphicsView::itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *item, int newNum, int newRadius)
{
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QUndoCommand *command = new CSetPolygonStarAttributeCommand(curScene, item, newNum, newRadius);
    this->pushUndoStack(command);
}

void CGraphicsView::slotStopContinuousDrawing()
{
    m_isStopContinuousDrawing = true;
}

void CGraphicsView::slotStartLoadDDF(QRectF rect)
{
    scene()->setSceneRect(rect);
}

void CGraphicsView::slotAddItemFromDDF(QGraphicsItem *item)
{
    scene()->addItem(item);
    itemAdded(item);
    if (item->type() == BlurType) {
        static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
    }

}


void CGraphicsView::slotOnCut()
{
    //    QList<QGraphicsItem *> itemList = scene()->selectedItems();
    //    if (itemList.isEmpty()) {
    //        return;
    //    }

    //    CShapeMimeData *data = new CShapeMimeData(itemList);
    //    data->setText("");
    //    QApplication::clipboard()->setMimeData(data);

    //    QUndoCommand *deleteCommand = new CRemoveShapeCommand(this->scene());
    //    m_pUndoStack->push(deleteCommand);

    //    if (!m_pasteAct->isEnabled()) {
    //        m_pasteAct->setEnabled(true);
    //    }

    QList<QGraphicsItem *> allItems;
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> seleteItems = curScene->getItemsMgr()->getItems();
    if (!seleteItems.isEmpty()) {
        for (QGraphicsItem *item : seleteItems) {
            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
                allItems.push_back(item);
            }
        }
        curScene->getItemsMgr()->hide();
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
    //升序排列
    qSort(allItems.begin(), allItems.end(), zValueSortASC);

    QUndoCommand *deleteCommand = new CDeleteShapeCommand(curScene, allItems);
    this->pushUndoStack(deleteCommand);

    CShapeMimeData *data = new CShapeMimeData(allItems);
    data->setText("");
    QApplication::clipboard()->setMimeData(data);

    if (!m_pasteAct->isEnabled()) {
        m_pasteAct->setEnabled(true);
    }
}

void CGraphicsView::slotOnCopy()
{
    //    if (scene()->selectedItems().isEmpty()) {
    //        return;
    //    }

    QList<QGraphicsItem *> allItems;
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> seleteItems = curScene->getItemsMgr()->getItems();
    if (!seleteItems.isEmpty()) {
        for (QGraphicsItem *item : seleteItems) {
            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
                allItems.push_back(item);
            }
        }
        curScene->getItemsMgr()->hide();
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

    CShapeMimeData *data = new CShapeMimeData( allItems );
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
            emit signalPastePixmap(pixmap);
        }
        qDebug() << "imageData" << imageData << endl;
    } else if (filePath != "") {
        //粘贴文件路径
        emit signalLoadDragOrPasteFile(filePath);
        //qDebug() << "filePath" << filePath << endl;
    } else {
        qDebug() << "mp->hasImage()"  << mp->hasImage() << endl;

        //粘贴画板内部图元
        CShapeMimeData *data = dynamic_cast< CShapeMimeData *>( mp );
        auto curScene = static_cast<CDrawScene *>(scene());
        if ( data ) {
            scene()->clearSelection();
            auto itemMgr = curScene->getItemsMgr();
            itemMgr->clear();
            //升序排列
            QList<CGraphicsItem *> allItems = data->itemList();
            qSort(allItems.begin(), allItems.end(), zValueSortASC);
            QList<QGraphicsItem *> addItems;
            addItems.clear();
            foreach (CGraphicsItem *item, allItems ) {
                CGraphicsItem *copy = nullptr;

                switch (item->type()) {
                case RectType:
                    copy = new CGraphicsRectItem();
                    break;
                case EllipseType:
                    copy = new CGraphicsEllipseItem();
                    break;
                case TriangleType:
                    copy = new CGraphicsTriangleItem();
                    break;
                case PolygonalStarType:
                    copy = new CGraphicsPolygonalStarItem();
                    break;

                case PolygonType:
                    copy = new CGraphicsPolygonItem();
                    break;
                case LineType:
                    copy = new CGraphicsLineItem();
                    break;

                case PenType:
                    copy = new CGraphicsPenItem();
                    break;
                case TextType:
                    copy = new CGraphicsTextItem();
                    break;

                case PictureType:
                    copy = new CPictureItem();
                    break;
                case BlurType:
                    copy = new CGraphicsMasicoItem();
                    break;

                }

                item->duplicate(copy);
                if ( copy ) {
                    //copy->setSelected(true);
                    itemMgr->addOrRemoveToGroup(copy);
                    if (itemMgr->getItems().size() > 1) {
                        CManagerAttributeService::getInstance()->showSelectedCommonProperty(curScene, itemMgr->getItems());
                    }
                    copy->moveBy(10, 10);
                    addItems.append(copy);
                }
            }
            QUndoCommand *addCommand = new CAddShapeCommand(curScene, addItems);

            this->pushUndoStack(addCommand);

            if (!itemMgr->getItems().isEmpty()) {
                itemMgr->show();
                itemMgr->setSelected(true);
            } else {
                itemMgr->hide();
            }
        }
    }

}

void CGraphicsView::slotOnSelectAll()
{
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    if (currentMode != selection) {
        return;
    }
    //    CDrawParamSigleton::GetInstance()->setSelectAllFlag(true);
    scene()->clearSelection();
    //    foreach (QGraphicsItem *item, scene()->items()) {
    //        if (item->type() > QGraphicsItem::UserType) {
    //            item->setSelected(true);
    //        }
    //    }

    auto curScene = dynamic_cast<CDrawScene *>(scene());
    foreach (QGraphicsItem *item, scene()->items()) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            auto curItem = static_cast<CGraphicsItem *>(item);
            curScene->getItemsMgr()->addToGroup(curItem);
        }
    }
    if (curScene->getItemsMgr()->getItems().size() > 1) {
        CManagerAttributeService::getInstance()->showSelectedCommonProperty(curScene, curScene->getItemsMgr()->getItems());
    }
    if (curScene->getItemsMgr()->getItems().isEmpty()) {
        return;
    }
    curScene->clearSelection();
    curScene->getItemsMgr()->show();
    curScene->getItemsMgr()->setSelected(true);

    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->updateBlurItem();

        curScene->changeAttribute(true, nullptr);
    }

    //    CDrawParamSigleton::GetInstance()->setSelectAllFlag(false);
}

void CGraphicsView::slotOnDelete()
{
    //    QList<QGraphicsItem *> seleteItems = scene()->selectedItems();
    //    QList<QGraphicsItem *> allItems;

    //    if (seleteItems.isEmpty()) {
    //        return;
    //    }

    //    for (QGraphicsItem *item : scene()->items(Qt::AscendingOrder)) {
    //        if (item->type() > QGraphicsItem::UserType ) {
    //            allItems.push_back(item);
    //        }

    //    }

    //    if (allItems.count() != seleteItems.count()) {
    //        if (seleteItems.count() > 1) {
    //            return;
    //        }
    //        allItems.clear();
    //        allItems.push_back(seleteItems.first());
    //    }
    //    auto curScene = dynamic_cast<CDrawScene *>(scene());

    QList<QGraphicsItem *> allItems;
    auto curScene = dynamic_cast<CDrawScene *>(scene());
    QList<CGraphicsItem *> seleteItems = curScene->getItemsMgr()->getItems();
    if (!seleteItems.isEmpty()) {
        for (QGraphicsItem *item : seleteItems) {
            if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
                allItems.push_back(item);
            }
        }
        curScene->getItemsMgr()->hide();
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

    QUndoCommand *deleteCommand = new CDeleteShapeCommand(curScene, allItems);
    this->pushUndoStack(deleteCommand);
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
    }
}

void CGraphicsView::slotQuitCutMode()
{
    static_cast<CDrawScene *>(scene())->quitCutMode();
}

void CGraphicsView::slotDoCutScene()
{
    static_cast<CDrawScene *>(scene())->doCutScene();
    //    qDebug() << "***************DoCut";
}

void CGraphicsView::slotRestContextMenuAfterQuitCut()
{
    setContextMenuAndActionEnable(true);
}

void CGraphicsView::slotViewZoomIn()
{
    zoomIn();
}

void CGraphicsView::slotViewZoomOut()
{
    zoomOut();
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

void CGraphicsView::slotOnTextTopAlignment()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doTopAlignment();
        }
    }
}

void CGraphicsView::slotOnTextRightAlignment()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doRightAlignment();
        }
    }
}

void CGraphicsView::slotOnTextLeftAlignment()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doLeftAlignment();
        }
    }
}

void CGraphicsView::slotOnTextCenterAlignment()
{
    if (!scene()->selectedItems().isEmpty()) {
        QGraphicsItem *item =  scene()->selectedItems().first();
        CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
        if (TextType == item->type() &&  tmpitem->isEditable()) {
            tmpitem->doCenterAlignment();
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
    if (ddfPath.isEmpty() || ddfPath == "") {
        showSaveDDFDialog(true, finishClose);
    } else {
        m_DDFManager->saveToDDF(ddfPath, scene(), finishClose);
        // 保存是否成功均等待信号触发后续事件
    }
}

void CGraphicsView::showSaveDDFDialog(bool type, bool finishClose)
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
    dialog.selectFile(getDrawParam()->viewName() + ".ddf"); //设置默认的文件名
    QStringList nameFilters;
    nameFilters << "*.ddf";
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    if (dialog.exec()) {
        QString path = dialog.selectedFiles().first();
        if (!path.isEmpty()) {
            if (path.split("/").last() == ".ddf" || QFileInfo(path).suffix().toLower() != ("ddf")) {
                path = path + ".ddf";
            }

            //再判断该文件是否正在被打开着的如果是那么就要提示不能覆盖
            if (CManageViewSigleton::GetInstance()->isDdfFileOpened(path)) {
                DDialog dia(this);
                dia.setModal(true);
                dia.setMessage(tr("Cannot save as \"%1\" because the document is currently open. Please save it with a different name, or close the document and try again.").arg(QFileInfo(path).fileName()));
                dia.setIcon(QPixmap(":/theme/common/images/deepin-draw-64.svg"));

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
        if (selectedItems.first()->zValue() >= allItems.last()->zValue()) {
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
        if (allItems.first()->zValue() >= selectedItems.last()->zValue()) {
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

//拖曳加载文件
void CGraphicsView::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasText()) {
        QList<QUrl> urls = e->mimeData()->urls();
        for (auto url : urls) {
            QString filePath = url.path();
            if (filePath != "") {
                emit signalLoadDragOrPasteFile(filePath);
            }
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
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->changeMouseShape(currentMode);
    }
}





