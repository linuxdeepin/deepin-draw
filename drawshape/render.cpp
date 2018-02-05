#include "render.h"

#include <QDebug>
#include <QThread>
#include <QPainter>
#include <QMutex>

#include "canvas.h"

class RenderPrivate
{
public:
    RenderPrivate(Render *parent) : q_ptr(parent) {}

    void doRender();

    QMutex mutex;
    bool quit = false;
    QList<QTime> renderRequest;

    qreal   devicePixelRatio    = 1.0;
    Canvas *canvas = Q_NULLPTR;

    Render *q_ptr;
    Q_DECLARE_PUBLIC(Render)
};

Render::Render(QObject *parent) :
    QObject(parent), d_ptr(new RenderPrivate(this))
{

}

Render::~Render()
{

}

qreal Render::devicePixelRatio() const
{
    Q_D(const Render);
    return d->devicePixelRatio;
}

void Render::setDevicePixelRatio(qreal ratio)
{
    Q_D(Render);
    d->devicePixelRatio = ratio;
}

void Render::setCanvas(Canvas *canvas)
{
    Q_D(Render);
    d->canvas = canvas;
}

void Render::render()
{
    Q_D(Render);
    d->doRender();
}

void Render::onDataChanged(QTime timestamp)
{
    Q_D(Render);
    d->mutex.lock();
    d->renderRequest.push_back(timestamp);
    d->mutex.unlock();
}

void RenderPrivate::doRender()
{
    Q_Q(Render);
    Q_ASSERT(canvas);
    while (true) {
        mutex.lock();
        if (quit) {
            mutex.unlock();
            break;
        }

        if (renderRequest.isEmpty()) {
            mutex.unlock();
            QThread::msleep(30);
        } else {
            renderRequest.clear();
            mutex.unlock();

            QPixmap canvasPixmap(canvas->size().toSize()*devicePixelRatio);
            Q_EMIT q->renderFinish(canvasPixmap);
        }
    }
}
