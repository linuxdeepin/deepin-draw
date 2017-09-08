#ifndef IMPORTER_H
#define IMPORTER_H

#include <QThread>

class DirCollectThread : public QThread
{
    Q_OBJECT
public:
    DirCollectThread(const QString &root);
    void run() Q_DECL_OVERRIDE;
    void setStop(bool stop);
    const QString dir() const;

signals:
    void currentImport(const QString &path);
    void resultReady(const QStringList &files);

private:
    QString m_root;
    bool m_stop;
};

class FilesCollectThread : public QThread
{
    Q_OBJECT
public:
    FilesCollectThread(const QStringList &paths);
    void run() Q_DECL_OVERRIDE;
    void setStop(bool stop);

signals:
    void currentImport(const QString &path);
    void resultReady(const QStringList &files);

private:
    QStringList m_paths;
    bool m_stop;
};

class Importer : public QObject
{
    Q_OBJECT
public:
    static Importer *instance();
    explicit Importer(QObject* parent = 0);
    bool isRunning() const;
    void appendDir(const QString &path);
    void appendFiles(const QStringList &paths);
    void stop();
    void stopDirCollect(const QString &dir);

signals:
    void importedFiles(const QStringList &files);
    void currentImport(const QString &path);
    void imported(bool success);
    void progressChanged();

private:
    QList<QThread *> m_threads;
    QStringList m_dirs;
    static Importer* m_importer;
};
#endif // IMPORTER_H
