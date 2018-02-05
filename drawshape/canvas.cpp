#include "canvas.h"

#include <QSizeF>
#include <QRectF>

class CanvasPrivate
{
public:
    CanvasPrivate(Canvas *parent) : q_ptr(parent) {}

    QSizeF  size          = QSizeF(1920, 1080);

    Canvas *q_ptr;
    Q_DECLARE_PUBLIC(Canvas)
};

Canvas::Canvas() : d_ptr(new CanvasPrivate(this))
{

}

Canvas::~Canvas()
{

}

QList<Shape *> Canvas::shapes()
{
    return  {};
}

QSizeF Canvas::size() const
{
    Q_D(const Canvas);
    return d->size;
}

void Canvas::setSize(QSizeF sz)
{
    Q_D(Canvas);
    d->size = sz;
}

void Canvas::clip(QRectF rect)
{
    Q_D(Canvas);
    d->size = rect.size();
}
