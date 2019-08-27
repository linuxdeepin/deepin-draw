#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DWidget>
#include <DLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>




DWIDGET_USE_NAMESPACE

class CLeftToolBar;
class QGraphicsView;
class CDrawScene;

class CCentralwidget: public DWidget
{
    Q_OBJECT
public:
    CCentralwidget(QWidget *parent = 0);
    ~CCentralwidget();
    CLeftToolBar *getLeftToolBar();
    void contextMenuEvent(QContextMenuEvent *event);


public slots:
    void getPicPath(QStringList path);
    void slotResetOriginPoint();
    void slotAttributeChanged();

private:
    CLeftToolBar *m_leftToolbar;
    DLabel *m_seperatorLine;

    QVBoxLayout *m_vLayout;
    QHBoxLayout *m_hLayout;

    int m_horizontalMargin;
    int m_verticalMargin;

    DMenu *m_contextMenu;
    QGraphicsView *m_pGraphicsView;
    CDrawScene *m_pDrawScene;

private:
    void initUI();
    void initContextMenu();
    void initConnect();

};

#endif // MAINWIDGET_H
