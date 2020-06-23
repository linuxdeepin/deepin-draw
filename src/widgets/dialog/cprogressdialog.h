#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H

#include <DDialog>
#include <DWidget>
#include <DProgressBar>

#include <QWidget>
#include <DLabel>

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

private:
    DLabel *_titleLabel  = nullptr;
};

class CAbstractProcessDialog: public DAbstractDialog
{
    Q_OBJECT
public:
    explicit CAbstractProcessDialog(DWidget *parent = nullptr);

public slots:
    void     setTitle(const QString &title);
    void     setProcess(int process);

protected:
    void     paintEvent(QPaintEvent *event);

protected:
    DLabel       *_titleLabel  = nullptr;
    DProgressBar *_progressBar = nullptr;
};

#endif // CPROGRESSDIALOG_H
