/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
