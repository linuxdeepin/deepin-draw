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
class ProgressLayout;

class CCentralwidget: public DWidget
{
    Q_OBJECT
public:
    CCentralwidget(QWidget *parent = 0);
    ~CCentralwidget();
    CLeftToolBar *getLeftToolBar();
    void contextMenuEvent(QContextMenuEvent *event);

signals:
    void sendImageItem(QPixmap pixMap);
    void loadImageNum(int num);
    /**
     * @brief signalAttributeChangedFromScene 传递场景中选择图元改变信号
     */
    void signalAttributeChangedFromScene(bool, int);



public slots:
    void getPicPath(QStringList pathList);
    void addImageItem(QPixmap pixMap);
    void setProcessBarValue(int value);

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
    ProgressLayout *m_progressLayout;
    int m_picNum;
private:
    void initUI();
    void initContextMenu();
    void initConnect();

};

#endif // MAINWIDGET_H
