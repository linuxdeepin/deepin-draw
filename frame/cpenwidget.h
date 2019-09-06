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
    CPenWidget(DWidget *parent = nullptr);
    ~CPenWidget();

public slots:
    void updatePenWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalPenAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget;
    BorderColorButton *m_strokeBtn;
    QList<CPushButton *> m_actionButtons;
    CPushButton *m_straightline;
    CPushButton *m_arrowline;

private:
    void initConnection();
    void initUI();
    void clearOtherSelections(CPushButton *clickedButton);
};

#endif // CPENWIDGET_H
