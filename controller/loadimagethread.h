#ifndef LOADIMAGETHREAD_H
#define LOADIMAGETHREAD_H

#include <QObject>
#include <QThread>
#include "utils/shapesutils.h"

class LoadImageThread : public QThread {
    Q_OBJECT
public:
    LoadImageThread(const QStringList &path,  QPointF startPos,
                    qreal winWidth, qreal winHeight, QSize canvasSize);
    ~LoadImageThread();

    bool isRunning() const;
    void run() Q_DECL_OVERRIDE;
    void loadImages(const QStringList &paths, QPointF startPos,
                    qreal winWidth, qreal winHeight, QSize canvasSize);
    QPointF startPos();

signals:
    void loaded(int counts);
    void finishedLoadingShapes(Toolshapes imageShapes);

private:
    QPointF m_imageStartPos;
    Toolshapes m_imageShapes;
    QStringList m_paths;
    QSize m_canvasSize;
    qreal m_winWidth;
    qreal m_winHeight;

    bool m_running;
};

#endif // LOADIMAGETHREAD_H
