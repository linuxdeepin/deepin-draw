#ifndef TEMPFILE_H
#define TEMPFILE_H

#include <QObject>
#include <QWindow>

class TempFile : public QObject {
    Q_OBJECT
public:
    static TempFile *instance();

    QString getBlurFileName();

private:
    static TempFile* m_tempFile;
    TempFile(QObject* parent = 0);
    ~TempFile();

    QString m_blurFile;
};
#endif // TEMPFILE_H
