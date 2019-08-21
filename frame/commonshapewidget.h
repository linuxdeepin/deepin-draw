#ifndef FILLSHAPEWIDGET_H
#define FILLSHAPEWIDGET_H

#include <DWidget>
#include "utils/baseutils.h"

DWIDGET_USE_NAMESPACE

class CommonshapeWidget : public DWidget
{
    Q_OBJECT
public:
    CommonshapeWidget(QWidget *parent = nullptr);
    ~CommonshapeWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus,
                        QPoint pos, bool visible = true);

};

#endif // FILLSHAPEWIDGET_H
