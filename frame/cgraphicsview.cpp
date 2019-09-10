#include "cgraphicsview.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/cshapemimedata.h"
#include "drawshape/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "cundocommands.h"
#include "widgets/dialog/cexportimagedialog.h"

#include <DMenu>

#include <QAction>
#include <QWheelEvent>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QUndoStack>

CGraphicsView::CGraphicsView(DWidget *parent)
    : DGraphicsView (parent)
    , m_scale(1)
{
    m_pUndoStack = new QUndoStack(this);
	m_exportImageDialog = new CExportImageDialog(this);
    initContextMenu();
    initContextMenuConnection();
}

void CGraphicsView::zoomOut()
{
    this->scale(1.1 * m_scale);
    emit signalSetScale(m_scale);
    CDrawParamSigleton::GetInstance()->setScale(m_scale);
}

void CGraphicsView::zoomIn()
{
    this->scale(0.9 * m_scale);
    emit signalSetScale(m_scale);
    CDrawParamSigleton::GetInstance()->setScale(m_scale);
}

void CGraphicsView::scale(qreal scale)
{
    qreal multiple = scale / m_scale;
    DGraphicsView::scale(multiple, multiple);
    m_scale = scale;
    CDrawParamSigleton::GetInstance()->setScale(m_scale);
}

void CGraphicsView::showExportDialog()
{
    QPixmap pixmap(scene()->sceneRect().width(), scene()->sceneRect().height());

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    scene()->render(&painter);

    m_exportImageDialog->showMe(pixmap);
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
    m_contextMenu = new DMenu(this);

    m_cutAct = new QAction(tr("Cut"));
    m_contextMenu->addAction(m_cutAct);
    m_cutAct->setShortcut(QKeySequence::Cut);
    this->addAction(m_cutAct);

    m_copyAct = new QAction(tr("Copy"));
    m_contextMenu->addAction(m_copyAct);
    m_copyAct->setShortcut(QKeySequence::Copy);
    this->addAction(m_copyAct);

    m_pasteAct = new QAction(tr("Paste"));
    m_contextMenu->addAction(m_pasteAct);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    this->addAction(m_pasteAct);

    m_selectAllAct = new QAction(tr("All"));
    m_contextMenu->addAction(m_selectAllAct);
    m_selectAllAct->setShortcut(QKeySequence::SelectAll);
    this->addAction(m_selectAllAct);

    m_contextMenu->addSeparator();

    m_deleteAct = new QAction(tr("Delete"));
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
    m_redoAct->setShortcut(QKeySequence::Redo);
    this->addAction(m_redoAct);
    m_contextMenu->addSeparator();

    m_oneLayerUpAct = new QAction(tr("One layer up"));
    m_contextMenu->addAction(m_oneLayerUpAct);
    m_oneLayerUpAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    this->addAction(m_oneLayerUpAct);

    m_oneLayerDownAct = new QAction(tr("One layer down"));
    m_contextMenu->addAction(m_oneLayerDownAct);
    m_oneLayerDownAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    this->addAction(m_oneLayerDownAct);

    m_bringToFrontAct = new QAction(tr("Bring to front"));
    m_contextMenu->addAction(m_bringToFrontAct);
    m_bringToFrontAct->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_BracketLeft));
    this->addAction(m_bringToFrontAct);

    m_sendTobackAct = new QAction(tr("Send to back"));
    m_contextMenu->addAction(m_sendTobackAct);
    m_sendTobackAct->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_BracketRight));
    this->addAction(m_sendTobackAct);

    m_leftAlignAct = m_contextMenu->addAction(tr("Left align"));
    m_topAlignAct = m_contextMenu->addAction(tr("Top align"));
    m_rightAlignAct = m_contextMenu->addAction(tr("Right align"));
    m_centerAlignAct = m_contextMenu->addAction(tr("Center align"));
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
}

void CGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)

    m_contextMenu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
    m_undoAct->setEnabled(m_pUndoStack->canUndo());
    m_redoAct->setEnabled(m_pUndoStack->canRedo());
    m_contextMenu->show();
}

void CGraphicsView::itemMoved(QGraphicsItem *item, const QPointF &oldPosition)
{
    if ( item != nullptr) {
        QUndoCommand *moveCommand = new CMoveShapeCommand(item, oldPosition);
        m_pUndoStack->push(moveCommand);
    }
}

