#ifndef TEMPFILE_H
#define TEMPFILE_H

#include <QObject>
#include <QWindow>
#include <QMap>

#include "utils/shapesutils.h"

class TempFile : public QObject
{
    Q_OBJECT
public:
    static TempFile *instance();

    QString getBlurFileName();
    QString getRandomFile(const QString &filepath,
                          const QString &imageFormat = "PNG");

    void setImageFile(QList<QPixmap> images);
    void setCanvasShapes(QList<Toolshape> shapes);
    void setSaveFinishedExit(bool exit);

    bool saveFinishedExit();
    QList<QPixmap> savedImage();
    QList<Toolshape> savedShapes();

signals:
    void saveDialogPopup();

private:
    static TempFile* m_tempFile;
    TempFile(QObject* parent = 0);
    ~TempFile();

    QString m_blurFile;
    QMap<QString , QString> m_pathMap;
    QList<QPixmap> m_pixmaps;
    QList<Toolshape> m_shapes;
    bool m_exit = false;
};
#endif // TEMPFILE_H