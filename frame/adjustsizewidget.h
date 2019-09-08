#ifndef ADJUSTSIZEWIDGET_H
#define ADJUSTSIZEWIDGET_H

#include <DWidget>
#include <QLineEdit>

#include "widgets/fontsizelineedit.h"
DWIDGET_USE_NAMESPACE
class AdjustsizeWidget : public DWidget
{
    Q_OBJECT
public:
    AdjustsizeWidget(DWidget *parent = 0);
    ~AdjustsizeWidget();

    void setCanvasSize(QSize size);
    void updateCanvasSize(QSize size);

    void resizeCanvasSize(bool resized, QSize addSize);

signals:
    void autoCrop();

private:
    FontsizeLineEdit *m_widthLEdit;
    FontsizeLineEdit *m_heightLEdit;

    int m_artBoardWidth;
    int m_artBoardHeight;
};

#endif // ADJUSTSIZEWIDGET_H
