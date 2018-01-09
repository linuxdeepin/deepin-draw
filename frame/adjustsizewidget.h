#ifndef ADJUSTSIZEWIDGET_H
#define ADJUSTSIZEWIDGET_H

#include <QWidget>
#include <QLineEdit>

#include "widgets/fontsizelineedit.h"

class AdjustsizeWidget : public QWidget
{
    Q_OBJECT
public:
    AdjustsizeWidget(QWidget* parent = 0);
    ~AdjustsizeWidget();

    void setCanvasSize(QSize size);
    void updateCanvasSize(QSize size);

    void resizeCanvasSize(bool resized, QSize addSize);

signals:
    void autoCrop();

private:
    FontsizeLineEdit* m_widthLEdit;
    FontsizeLineEdit* m_heightLEdit;

    int m_artBoardWidth;
    int m_artBoardHeight;
};

#endif // ADJUSTSIZEWIDGET_H
