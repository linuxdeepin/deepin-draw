#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <QWidget>

#include "utils/baseutils.h"

class LineWidget : public QWidget
{
    Q_OBJECT
public:
    LineWidget(QWidget* parent = 0);
    ~LineWidget();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos);

};

#endif // LINEWIDGET_H
