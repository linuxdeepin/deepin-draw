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
    if (blurFile.open()) {
        m_blurFile = blurFile.fileName() + ".png";
    }
    qDebug() << "Blur File:" << m_blurFile;
}

TempFile::~TempFile() {}

QString TempFile::getBlurFileName()
{
    if (!m_blurFile.isEmpty()) {
        return m_blurFile;
    } else {
        return "/tmp/deepin-draw-blur.png";
    }
}

QString TempFile::getRandomFile(const QString &filepath)
{
    QString hashKey = createHash(filepath);
    QTemporaryFile randomFile;
    QString randomFilename;
    if (randomFile.open())
    {
        randomFilename = randomFile.fileName() + ".png";
        m_pathMap.insert(hashKey, randomFilename);
    }

    return randomFilename;
}
