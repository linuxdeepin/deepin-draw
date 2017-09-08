#include "importer.h"
#include "application.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QStorageInfo>
#include <QDebug>

#include "utils/imageutils.h"

QStringList collectSubDirs(const QString &path)
{
    QStringList dirs;
    QDirIterator dirIterator(path,
                                                 QDir::Dirs | QDir::NoDotDot | QDir::NoDot,
                                                 QDirIterator::Subdirectories);

    while(dirIterator.hasNext()) {
        dirIterator.next();
        dirs.append(dirIterator.filePath());
    }
    return dirs;
}

Importer *Importer::m_importer = NULL;

Importer *Importer::instance()
{
    if (!m_importer) {
        m_importer = new Importer();
    }

    return m_importer;
}

Importer::Importer(QObject *parent)
    : QObject(parent)
{

}

bool Importer::isRunning() const
{
    return !m_threads.isEmpty();
}

void Importer::appendDir(const QString &path)
{
    if (m_dirs.contains(path))
        return;
    else
        m_dirs << path;

    emit progressChanged();
    emit currentImport(path);

    DirCollectThread* dt = new DirCollectThread(path);
    connect(dt, &DirCollectThread::resultReady, this, &Importer::importedFiles);
    connect(dt, &DirCollectThread::currentImport,
            this, &Importer::currentImport);
    connect(dt, &DirCollectThread::finished, this, [=]{
        m_threads.removeAll(dt);
        if (m_threads.isEmpty()) {
            emit imported(true);
            m_dirs.clear();
        }

        dt->deleteLater();
    });
    dt->start();
    m_threads.append(dt);
}

void Importer::appendFiles(const QStringList &paths)
{
    emit progressChanged();

    FilesCollectThread* ft = new FilesCollectThread(paths);
    //resultReady...
    connect(ft, &FilesCollectThread::currentImport,
            this, &Importer::currentImport);

    connect(ft, &FilesCollectThread::finished, this, [=]{
        m_threads.removeAll(ft);
        if (m_threads.isEmpty())
            emit imported(true);

        ft->deleteLater();
    });

    ft->start();
    m_threads.append(ft);
}

void Importer::stop()
{
    for(auto t : m_threads) {
        t->quit();
        t->wait();
        t->deleteLater();
    }

    emit imported(true);
}

void Importer::stopDirCollect(const QString &dir)
{
    for(auto t : m_threads) {
        DirCollectThread* dc = dynamic_cast<DirCollectThread*>(t);
        if (dc && dc->dir() == dir) {
            qDebug() << "Stoping dir collect thread..." << dir;
            dc->setStop(true);
            t->quit();
            t->wait();
            t->deleteLater();
        }
    }

    emit imported(true);
}

DirCollectThread::DirCollectThread(const QString &root)
    : QThread(NULL)
    , m_root(root)
    , m_stop(false)
{

}

void DirCollectThread::run()
{
    QStringList subDirs = collectSubDirs(m_root);
    QStringList paths;

    subDirs << m_root;
    using namespace utils::image;
    for(QString dir : subDirs) {
        QFileInfoList fileInfoList = getImagesInfo(dir, false);
        for(auto fi : fileInfoList) {
            if (m_stop) {
                return;
            }

            const QString path = fi.absoluteFilePath();
            paths << path;

            emit currentImport(path);
        }
    }

    emit resultReady(paths);
}

void DirCollectThread::setStop(bool stop)
{
    m_stop = stop;
}

const QString DirCollectThread::dir() const
{
    return m_root;
}

FilesCollectThread::FilesCollectThread(const QStringList &paths)
    : QThread(NULL)
    , m_paths(paths)
    , m_stop(false)
{

}

void FilesCollectThread::run()
{
    QStringList supportPaths;

    using namespace utils::image;

    for(auto path : m_paths) {
        if (m_stop) {
            return;
        }

        if (!imageSupportRead(path)) {
            continue;
        }

        supportPaths << path;
        emit currentImport(path);
    }

    emit resultReady(supportPaths);
}

void FilesCollectThread::setStop(bool stop)
{
    m_stop = stop;
}
