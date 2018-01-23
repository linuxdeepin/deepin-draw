#include "loadimagethread.h"

#include <QPixmap>
#include <QPixmapCache>
#include <QDebug>

#include "utils/baseutils.h"

const int PIC_SPACING = 20;
const int POINT_SPACING = 20;
const int IMG_ROTATEPOINT_SPACING = 45;
const QPointF START_POINT = QPointF(0, 45);


LoadImageThread::LoadImageThread(const QStringList &path, QPointF startPos,
    qreal winWidth, qreal winHeight, QSize canvasSize)
    : QThread(NULL)
    , m_running(false)
    , m_paths(path)
    , m_imageStartPos(startPos)
    , m_winWidth(winWidth)
    , m_winHeight(winHeight)
    , m_canvasSize(canvasSize)
{
}

LoadImageThread::~LoadImageThread()
{}

bool LoadImageThread::isRunning() const
{
    return m_running;
}

void LoadImageThread::run()
{
    loadImages(m_paths, m_imageStartPos, m_winWidth, m_winHeight, m_canvasSize);
}

void LoadImageThread::loadImages(const QStringList &paths, QPointF startPos,
    qreal winWidth, qreal winHeight, QSize canvasSize)
{
    m_running = true;
    m_imageShapes.clear();
     m_imageStartPos = startPos;
    qDebug() << "YU*" << startPos << winWidth << winHeight << canvasSize;
    for(int i = 0; i < paths.length(); i++)
    {
        if (QFileInfo(paths[i]).exists())
        {
            Toolshape imageShape;
            imageShape.type = "image";
            imageShape.imagePath = paths[i];
            QPixmap pixmap= QPixmap(paths[i]);
            QPixmapCache::setCacheLimit(10240*100);
            imageShape.imageSize = pixmap.size();

            if (paths.length() == 1 &&( imageShape.imageSize.width()
              == winWidth && imageShape.imageSize.height()
              + IMG_ROTATEPOINT_SPACING == winHeight))
            {
                m_imageStartPos = QPointF(0, IMG_ROTATEPOINT_SPACING);
                qDebug() << "loadImages:" << i+1 << QPixmapCache::insert(
                                createHash(paths[i]), pixmap);
            } else {
                if (imageShape.imageSize.width() > (winWidth - m_imageStartPos.x()) ||
                    imageShape.imageSize.height() > (winHeight - m_imageStartPos.y()))
                {
                    pixmap= QPixmap(paths[i]);
                    pixmap = pixmap.scaled(
                                int(std::abs(winWidth - 0)),
                                int(std::abs(winHeight - IMG_ROTATEPOINT_SPACING)),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    imageShape.imageSize = pixmap.size();
                    QPixmapCache::remove(createHash(paths[i]));
                    QPixmapCache::insert(createHash(paths[i]), pixmap);
                } else {
                    qDebug() << "loadImages:" << i+1 << QPixmapCache::insert(
                                    createHash(paths[i]), pixmap);
                }
            }

            if (paths.length() == 1)
            {
                m_imageStartPos = QPointF((canvasSize.width() - imageShape.imageSize.width())/2,
                (canvasSize.height() - imageShape.imageSize.height() - IMG_ROTATEPOINT_SPACING)/2
                 + IMG_ROTATEPOINT_SPACING);
            }

            imageShape.mainPoints[0] =  QPointF(m_imageStartPos.x(), m_imageStartPos.y());
            imageShape.mainPoints[1] = QPointF(m_imageStartPos.x(),
                m_imageStartPos.y() + imageShape.imageSize.height());
            imageShape.mainPoints[2] = QPointF(m_imageStartPos.x() +
                imageShape.imageSize.width(), m_imageStartPos.y());
            imageShape.mainPoints[3] = QPointF(m_imageStartPos.x() +
                imageShape.imageSize.width(), m_imageStartPos.y() +
                imageShape.imageSize.height());

            m_imageShapes.append(imageShape);

            emit loaded(i+1);
            m_imageStartPos = QPointF(m_imageStartPos.x() + PIC_SPACING,
                               m_imageStartPos.y() + PIC_SPACING);
//            m_imageStartPos = m_imageStartPos;
        }
    }

    if (!m_imageShapes.isEmpty()) {
        m_running = false;
        emit finishedLoadingShapes(m_imageShapes);
    }
}

QPointF LoadImageThread::startPos() {
    return m_imageStartPos;
}
