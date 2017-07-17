#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include "widgets/toptoolbar.h"
class MainWidget: public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget();

private:
    TopToolbar* m_topToolbar;
    QWidget* m_mainWidget;
};

#endif // MAINWIDGET_H
