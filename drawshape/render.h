#pragma once

#include <QObject>
#include <QScopedPointer>

#include <QPixmap>
#include <QTime>

class Canvas;
class RenderPrivate;
class Render : public QObject
{
    Q_OBJECT
public:
    explicit Render(QObject *parent = 0);
    ~Render();

    qreal devicePixelRatio() const;
    void setDevicePixelRatio(qreal ratio);

    void setCanvas(Canvas *canvas);

    QPixmap grab();
Q_SIGNALS:
    void renderFinish(QPixmap pixmap);

public Q_SLOTS:
    void render();
    void onDataChanged(QTime timestamp);

private:
    QScopedPointer<RenderPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), Render)
};

