// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cprogressdialog.h"
#include "application.h"
#include "mainwindow.h"

#include <DLabel>

#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

ProgressDialog::ProgressDialog(const QString &text, DWidget *parent)
    : DDialog(parent)
{
    initUI();
    _titleLabel->setText(text);
}

void ProgressDialog::initUI()
{
    setFixedSize(QSize(400, 120));
    setModal(true);
    setCloseButtonVisible(false);

    m_progressBar = new DProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(8);


    DWidget *widget = new DWidget(this);
    _titleLabel = new DLabel(this);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(_titleLabel);
    layout->addWidget(m_progressBar);

    addContent(widget, Qt::AlignVCenter);
}

void ProgressDialog::setProcess(int process, int total)
{
    m_progressBar->setRange(0, total);
    m_progressBar->setValue(process);
}

int ProgressDialog::exec()
{
    m_progressBar->reset();

    if (!isVisible()) {
        if (parentWidget() != nullptr) {
            QRect rct = parentWidget()->window()->geometry();
            this->moveToCenterByRect(rct);
        }

        show();
    }
    return 0;
}

void ProgressDialog::setText(const QString &text)
{
    _titleLabel->setText(text);
}

void CAbstractProcessDialog::setTitle(const QString &title)
{
    _titleLabel->setText(title);
}

void CAbstractProcessDialog::setProcess(int process)
{
    _progressBar->setValue(process);
}

CAbstractProcessDialog::CAbstractProcessDialog(DWidget *parent): DAbstractDialog(parent)
{
    setFixedSize(QSize(480, 80));

    _titleLabel = new DLabel(this);
    _titleLabel->setGeometry(QRect(40, 20, 400, 24));
    _titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _titleLabel->setText(tr("Export"));

    _progressBar = new DProgressBar(this);
    _progressBar->setGeometry(QRect(40, 54, 400, 6));
    _progressBar->setRange(0, 100);

    setWindowModality(Qt::WindowModal);
}
void CAbstractProcessDialog::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);
//    QColor c(Qt::red);
//    c.setAlpha(255 * 80 / 100);
//    painter.setBrush(c);
//    painter.drawRect(rect());
    DAbstractDialog::paintEvent(event);
}
