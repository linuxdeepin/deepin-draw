#ifndef SHAPESWIDGET_H
#define SHAPESWIDGET_H

#include <QFrame>
#include <QMouseEvent>

#include "utils/shapesutils.h"
#include "utils/baseutils.h"
#include "textedit.h"
#include "cutimagetips.h"

class ShapesWidget : public QFrame
{
    Q_OBJECT

public:
    ShapesWidget(QWidget* parent = 0);
    ~ShapesWidget();

    enum ShapeBlurStatus {
        Drawing,
        Selected,
        Hovered,
        Normal,
    };

    enum ClickedKey {
        First,
        Second,
        Third,
        Fourth,
        Fifth,
        Sixth,
        Seventh,
        Eighth,
    };

signals:
    void requestScreenshot();
    void shapePressed(QString shape);
    void saveBtnPressed(int index);
    void requestExit();
    void menuNoFocus();
    void finishedDrawCut(QPointF pos);
    void updateMiddleWidgets(QString type);
    void adjustArtBoardSize(QSize size);
    void cutImageFinished();

public slots:
    void autoCrop();
    void clearSelected();
    void createBlurImage();
    void compressToImage();
    void deleteCurrentShape();

    QRect effectiveRect();
    QString  getCurrentType();
    QRect rightBottomRect();
    void loadImage(QStringList paths);
    void mirroredImage(bool horizontal, bool vertical);
    void setAllTextEditReadOnly();
    void setFillShapeSelectedActive(bool selected);
    void setLineStyle(int index);
    void showCutImageTips(QPointF pos);
    void saveImage();
    void updateCanvasSize();

    void updateSelectedShape(const QString &group, const QString &key);
    void setShapes(QList<Toolshape> shapes);
    void setCurrentShape(QString shapeType);
    void setPenColor(QColor color);
    void setBrushColor(QColor color);
    void setLineWidth(int linewidth);
    void setTextFontsize(int fontsize);
    void setBlurLinewidth(int linewidth);

    void saveActionTriggered();
    bool textEditIsReadOnly();

    void undoDrawShapes();
    void microAdjust(QString direction);

    void updateCursorDirection(ResizeDirection direction);
    void updateCursorShape();
    void setImageCutting(bool cutting);
    void cutImage();

    void updateCutShape(CutRation ration);
    void initShortcut();
    void handleDrag(QPointF oldPoint, QPointF newPoint);
    void handleImageRotate(int degree);
    void handleRotate(QPointF pos);
    void handleResize(QPointF pos, int key);

protected:
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    void resizeEvent(QEvent* e);
    void enterEvent(QEvent* e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent* e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent* e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* e) Q_DECL_OVERRIDE;

private:
    void initAttribute();
    void initCanvasSize();

    bool clickedOnShapes(QPointF pos);
    bool clickedOnImage(FourPoints rectPoints, QPointF pos);
    bool clickedOnCutImage(FourPoints rectPoints, QPointF pos);
    bool clickedOnRect(FourPoints rectPoints, QPointF pos, bool isFilled = false);
    bool clickedOnEllipse(FourPoints mainPoints, QPointF pos, bool isFilled = false);
    bool clickedOnArrow(QList<QPointF> points, QPointF pos);
    bool clickedOnLine(FourPoints mainPoints,
                       QList<QPointF> points, QPointF pos);
    bool clickedOnText(FourPoints mainPoints, QPointF pos);

    bool rotateOnImagePoint(FourPoints mainPoints, QPointF pos);
    bool rotateOnPoint(FourPoints mainPoints, QPointF pos);

    void hoverOnShapes(QPointF pos);
    bool hoverOnRotatePoint(FourPoints mainPoints, QPointF pos);
    bool hoverOnImage(FourPoints rectPoints, QPointF pos);
    bool hoverOnCutImage(FourPoints rectPoints, QPointF pos);
    bool hoverOnRect(FourPoints rectPoints, QPointF pos, bool isFilled = false);
    bool hoverOnEllipse(FourPoints mainPoints, QPointF pos, bool isFilled = false);
    bool hoverOnArrow(QList<QPointF> points, QPointF pos);
    bool hoverOnArbitraryCurve(FourPoints mainPoints, QList<QPointF> points, QPointF pos);
    bool hoverOnText(FourPoints mainPoints, QPointF pos);

