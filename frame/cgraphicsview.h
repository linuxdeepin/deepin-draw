#ifndef CGRAPHICSVIEW_H
#define CGRAPHICSVIEW_H

#include <DGraphicsView>
#include "drawshape/csizehandlerect.h"
DWIDGET_USE_NAMESPACE

class QUndoStack;
class CGraphicsItem;
class CExportImageDialog;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;

class CGraphicsView : public DGraphicsView
{
    Q_OBJECT
public:
    CGraphicsView(DWidget *parent = nullptr);
    void zoomOut();
    void zoomIn();
    void scale(qreal scale);
    void showExportDialog();

protected:
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

signals:
    void signalSetScale(const qreal scale);

public slots:
    void itemMoved(QGraphicsItem *item, const QPointF &oldPosition );
    void itemAdded(QGraphicsItem *item );
    void itemRotate(QGraphicsItem *item, const qreal oldAngle );
    void itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress);
    void itemPropertyChange(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange);
    void itemPolygonPointChange(CGraphicsPolygonItem *item, int oldNum);
    void itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *item, int oldNum, int oldRadius);

private slots:
    void slotOnCut();
    void slotOnCopy();
    void slotOnPaste();
    void slotOnSelectAll();
    void slotOnDelete();
    void slotOneLayerUp();
    void slotOneLayerDown();
    void slotBringToFront();
    void slotSendTobackAct();

private:
    qreal m_scale; //记录当前缩放

    DMenu *m_contextMenu;//右键菜单
    QAction *m_cutAct;              //剪切
    QAction *m_copyAct;             //拷贝
    QAction *m_pasteAct;            //粘贴
    QAction *m_selectAllAct;        //全选
    QAction *m_deleteAct;   //删除
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_oneLayerUpAct;       //向上一层
    QAction *m_oneLayerDownAct;     //向下一层
    QAction *m_bringToFrontAct;     //置于最顶层
    QAction *m_sendTobackAct;       //置于最底层
    QAction *m_leftAlignAct;
    QAction *m_topAlignAct;
    QAction *m_rightAlignAct;
    QAction *m_centerAlignAct;

    QUndoStack *m_pUndoStack;

    CExportImageDialog *m_exportImageDialog;


private:
    void initContextMenu();
    void initContextMenuConnection();

};

#endif // CGRAPHICSVIEW_H
