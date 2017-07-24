#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QScrollArea>
#include <QLabel>

class CanvasWidget : public QScrollArea {
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

    void setImage(QString filename);

    bool overWindowSize();

    QSize fitWindowScaledSize(QSize windowSize, QSize imgSize);

private:
    QLabel* m_canvasLabel;
    QString m_currentFile;

    qreal m_scaleValue;
    QSize m_scaledSize;
};

#endif // CANVASWIDGET_H