    void paintShape(QPainter &painter, Toolshape shape, bool selected = false);
    void paintHoveredShape(QPainter &painter, Toolshape shape);
    void paintSelectedRect(QPainter &painter, FourPoints mainPoints);
    void paintSelectedImageRectPoints(QPainter &painter, FourPoints mainPoints);
    void paintSelectedRectPoints(QPainter &painter, FourPoints mainPoints);
    void paintImgPoint(QPainter &painter, QPointF pos, QPixmap img, bool isResize = true);
    void paintRect(QPainter &painter,  Toolshape shape);
    void paintEllipse(QPainter &painter, Toolshape shape);
    void paintArrow(QPainter &painter, Toolshape shape, bool isStraight = false);
    void paintStraightLine(QPainter &painter, Toolshape shape);
    void paintArbitraryCurve(QPainter &painter, Toolshape shape);
    void paintText(QPainter &painter,  Toolshape shape);

    QPainterPath drawPair(QPainter &p,
                          QPointF p1, QSizeF size1, QColor c1,
                          QPointF p2, QSizeF size2, QColor c2,
                          QPainterPath oldpath);
    void paintPointList(QPainter &p, QList<QPointF> points);
    void paintBlur(QPainter &painter, Toolshape shape);
    void paintCutImageRect(QPainter &painter, Toolshape shape);
    void paintImage(QPainter &painter, Toolshape imageShape);
    void paintSelectedShape(QPainter &painter, Toolshape shape);

    QPointF m_pos1 = QPointF(0, 0);
    QPointF m_pos2 = QPointF(0, 0);
    QPointF m_altCenterPos = QPointF(0, 0);
    QPointF m_pressedPoint;
    QPointF m_movingPoint;

    QPixmap m_emptyBgPixmap;
    QPixmap m_topPixmap;
    QPixmap m_middlePixmap;
    QPixmap m_bottomPixmap;

    int m_bgContainShapeNum;
    QPointF m_startPos;

    bool m_isRecording;
    bool m_isMoving;
    bool m_isSelected;
    bool m_isPressed;
    bool m_isHovered;
    bool m_isRotated;
    bool m_isResize;

    bool m_editing;
    bool m_needCompress;
    bool m_moveFillShape;

    bool m_isShiftPressed;
    bool m_isAltPressed;
    bool m_initAltStart;

    ResizeDirection m_resizeDirection;
    ClickedKey m_clickedKey;
    QString m_currentType = "";

    QString m_imageSavePath = "";
    bool m_scaledImage;
    bool m_stickCurosr;
    bool m_rationChanged;

    int m_shapesIndex;
    int m_selectedIndex;
    int m_currentIndex;
    int m_hoveredIndex;
    int m_selectedOrder;
    int m_cutImageOrder;
    int m_imagesCount;
    bool m_blurEffectExist;
    bool m_mosaicEffectExist;
    bool m_clearAllTextBorder;
    bool m_imageCutting;
    bool m_rotateImage;
    bool m_inBtmRight;
    bool m_saveWithRation;
    bool m_initCanvasSideLength;

    QColor m_penColor;
    QColor m_brushColor;
    int m_linewidth;
    int m_textFontsize = 12;
    int m_blurLinewidth = 20;

    Toolshape m_cutShape;
    Toolshape m_currentShape;
    Toolshape m_selectedShape;
    Toolshape m_hoveredShape;
    Toolshape m_beCutImageShape;

    void updateTextRect(TextEdit* edit, QRectF newRect);
    QMap<int, TextEdit*> m_editMap;
    Toolshapes m_shapes;
    QList<QPointF> m_imagePosList;
    CutImageTips* m_cutImageTips;
    QTimer* m_updateTimer;

    qreal m_artBoardActualWidth;
    qreal m_artBoardActualHeight;
    qreal m_artBoardWindowWidth;
    qreal m_artBoardWindowHeight;
    qreal m_canvasSideLength;
    qreal m_canvasMicroSideLength;
    qreal m_ration;
    qreal m_saveRation;
};
#endif // SHAPESWIDGET_H
