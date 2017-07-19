#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "widgets/toptoolbar.h"
#include "widgets/canvaswidget.h"

class MainWidget: public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    TopToolbar* m_topToolbar;
    CanvasWidget* m_canvas;
    QLabel* m_seperatorLine;
    QVBoxLayout* m_vLayout;
};

#endif // MAINWIDGET_H
