#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>

class QWheelEvent;
class QPaintEvent;
class QFile;

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    enum RendererType { Native, OpenGL, Image };

    ImageView(QWidget *parent = 0);

    void setImage(const QString &path);
    void setRenderer(RendererType type = Native);
    void drawBackground(QPainter *p, const QRectF &rect);

public slots:
    void setHighQualityAntialiasing(bool highQualityAntialiasing);
    void setViewBackground(bool enable);
    void setViewOutline(bool enable);

protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    RendererType m_renderer;

    QGraphicsItem *m_svgItem;
    QGraphicsItem *m_pixmapItem;
    QGraphicsRectItem *m_backgroundItem;
    QGraphicsRectItem *m_outlineItem;

    QImage m_image;
};
#endif // IMAGEVIEW_H
