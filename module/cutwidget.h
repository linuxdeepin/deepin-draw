#ifndef CUTWIDGET_H
#define CUTWIDGET_H

#include <QWidget>

class CutWidget : public QWidget
{
    Q_OBJECT
public:
    CutWidget(QWidget* parent = 0);
    ~CutWidget();

signals:
    void rotateImage(int degree);
    void cutImage();
    void drawShapes(QString shape);
    void mirroredImage(bool hor, bool ver);

};
#endif // CUTWIDGET_H
