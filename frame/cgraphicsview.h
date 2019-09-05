#ifndef CGRAPHICSVIEW_H
#define CGRAPHICSVIEW_H

#include <DGraphicsView>
DWIDGET_USE_NAMESPACE

class CGraphicsView : public DGraphicsView
{
    Q_OBJECT
public:
    CGraphicsView(QWidget *parent = nullptr);
    void zoomOut();
    void zoomIn();
    void scale(qreal scale);

protected:
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

signals:
    void signalSetScale(const qreal scale);

private:
    qreal m_scale; //记录当前缩放
};

#endif // CGRAPHICSVIEW_H
