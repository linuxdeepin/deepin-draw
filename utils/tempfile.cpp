#include "tempfile.h"

#include <QTemporaryFile>
#include <QDebug>

#include "baseutils.h"

TempFile* TempFile::m_tempFile = nullptr;
TempFile* TempFile::instance()
{
    if (!m_tempFile)
    {
        m_tempFile = new TempFile();
    }

    return m_tempFile;
}

TempFile::TempFile(QObject *parent)
    : QObject(parent)
{
    QTemporaryFile blurFile;
    if (blurFile.open())
    {
        m_blurFile = blurFile.fileName() + ".png";
    }
    qDebug() << "Blur File:" << m_blurFile;
}

TempFile::~TempFile() {}

QString TempFile::getBlurFileName()
{
    if (!m_blurFile.isEmpty())
    {
        return m_blurFile;
    } else
    {
        return "/tmp/deepin-draw-blur.png";
    }
}

QString TempFile::getRandomFile(const QString &filepath,
                                const QString &imageFormat)
{
    QString hashKey = createHash(filepath);
    QTemporaryFile randomFile;
    QString randomFilename;
    if (randomFile.open())
    {
        if (imageFormat == ".png" || imageFormat == ".pdf")
            randomFilename = randomFile.fileName() + ".png";
        else
            randomFilename = randomFile.fileName() + imageFormat;
        m_pathMap.insert(hashKey, randomFilename);
    }

    return randomFilename;
}

void TempFile::setImageFile(QPixmap image)
{
    m_pixmap = image;
    emit saveDialogPopup();
}

void TempFile::setCanvasShapes(QList<Toolshape> shapes)
{
    m_shapes = shapes;
}

void TempFile::setSaveFinishedExit(bool exit)
{
    m_exit = exit;
}

bool TempFile::saveFinishedExit()
{
    return m_exit;
}

QPixmap TempFile::savedImage()
{
    return m_pixmap;
}

QList<Toolshape> TempFile::savedShapes()
{
    return m_shapes;
}
