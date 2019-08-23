#ifndef BLURWIDGET_H
#define BLURWIDGET_H

#include <DWidget>


class CPushButton;

DWIDGET_USE_NAMESPACE

class BlurWidget : public DWidget
{
    Q_OBJECT


public:
    BlurWidget(QWidget *parent = 0);
    ~BlurWidget();

private:
    QList<CPushButton *> m_actionButtons;
    CPushButton *m_blurBtn;
    CPushButton *m_masicBtn;

private:
    void clearOtherSelections(CPushButton *clickedButton);
    void initUI();
    void initConnection();
};

#endif // BLURWIDGET_H
