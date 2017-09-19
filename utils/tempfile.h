#ifndef TEMPFILE_H
#define TEMPFILE_H

#include <QObject>
#include <QWindow>
#include <QMap>

class TempFile : public QObject
{
    Q_OBJECT
public:
    static TempFile *instance();

    QString getBlurFileName();

    QString getRandomFile(const QString &filepath);

private:
    static TempFile* m_tempFile;
    TempFile(QObject* parent = 0);
    ~TempFile();

    QString m_blurFile;
    QMap<QString , QString> m_pathMap;

};
#endif // TEMPFILE_H
