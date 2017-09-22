#ifndef ADJUSTSIZEWIDGET_H
#define ADJUSTSIZEWIDGET_H

#include <QWidget>
#include <QLineEdit>

class AdjustsizeWidget : public QWidget
{
    Q_OBJECT
public:
    AdjustsizeWidget(QWidget* parent = 0);
    ~AdjustsizeWidget();

    void setCanvasSize(int width, int height);

private:
    QLineEdit* m_widthLEdit;
    QLineEdit* m_heightLEdit;
};

#endif // ADJUSTSIZEWIDGET_H
