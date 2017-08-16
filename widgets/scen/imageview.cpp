#include "imageview.h"

#include <QFileInfo>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QGraphicsPixmapItem>
#include <QPaintEvent>
#include <QSvgRenderer>
#include <QDebug>
#include <qmath.h>

#include "utils/baseutils.h"
#include "utils/imageutils.h"
#include "utils/tempfile.h"

#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
    , m_renderer(Native)
    , m_svgItem(0)
    , m_backgroundItem(0)
    , m_outlineItem(0)
    , m_imageLoaded(false)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

    // Prepare background check-board pattern
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);
}

void ImageView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void ImageView::setImage(const QString &path)
{
    qDebug() << "ImageView path:" << path;

    if (!QFileInfo(path).exists())
        return;

    QGraphicsScene *s = scene();

    bool drawBackground = (m_backgroundItem ? m_backgroundItem->isVisible() : false);
    bool drawOutline = (m_outlineItem ? m_outlineItem->isVisible() : true);

    s->clear();
    resetTransform();

    bool loadSvg = false;

    m_originRect = this->viewport()->rect();

    if (QFileInfo(path).suffix() == "svg" && QSvgRenderer().load(path))
    {
        m_svgItem = new QGraphicsSvgItem(path);
        m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
        m_svgItem->setCacheMode(QGraphicsItem::NoCache);
        m_svgItem->setZValue(0);
        loadSvg = true;
        m_imageRect = m_svgItem->boundingRect();

        m_backgroundItem = new QGraphicsRectItem(m_svgItem->boundingRect());
        m_outlineItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    } else {
        QVariantList vl = cachePixmap(path);
        QPixmap pixmap = vl.last().value<QPixmap>();
        m_pixmapItem = new QGraphicsPixmapItem(pixmap);
        m_pixmapItem->setZValue(0);
        loadSvg = false;
        m_imageRect = m_pixmapItem->boundingRect();

        qDebug() << "image size:" << pixmap.size() << m_pixmapItem->boundingRect() << this->viewport()->rect();
        m_backgroundItem = new QGraphicsRectItem(m_pixmapItem->boundingRect());
        m_outlineItem = new QGraphicsRectItem(m_pixmapItem->boundingRect());
    }


    m_imageLoaded = true;
    m_currentPath = path;
    m_backgroundItem->setBrush(Qt::white);
    m_backgroundItem->setPen(Qt::NoPen);
    m_backgroundItem->setVisible(drawBackground);
    m_backgroundItem->setZValue(-1);

    QPen outline(Qt::black, 2, Qt::DashLine);
    outline.setCosmetic(true);
    m_outlineItem->setPen(outline);
    m_outlineItem->setBrush(Qt::NoBrush);
    m_outlineItem->setVisible(drawOutline);
    m_outlineItem->setZValue(1);

    s->addItem(m_backgroundItem);
    if (loadSvg) {
        s->addItem(m_svgItem);
    } else {
        s->addItem(m_pixmapItem);
    }
    s->addItem(m_outlineItem);

    s->setSceneRect(m_outlineItem->boundingRect().adjusted(-10, -10, 10, 10));
}

void ImageView::setRenderer(RendererType type)
{
    m_renderer = type;

    if (m_renderer == OpenGL) {
#ifndef QT_NO_OPENGL
        setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    } else {
        setViewport(new QWidget);
    }
}

QRect ImageView::calculateImageScaledGeometry()
{
    m_sx = 1;
    m_sy = 1;
    const QTransform viewTransform = this->viewportTransform();
    m_sx = viewTransform.m11();
    m_sy = viewTransform.m22();

    qreal scaledWidth = m_originRect.width()*m_sx;
    qreal scaledHeight = m_originRect.height()*m_sx;
    qreal width = qreal(m_imageRect.width()/m_originRect.width())*scaledWidth;
    qreal height = qreal(m_imageRect.height()/m_originRect.height())*scaledHeight;

    return QRect(viewTransform.m31(), viewTransform.m32(), int(width), int(height));
}

void ImageView::setHighQualityAntialiasing(bool highQualityAntialiasing)
{
#ifndef QT_NO_OPENGL
    setRenderHint(QPainter::HighQualityAntialiasing, highQualityAntialiasing);
#else
    Q_UNUSED(highQualityAntialiasing);
#endif
}

