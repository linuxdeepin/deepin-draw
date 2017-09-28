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
    void updateCanvasSize(int addX, int addY);

private:
    QLineEdit* m_widthLEdit;
    QLineEdit* m_heightLEdit;

    int m_artBoardWidth;
    int m_artBoardHeight;
};

#endif // ADJUSTSIZEWIDGET_H
