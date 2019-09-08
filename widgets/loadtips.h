#ifndef LOADTIPS_H
#define LOADTIPS_H

#include <DDialog>
#include <DLabel>
#include <DWaterProgress>

DWIDGET_USE_NAMESPACE

class LoadTips : public DDialog
{
    Q_OBJECT
public:
    LoadTips(DWidget *parent = 0);
    ~LoadTips();

public:
    void showTips();

signals:
    void progressValueChanged(int value);
    void finishedPainting();

private:
    DWaterProgress *m_waterProg;
    DLabel *m_messageLabel;
    int m_counts;
};
#endif // LOADTIPS_H
