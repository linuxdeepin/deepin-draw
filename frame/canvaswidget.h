#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

#include "utils/baseutils.h"
#include "utils/shapesutils.h"
#include "drawshape/shapeswidget.h"

class CanvasWidget : public QLabel
{
    Q_OBJECT

public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

    void openImage(const QString &path);
    void setShapes(QList<Toolshape> shapes);

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
    void pressToShapeWidget(QMouseEvent *ev);
    void releaseToShapeWidget(QMouseEvent *ev);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;

private:
    QString m_currentFile;
    ShapesWidget* m_shapesWidget;

    qreal m_scaleValue;
    QSize m_scaledSize;
};

#endif // CANVASWIDGET_H
