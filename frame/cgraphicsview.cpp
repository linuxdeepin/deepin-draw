#include "cgraphicsview.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/cshapemimedata.h"
#include "drawshape/cgraphicsitem.h"

#include <DMenu>

#include <QAction>
#include <QWheelEvent>
#include <QClipboard>
#include <QApplication>

CGraphicsView::CGraphicsView(QWidget *parent)
    : DGraphicsView (parent)
    , m_scale(1)
{
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

    m_undoAct = m_contextMenu->addAction(tr("Undo"));
    m_contextMenu->addSeparator();
    m_oneLayerUpAct = m_contextMenu->addAction(tr("One layer up"));
    m_oneLayerDownAct = m_contextMenu->addAction(tr("One layer down"));
    m_bringToFrontAct = m_contextMenu->addAction(tr("Bring to front"));
    m_sendTobackAct = m_contextMenu->addAction(tr("Send to back"));
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
}

void CGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)

    m_contextMenu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
    m_contextMenu->show();
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
        item->setSelected(true);
    }
}

void CGraphicsView::slotOnDelete()
{
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
        scene()->removeItem(item);
    }
}
