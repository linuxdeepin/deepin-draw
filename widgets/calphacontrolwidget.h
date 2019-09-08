#ifndef CALPHACONTROLWIDGET_H
#define CALPHACONTROLWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class CAlphaControlWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CAlphaControlWidget(DWidget *parent = nullptr);
    void updateAlphaControlWidget(int alpha);
signals:
    void signalAlphaChanged(int);

private:
    DSlider *m_alphaSlider;
    DLabel *m_alphaLabel;
    bool m_isUserOperation;

private:
    void initUI();
    void initConnection();
};

#endif // CALPHACONTROLWIDGET_H
