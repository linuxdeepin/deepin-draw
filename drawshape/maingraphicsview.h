#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>

class MainGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainGraphicsView(QGraphicsScene *scene = nullptr);


signals:

public slots:
//    void slotZoomIn();
//    void slotZoomOut();
};

#endif // MAINVIEW_H
