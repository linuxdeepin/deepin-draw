#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <QWidget>

#include "drawshape/globaldefine.h"


class CPushButton;

class LineWidget : public QWidget
{
    Q_OBJECT
public:
    LineWidget(QWidget *parent = 0);
    ~LineWidget();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);

private:
    QList<CPushButton *> m_actionButtons;
    CPushButton *m_straightline;
    CPushButton *m_arrowline;

private:
    void clearOtherSelections(CPushButton *clickedButton);
    void initUI();
    void initConnection();
};

#endif // LINEWIDGET_H
