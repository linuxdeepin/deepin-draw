#ifndef PROGRESSLAYOUT_H
#define PROGRESSLAYOUT_H

#include <QHBoxLayout>
#include <DProgressBar>
#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ProgressLayout: public DWidget
{
public:
    ProgressLayout(DWidget *parent = nullptr);
    ~ProgressLayout();
    void setRange(int start, int end);
    void setProgressValue(int value);
    void showInCenter(DWidget *w);



    QVBoxLayout *m_progressVBoxLayout;
    DLabel *m_label;
    DLabel *m_progressLabel;
    DProgressBar *m_progressbar;
    int m_start;
    int m_end;

};

#endif // PROGRESSLAYOUT_H