void ImageView::setViewBackground(bool enable)
{
    if (!m_backgroundItem)
        return;

    m_backgroundItem->setVisible(enable);
}

void ImageView::setViewOutline(bool enable)
{
    if (!m_outlineItem)
        return;

    m_outlineItem->setVisible(enable);
}

void ImageView::initShapesWidget(QString shape)
{
    if (!m_imageLoaded)
        return;

    if (!m_shapesWidgetExist) {
        m_shapesWidget = new ShapesWidget(this);
        m_shapesWidgetExist = true;
    }

    m_shapesWidget->setCurrentShape(shape);
    QRect coordinateRect = calculateImageScaledGeometry();
    m_shapesWidget->resize(coordinateRect.width(), coordinateRect.height());
    m_shapesWidget->move(coordinateRect.x(), coordinateRect.y());
    m_shapesWidget->show();

    connect(m_shapesWidget, &ShapesWidget::reloadEffectImg, this,
            &ImageView::generateBlurEffect);
    connect(m_shapesWidget, &ShapesWidget::cutImage, this, [=]{
        QPixmap cutPixmap = this->grab(QRect(
             m_pixmapItem->boundingRect().x(), m_pixmapItem->boundingRect().y(),
             m_pixmapItem->boundingRect().width(), m_pixmapItem->boundingRect().height()));
        cutPixmap.save("/tmp/cut.png", "PNG");
    });
}

void ImageView::updateShapesColor(DrawStatus drawstatus, QColor color)
{
    if (!m_shapesWidgetExist)
        return;

    if (drawstatus == DrawStatus::Fill) {
        m_shapesWidget->setBrushColor(color);
    } else {
        m_shapesWidget->setPenColor(color);
    }
}

void ImageView::updateShapesLineWidth(int linewidth)
{
    m_shapesWidget->setLineWidth(linewidth);
}

void ImageView::updateLineShapes(QString lineShape)
{
      m_shapesWidget->setCurrentShape(lineShape);
}

void ImageView::updateTextFontsize(int fontsize)
{
    m_shapesWidget->setTextFontsize(fontsize);
}

void ImageView::updateBlurLinewidth(int linewidth)
{
    m_shapesWidget->setBlurLinewidth(linewidth);
}

void ImageView::rotateImage(const QString &path, int degree)
{
    qDebug() << "imageview rotateImage:" << path;
    utils::image::rotate(path, degree);
    setImage(path);
}

void ImageView::generateBlurEffect(const QString &type)
{
    if (!m_imageLoaded)
        return ;

    if (type == "blur")
    {
        QPixmap tmpPixmap;
        tmpPixmap = this->grab(
                    QRect(m_shapesWidget->x(), m_shapesWidget->y(),
                                m_shapesWidget->width(), m_shapesWidget->height()));

        const int radius = 10;
        int imgWidth = tmpPixmap.width();
        int imgHeight = tmpPixmap.height();
        if (!tmpPixmap.isNull()) {
            tmpPixmap = tmpPixmap.scaled(imgWidth/radius, imgHeight/radius,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeight,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            tmpPixmap.save(TempFile::instance()->getBlurFileName(), "PNG");
        }
    }
}

void ImageView::mirroredImage(bool horizontal, bool vertical) /*const*/
{
    qDebug() << "mirror image:" << horizontal << vertical;
    QImage originImage(m_currentPath);

    originImage = originImage.mirrored(horizontal,vertical);
    originImage.save("/tmp/abc.png", "PNG");
    this->setImage("/tmp/abc.png");
}

void ImageView::paintEvent(QPaintEvent *event)
{
    if (m_renderer == Image) {
        if (m_image.size() != viewport()->size()) {
            m_image = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        }

        QPainter imagePainter(&m_image);
        QGraphicsView::render(&imagePainter);
        imagePainter.end();

        QPainter p(viewport());
        p.drawImage(0, 0, m_image);

    } else {
        QGraphicsView::paintEvent(event);
    }
}

void ImageView:: wheelEvent(QWheelEvent *event)
{
    qreal factor = qPow(1.2, event->delta() / 240.0);
    scale(factor, factor);

    event->accept();
}
