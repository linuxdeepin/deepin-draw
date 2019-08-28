#ifndef PROGRESSLAYOUT_H
#define PROGRESSLAYOUT_H

#include <QHBoxLayout>
#include <DProgressBar>
#include <QDialog>

class QLabel;
DWIDGET_USE_NAMESPACE

class ProgressLayout: public QDialog
{
public:
    ProgressLayout(QWidget *parent = nullptr);
    ~ProgressLayout();
    void setRange(int start, int end);
    void setProgressValue(int value);
    void layoutShow();



    QVBoxLayout *m_progressVBoxLayout;
    QLabel *m_label;
    QLabel *m_progressLabel;
    DProgressBar *m_progressbar;
    int m_start;
    int m_end;

};

#endif // PROGRESSLAYOUT_H
