#ifndef CGRAPHICSVIEW_H
#define CGRAPHICSVIEW_H

#include <DGraphicsView>
DWIDGET_USE_NAMESPACE

class CGraphicsView : public DGraphicsView
{
    Q_OBJECT
public:
    CGraphicsView(DWidget *parent = nullptr);
    void zoomOut();
    void zoomIn();
    void scale(qreal scale);

protected:
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

signals:
    void signalSetScale(const qreal scale);

private slots:
    void slotOnCut();
    void slotOnCopy();
    void slotOnPaste();
    void slotOnSelectAll();
    void slotOnDelete();
private:
    qreal m_scale; //记录当前缩放

    DMenu *m_contextMenu;//右键菜单
    QAction *m_cutAct;              //剪切
    QAction *m_copyAct;             //拷贝
    QAction *m_pasteAct;            //粘贴
    QAction *m_selectAllAct;        //全选
    QAction *m_deleteAct;           //删除
    QAction *m_undoAct;
    QAction *m_oneLayerUpAct;
    QAction *m_oneLayerDownAct;
    QAction *m_bringToFrontAct;
    QAction *m_sendTobackAct;
    QAction *m_leftAlignAct;
    QAction *m_topAlignAct;
    QAction *m_rightAlignAct;
    QAction *m_centerAlignAct;


private:
    void initContextMenu();
    void initContextMenuConnection();
};

#endif // CGRAPHICSVIEW_H
