#ifndef BASEUTILS_H
#define BASEUTILS_H

#include <QCursor>
#include <QFont>
#include <QLayout>
#include <QFontMetrics>
#include <QPainter>

#include "shapesutils.h"

#include <dscreenwindowsutil.h>

DWM_USE_NAMESPACE

enum ResizeDirection {
    Rotate,
    Moving,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Top,
    Bottom,
    Left,
    Right,
    Outting,
};

enum CutRation {
    Ration4_3,
    Ration8_5,
    Ration16_9,
    Ration1_1,
    FreeRation,
};

enum DrawStatus {
    Fill,
    Stroke,
};

enum MiddleWidgetStatus {
    Empty,
    Cut,
    DrawLine,
    FillShape,
    DrawText,
    DrawBlur,
    AdjustSize,
};

QList<QColor> specifiedColorList();
QColor       colorIndexOf(int index);
int                colorIndex(QColor color);

QSize      initArtboardSize(QPoint pos);
QSize      getArtboardSize(QPoint pos);
QSize      getCanvasSize(QSize artboardSize, QSize windowSize);

QCursor setCursorShape(QString cursorName);
int stringWidth(const QFont &f, const QString &str);
QString     getFileContent(const QString &file);
bool          isValidFormat(QString suffix);
bool          isCommandExist(QString command);
void  paintSelectedPoint(QPainter &painter, QPointF pos,
                                              QPixmap pointImg);
QVariantList cachePixmap(const QString &path);
QString DetectImageFormat(const QString &filepath);
QString allImageformat();
QString     createHash(const QString &str);
QString sizeToHuman(const qlonglong bytes);
QCursor blurToolCursor(const int &lineWidth);
QCursor pickColorCursor();

/* Convert shape info to jsonObject*/
QString getStringFromShape(Toolshape shape);
Toolshape getShapeInfoFromJsonStr(QString jsonStr);
#endif // BASEUTILS_H
