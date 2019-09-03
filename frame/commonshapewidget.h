#ifndef FILLSHAPEWIDGET_H
#define FILLSHAPEWIDGET_H

#include <DWidget>
#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE


class BorderColorButton;
class BigColorButton;
class CSideWidthWidget;

class CommonshapeWidget : public DWidget
{
    Q_OBJECT
public:
    CommonshapeWidget(QWidget *parent = nullptr);
    ~CommonshapeWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalCommonShapeChanged();

public slots:
    void updateCommonShapWidget();

private:
    BigColorButton *m_fillBtn;
    BorderColorButton *m_strokeBtn;
    CSideWidthWidget *m_sideWidthWidget;

private:
    void initUI();
    void initConnection();
    QPoint getBtnPosition(const QPushButton *btn);
};

#endif // FILLSHAPEWIDGET_H
