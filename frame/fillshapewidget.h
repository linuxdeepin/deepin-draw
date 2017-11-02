#ifndef FILLSHAPEWIDGET_H
#define FILLSHAPEWIDGET_H

#include <QWidget>

#include "utils/baseutils.h"

class FillshapeWidget : public QWidget
{
    Q_OBJECT
public:
    FillshapeWidget(QWidget* parent = 0);
    ~FillshapeWidget();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible=true);

};

#endif // FILLSHAPEWIDGET_H
