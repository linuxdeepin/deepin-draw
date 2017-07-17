#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QLabel>

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

private:
    QLabel* m_canvasLabel;
};

#endif // CANVASWIDGET_H
