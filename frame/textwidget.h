#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <QWidget>

#include "drawshape/globaldefine.h"

class TextWidget : public QWidget
{
    Q_OBJECT
public:
    TextWidget(QWidget *parent = 0);
    ~TextWidget();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void updateColorBtn();
    void resetColorBtns();
};

#endif // TEXTWIDGET_H
