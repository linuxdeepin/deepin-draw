#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H

#include <DDialog>
#include <DWidget>
#include <DProgressBar>

#include <QWidget>

DWIDGET_USE_NAMESPACE

class CProgressDialog : public DDialog
{
    Q_OBJECT

public:
    enum EProgressDialogType {
        SaveDDF,
        LoadDDF
    };

public:
    explicit CProgressDialog(DWidget *parent = nullptr);
    void showProgressDialog(EProgressDialogType type, bool isOpenByDDF = false);

signals:

public slots:
    void slotupDateProcessBar(int);

private:
    DProgressBar *m_progressBar;


private:
    void initUI();
    void showInCenter();

};

#endif // CPROGRESSDIALOG_H
