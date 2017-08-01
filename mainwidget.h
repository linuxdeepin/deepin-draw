#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QKeyEvent>

#include "widgets/toptoolbar.h"
#include "widgets/canvaswidget.h"
#include "utils/baseutils.h"

class MainWidget: public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget();

    void setImageInCanvas(QString imageFileName);

signals:
    void zoomOutAction();
    void zoomInAction();
    void prepareInitShapesWidget(QString shape, bool needInited);
    void changeShapesColor(DrawStatus drawstatus, QColor color);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    TopToolbar* m_topToolbar;
    CanvasWidget* m_canvas;
    QLabel* m_seperatorLine;
    QVBoxLayout* m_vLayout;
};

#endif // MAINWIDGET_H
