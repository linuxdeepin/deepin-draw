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

    if (QFileInfo(path).suffix() == "svg" && QSvgRenderer().load(path)) {
        m_svgItem = new QGraphicsSvgItem(path);
        m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
        m_svgItem->setCacheMode(QGraphicsItem::NoCache);
        m_svgItem->setZValue(0);
        loadSvg = true;

        m_backgroundItem = new QGraphicsRectItem(m_svgItem->boundingRect());
        m_outlineItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    } else {
        QVariantList vl = cachePixmap(path);
        QPixmap pixmap = vl.last().value<QPixmap>();
        m_pixmapItem = new QGraphicsPixmapItem(pixmap);
        m_pixmapItem->setZValue(0);
        loadSvg = false;

        m_backgroundItem = new QGraphicsRectItem(m_pixmapItem->boundingRect());
        m_outlineItem = new QGraphicsRectItem(m_pixmapItem->boundingRect());
    }

    m_imageLoaded = true;
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

    m_shapesWidget->resize(m_backgroundItem->rect().width(),
                                                    m_backgroundItem->rect().height());
    m_shapesWidget->move((width() - m_backgroundItem->rect().width())/2,
                                                   (height() - m_backgroundItem->rect().height())/2);
    m_shapesWidget->show();
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

void ImageView::updateShapesLineWidth(int linewidth) {
    m_shapesWidget->setLineWidth(linewidth);
}
void ImageView::updateLineShapes(QString lineShape) {
      m_shapesWidget->setCurrentShape(lineShape);
}
void ImageView::updateTextFontsize(int fontsize) {
    m_shapesWidget->setTextFontsize(fontsize);
}
void ImageView::updateBlurLinewidth(int linewidth) {
    m_shapesWidget->setBlurLinewidth(linewidth);
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
