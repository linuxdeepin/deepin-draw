#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>


#include "toptoolbar.h"
//#include "utils/baseutils.h"


class LeftToolBar;
class MainGraphicsView;
class MainGraphicsScene;
class QPixmap;

class MainWidget: public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();
    LeftToolBar *getLeftToolBar();
    void contextMenuEvent(QContextMenuEvent *event);
    //void importPicture(QString path);

public slots:
    void getPicPath(QStringList pathList);
    void addImageItem(QPixmap pixMap);
    void setProcessBarValue(int value);

signals:
    void sendImageItem(QPixmap pixMap);
    void loadImageNum(int num);

private:
    LeftToolBar *m_leftToolbar;
    MainGraphicsView *m_MainGraphicsView;
    MainGraphicsScene *m_MainGraphicsScene;
    QLabel *m_seperatorLine;

    QVBoxLayout *m_vLayout;
    QHBoxLayout *m_hLayout;

    int m_horizontalMargin;
    int m_verticalMargin;

    DMenu *m_contextMenu;
    DProgressBar *m_importProgressbar;



};

#endif // MAINWIDGET_H
