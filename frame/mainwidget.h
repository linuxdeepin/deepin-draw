#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>

#include "toptoolbar.h"
#include "canvaswidget.h"
#include "utils/baseutils.h"
#include "utils/shapesutils.h"

class MainWidget: public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget();

signals:
    void shapePressed(QString shape);
    void drawShapeChanged(QString shape);
    void fillShapeSelectedActive(bool selected);
    void rotateImage(int degree);
    void mirroredImage(bool horizontal, bool vertical);
//    void saveImageAction(const QString &path);
    void generateSaveImage();
    void printImage();
    void autoCrop();

    void updateMiddleWidget(QString type);
    void adjustArtBoardSize(QSize size);
    void cutImageFinished();
    void pressToCanvas(QMouseEvent *ev);

public:
    void updateLayout();
    void openImage(const QString &path);
    void updateCanvasSize(const QSize &size);
    void initShapes(QList<Toolshape> shapes);
    const int shapeNum();

protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    TopToolbar* m_topToolbar;
    CanvasWidget* m_canvas;
    QLabel* m_seperatorLine;

    QVBoxLayout* m_vLayout;
    QHBoxLayout* m_hLayout;
};

#endif // MAINWIDGET_H
