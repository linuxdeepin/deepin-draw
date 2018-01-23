#ifndef LOADER_H
#define LOADER_H

#include <QObject>

#include "utils/shapesutils.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    static Loader *instance();
    explicit Loader(QObject* parent = 0);

    bool isRunning() const;
    void loadPaths(const QStringList &path, QPointF startPos,
                   qreal winWidth, qreal winHeight, QSize canvasSize);

signals:
    void importProgress(int counts);
    void imported(bool success);
    void finishedLoadShapes(Toolshapes shapes, QPointF pos);

private:
    bool m_isRunning;
    QStringList m_paths;
    static Loader* m_loader;
};
#endif // LOADER_H
