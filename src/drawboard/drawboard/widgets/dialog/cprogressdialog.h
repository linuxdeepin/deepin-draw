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

#include <globaldefine.h>

#include <QWidget>
#include <QProgressBar>

#include <QWidget>
#include <QLabel>

#ifdef USE_DTK
#include <DDialog>
DWIDGET_USE_NAMESPACE
#define PROGRESSSDIALOG_INHERIT DDialog
#define CABSTRACTPROCESSDIALOG_INHERIT DAbstractDialog
#else
#include <QDialog>
#define PROGRESSSDIALOG_INHERIT QDialog
#define CABSTRACTPROCESSDIALOG_INHERIT QDialog
#endif

class DRAWLIB_EXPORT ProgressDialog : PROGRESSSDIALOG_INHERIT
{
    Q_OBJECT
public:
    explicit ProgressDialog(const QString &text = "", QWidget *parent = nullptr);
    int exec() override;

public slots:
    void setText(const QString &text);
    void setProcess(int process, int total);

private:
    QProgressBar *m_progressBar;

private:
    void initUI();

private:
    QLabel *_titleLabel  = nullptr;
};

class CAbstractProcessDialog: public CABSTRACTPROCESSDIALOG_INHERIT
{
    Q_OBJECT
public:
    explicit CAbstractProcessDialog(QWidget *parent = nullptr);

public slots:
    void     setTitle(const QString &title);
    void     setProcess(int process);

protected:
    void     paintEvent(QPaintEvent *event);

protected:
    QLabel       *_titleLabel  = nullptr;
    QProgressBar *_progressBar = nullptr;
};

#endif // CPROGRESSDIALOG_H
