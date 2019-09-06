#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <DWidget>

#include "drawshape/globaldefine.h"


class CSideWidthWidget;
class BorderColorButton;

DWIDGET_USE_NAMESPACE

class LineWidget : public DWidget
{
    Q_OBJECT
public:
    LineWidget(DWidget *parent = nullptr);
    ~LineWidget();

public slots:
    void updateLineWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalLineAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget;
    BorderColorButton *m_strokeBtn;

private:
    void initUI();
    void initConnection();
};

#endif // LINEWIDGET_H
