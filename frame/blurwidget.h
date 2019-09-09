#ifndef BLURWIDGET_H
#define BLURWIDGET_H

#include <DWidget>


class CCheckButton;

DWIDGET_USE_NAMESPACE

class BlurWidget : public DWidget
{
    Q_OBJECT


public:
    BlurWidget(DWidget *parent = 0);
    ~BlurWidget();

private:
    QList<CCheckButton *> m_actionButtons;
    CCheckButton *m_blurBtn;
    CCheckButton *m_masicBtn;

private:
    void clearOtherSelections(CCheckButton *clickedButton);
    void initUI();
    void initConnection();
};

#endif // BLURWIDGET_H
