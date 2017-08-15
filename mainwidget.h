#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QKeyEvent>

#include "widgets/toptoolbar.h"
#include "widgets/canvaswidget.h"
#include "utils/baseutils.h"
#include "utils/shapesutils.h"

class MainWidget: public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget();

    void setImageInCanvas(QString imageFileName);

signals:
    void initShapesWidget(QString shape);
    void shapesColorChanged(DrawStatus drawstatus, QColor color);
    void shapesLineWidthChanged(int linewidth);
    void lineShapeChanged(QString lineShape);
    void textFontsizeChanged(int fontsize);
    void blurLinewidthChanged(int linewidth);
    void rotateImage(const QString &path, int degree);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    TopToolbar* m_topToolbar;
    CanvasWidget* m_canvas;
    QLabel* m_seperatorLine;
    QVBoxLayout* m_vLayout;
};

#endif // MAINWIDGET_H
