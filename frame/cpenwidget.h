#ifndef CPENWIDGET_H
#define CPENWIDGET_H

#include <DWidget>

#include "drawshape/globaldefine.h"

class CSideWidthWidget;
class BorderColorButton;
class CPushButton;

DWIDGET_USE_NAMESPACE

class CPenWidget : public DWidget
{
    Q_OBJECT

public:
    CPenWidget(QWidget *parent = nullptr);
    ~CPenWidget();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);

private:
    CSideWidthWidget *m_sideWidthWidget;
    BorderColorButton *m_strokeButton;
    QList<CPushButton *> m_actionButtons;
    CPushButton *m_straightline;
    CPushButton *m_arrowline;

private:
    void initConnection();
    void initUI();
    void clearOtherSelections(CPushButton *clickedButton);
};

#endif // CPENWIDGET_H
