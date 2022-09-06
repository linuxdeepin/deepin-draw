// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H

#include <DDialog>
#include <DWidget>
#include <DProgressBar>

#include <QWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ProgressDialog : public DDialog
{
    Q_OBJECT

public:
//    enum EProgressDialogType {
//        SaveDDF,
//        LoadDDF
//    };

public:
    explicit ProgressDialog(const QString &text = "", DWidget *parent = nullptr);
    //void showProgressDialog(EProgressDialogType type);
    int exec() override;

public slots:
    void setText(const QString &text);
    void setProcess(int process, int total);

private:
    DProgressBar *m_progressBar;

private:
    void initUI();

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
