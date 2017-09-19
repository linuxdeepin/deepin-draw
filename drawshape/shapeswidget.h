#ifndef SHAPESWIDGET_H
#define SHAPESWIDGET_H

#include <QFrame>
#include <QMouseEvent>

#include "utils/shapesutils.h"
#include "utils/baseutils.h"
#include "textedit.h"
#include "cutimagetips.h"

class ShapesWidget : public QFrame {
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
    void reloadEffectImg(QString effect);
    void requestScreenshot();
    void shapePressed(QString shape);
    void saveBtnPressed(int index);
    void requestExit();
    void menuNoFocus();
    void finishedDrawCut();
    void cutImage(QRect cutRect);

public:
    void initAttribute();
    void clearSelected();
    void setAllTextEditReadOnly();

    void handleDrag(QPointF oldPoint, QPointF newPoint);
    void handleRotate(QPointF pos);
    void handleResize(QPointF pos, int key);

    bool clickedOnShapes(QPointF pos);
    bool clickedOnImage(FourPoints rectPoints, QPointF pos, bool isFilled = false);
    bool clickedOnRect(FourPoints rectPoints, QPointF pos, bool isFilled = false);
    bool clickedOnEllipse(FourPoints mainPoints, QPointF pos, bool isFilled = false);
    bool clickedOnArrow(QList<QPointF> points, QPointF pos);
    bool clickedOnLine(FourPoints mainPoints,
                                         QList<QPointF> points, QPointF pos);
    bool clickedOnText(FourPoints mainPoints, QPointF pos);
    bool rotateOnImagePoint(FourPoints mainPoints, QPointF pos);
    bool rotateOnPoint(FourPoints mainPoints, QPointF pos);

    bool hoverOnShapes(Toolshape toolShape, QPointF pos);
    bool hoverOnRotatePoint(FourPoints mainPoints, QPointF pos);
    bool hoverOnImage(FourPoints rectPoints, QPointF pos);
    bool hoverOnRect(FourPoints rectPoints, QPointF pos, bool isTextBorder = false);
    bool hoverOnEllipse(FourPoints mainPoints, QPointF pos);
    bool hoverOnArrow(QList<QPointF> points, QPointF pos);
    bool hoverOnArbitraryCurve(FourPoints mainPoints, QList<QPointF> points, QPointF pos);
    bool hoverOnText(FourPoints mainPoints, QPointF pos);

    void deleteCurrentShape();
    QString  getCurrentType();
    void setLineStyle(int index);
    void showCutImageTips();
    void loadImage(QStringList paths);
    void compressToImage();

public slots:
    void updateSelectedShape(const QString &group, const QString &key);
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
    void setShiftKeyPressed(bool isShift);
    void updateCursorDirection(ResizeDirection direction);
    void updateCursorShape();
    void setImageCutting(bool cutting);

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *e);
    void keyPressEvent(QKeyEvent* e);

private:
    QPointF m_pos1 = QPointF(0, 0);
    QPointF m_pos2 = QPointF(0, 0);
    QPointF m_pos3, m_pos4;
    QPointF m_pressedPoint;
    QPointF m_movingPoint;

    QPixmap m_emptyBgPixmap;
    QPixmap m_backgroundPixmap;
    QLabel* m_rotateImageLabel;
    int m_bgContainShapeNum;
    QPointF m_startPos;

    bool m_isRecording;
    bool m_isMoving;
    bool m_isSelected;
    bool m_isPressed;
    bool m_isHovered;
    bool m_isRotated;
    bool m_isResize;
    bool m_isShiftPressed;
    bool m_editing;
    bool m_ownImages;
    bool m_rotateMode;

    ResizeDirection m_resizeDirection;
    ClickedKey m_clickedKey;
    QString m_currentType = "rectangle";

    int m_shapesIndex;
    int m_selectedIndex;
    int m_currentIndex;
    int m_hoveredIndex;
    int m_selectedOrder;
    bool m_blurEffectExist = false;
    bool m_mosaicEffectExist = false;
    bool m_clearAllTextBorder = false;
    bool m_imageCutting = false;

    QColor m_penColor;
    QColor m_brushColor;
    int m_linewidth;
    int m_textFontsize = 12;
    int m_blurLinewidth = 20;

    Toolshape m_cutShape;
    Toolshape m_currentShape;
    Toolshape m_selectedShape;
    Toolshape m_hoveredShape;

    QMap<int, TextEdit*> m_editMap;
    void updateTextRect(TextEdit* edit, QRectF newRect);
    Toolshapes m_shapes;
    QList<QPointF> m_imagePosList;
    CutImageTips* m_cutImageTips;
    QTimer* m_updateTimer;

    void paintShape(QPainter &painter, Toolshape shape, bool selected = false);
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
};
#endif // SHAPESWIDGET_H
