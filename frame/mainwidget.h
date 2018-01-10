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
#include "drawshape/shapeswidget.h"
#include "widgets/resizelabel.h"

class MainWidget: public QLabel
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
//    void pressToCanvas(QMouseEvent *ev);
    void resizeArtboard(bool resized, QSize size);

public:
    void updateLayout();
    void openImage(const QString &path);
    void updateCanvasSize(const QSize &size);
    void initShapes(QList<Toolshape> shapes);
    int shapeNum() const;

protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    TopToolbar* m_topToolbar;
//    CanvasWidget* m_canvas;
    ShapesWidget* m_shapesWidget;
    QLabel* m_seperatorLine;

    QVBoxLayout* m_vLayout;
    QHBoxLayout* m_hLayout;

    ResizeLabel* m_resizeLabel;
    bool m_drawArtboard;
    FourPoints m_artboardMPoints;
    int m_horizontalMargin;
    int m_verticalMargin;
};

#endif // MAINWIDGET_H