void CGraphicsView::itemAdded(QGraphicsItem *item)
{
    QUndoCommand *addCommand = new CAddShapeCommand(item, item->scene());
    m_pUndoStack->push(addCommand);
}

void CGraphicsView::itemRotate(QGraphicsItem *item, const qreal oldAngle)
{
    QUndoCommand *rotateCommand = new CRotateShapeCommand(item, oldAngle);
    m_pUndoStack->push(rotateCommand);
}

void CGraphicsView::itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress)
{
    QUndoCommand *resizeCommand = new CResizeShapeCommand(item, handle, beginPos,  endPos, bShiftPress, bALtPress);
    m_pUndoStack->push(resizeCommand);
}

void CGraphicsView::slotOnCut()
{
    QList<QGraphicsItem *> itemList = scene()->selectedItems();
    CShapeMimeData *data = new CShapeMimeData(itemList);
    QApplication::clipboard()->setMimeData(data);

    foreach (QGraphicsItem *item, itemList) {
        scene()->removeItem(item);
    }
}

void CGraphicsView::slotOnCopy()
{
    CShapeMimeData *data = new CShapeMimeData( scene()->selectedItems() );
    QApplication::clipboard()->setMimeData(data);
}

void CGraphicsView::slotOnPaste()
{
    QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
    CShapeMimeData *data = dynamic_cast< CShapeMimeData *>( mp );
    if ( data ) {
        scene()->clearSelection();
        foreach (CGraphicsItem *item, data->itemList() ) {
            CGraphicsItem *copy = item->duplicate();
            if ( copy ) {
                copy->setSelected(true);
                copy->moveBy(10, 10);
                scene()->addItem(copy);
            }
        }
    }
}


void CGraphicsView::slotOnSelectAll()
{
    scene()->clearSelection();
    foreach (QGraphicsItem *item, scene()->items()) {
        if (item->type() > QGraphicsItem::UserType) {
            item->setSelected(true);
        }
    }
}

void CGraphicsView::slotOnDelete()
{
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
        scene()->removeItem(item);
    }
}

void CGraphicsView::slotOneLayerUp()
{
    QList<QGraphicsItem *> itemList = scene()->items();

    QList<QGraphicsItem *> selectedList = scene()->selectedItems();

    if (selectedList.length() <= 0 ||  itemList.length() == 1) {
        return;
    }

    QGraphicsItem *selectedItem = selectedList.first();

    int index = itemList.indexOf(selectedItem);

    for (int i = index - 1 ; i >= 0 ; i--) {
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            itemList.at(i)->stackBefore(selectedItem);
            break;
        }
    }


    scene()->update();
}

void CGraphicsView::slotOneLayerDown()
{
    QList<QGraphicsItem *> itemList = scene()->items();

    QList<QGraphicsItem *> selectedList = scene()->selectedItems();

    if (selectedList.length() <= 0 ||  itemList.length() == 1) {
        return;
    }

    QGraphicsItem *selectedItem = selectedList.first();

    int index = itemList.indexOf(selectedItem);

    for (int i = index + 1; i < itemList.length() ; i++) {

        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            selectedItem->stackBefore(itemList.at(i));
            break;
        }
    }

    scene()->update();
}

void CGraphicsView::slotBringToFront()
{
    QList<QGraphicsItem *> itemList = scene()->items();

    QList<QGraphicsItem *> selectedList = scene()->selectedItems();

    if (selectedList.length() <= 0 ||  itemList.length() == 1) {
        return;
    }

    QGraphicsItem *selectedItem = selectedList.first();

    int index = itemList.indexOf(selectedItem);

    for (int i = index - 1; i >= 0 ; i--) {
//        qDebug() << "@@@@@@@@@item=" << itemList.at(i)->type() << "zValue=" << "i=" << i << "::" << itemList.at(i)->zValue();
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            itemList.at(i)->stackBefore(selectedItem);
        }
    }


    scene()->update();
}

void CGraphicsView::slotSendTobackAct()
{
    QList<QGraphicsItem *> itemList = scene()->items();

    QList<QGraphicsItem *> selectedList = scene()->selectedItems();

    if (selectedList.length() <= 0 ||  itemList.length() == 1) {
        return;
    }

    QGraphicsItem *selectedItem = selectedList.first();

    int index = itemList.indexOf(selectedItem);

    for (int i = index + 1; i < itemList.length() ; i++) {
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            selectedItem->stackBefore(itemList.at(i));
        }
    }

    scene()->update();
}


