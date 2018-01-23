#include "loader.h"
#include "application.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QStorageInfo>
#include <QDebug>

#include "loadimagethread.h"
#include "utils/imageutils.h"

Loader *Loader::m_loader= NULL;

Loader *Loader::instance()
{
    if (!m_loader)
    {
        m_loader = new Loader();
    }

    return m_loader;
}

Loader::Loader(QObject *parent)
    : QObject(parent)
    , m_isRunning(false)
{
}

bool Loader::isRunning() const
{
    return m_isRunning;
}

void Loader::loadPaths(const QStringList &path, QPointF startPos,
                         qreal winWidth, qreal winHeight, QSize canvasSize)
{
    m_paths = path;
    if (m_paths.isEmpty())
        return;

    m_isRunning = true;
    emit importProgress(0);

    LoadImageThread* lt = new LoadImageThread(path, startPos, winWidth, winHeight, canvasSize);
    connect(lt, &LoadImageThread::loaded, this,  [=](int curCounts){
        emit importProgress(int((qreal(curCounts)/qreal(m_paths.length())*100)));
    });
    connect(lt, &LoadImageThread::finishedLoadingShapes,
            this, [=](Toolshapes shapes){
        emit imported(true);
        m_isRunning = false;
        emit finishedLoadShapes(shapes, lt->startPos());
        m_paths.clear();
        lt->deleteLater();
    });
    lt->start();
}
