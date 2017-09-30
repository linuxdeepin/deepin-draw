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

    void setCanvasSize(QSize size);
    void updateCanvasSize(QSize size);

private:
    QLineEdit* m_widthLEdit;
    QLineEdit* m_heightLEdit;

    int m_artBoardWidth;
    int m_artBoardHeight;
};

#endif // ADJUSTSIZEWIDGET_H
