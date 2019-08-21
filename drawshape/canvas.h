#pragma once

#include <QObject>
#include <QScopedPointer>

class Shape;
class CanvasPrivate;
class Canvas
{
public:
    Canvas();
    ~Canvas();

    QList<Shape *> shapes();

    QSizeF size() const;
    void setSize(QSizeF sz);
    void clip(QRectF rect);

private:
    QScopedPointer<CanvasPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), Canvas)
};

