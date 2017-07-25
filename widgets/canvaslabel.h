#ifndef CANVASLABEL_H
#define CANVASLABEL_H

#include <QLabel>
#include <QPaintEvent>

class CanvasLabel : public QLabel {
    Q_OBJECT
public:
    CanvasLabel(QWidget* parent = 0);
    ~CanvasLabel();

    void setCanvasPixmap(QString imageFile);
    void setCanvasPixmap(QPixmap pixmap);
    void zoomOutImage();
    void zoomInImage();

protected:
    void paintEvent(QPaintEvent* e);

private:
    QString m_currentFile;
    QPixmap m_currentPixmap;
};

#endif // CANVASLABEL_H
