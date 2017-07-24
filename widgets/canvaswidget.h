#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QLabel>

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

    void setImage(QString filename);

private:
    QLabel* m_canvasLabel;
    QString m_currentFile;
};

#endif // CANVASWIDGET_H
