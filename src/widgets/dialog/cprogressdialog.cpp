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
#include "cprogressdialog.h"
#include "application.h"
#include "mainwindow.h"

#include <DLabel>

#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

CProgressDialog::CProgressDialog(DWidget *parent)
    : DDialog(parent)
{
    initUI();
}

void CProgressDialog::initUI()
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

void CProgressDialog::slotupDateProcessBar(int value)
{
    m_progressBar->setValue(value);
}

void CProgressDialog::showProgressDialog(EProgressDialogType type, bool isOpenByDDF)
{
    if (SaveDDF == type) {
        _titleLabel->setText(tr("Saving..."));
    } else if (LoadDDF == type) {
        _titleLabel->setText(tr("Opening..."));
    }
    m_progressBar->reset();

    Q_UNUSED(isOpenByDDF)

    QMetaObject::invokeMethod(this, [ = ]() {
        QRect rct = drawApp->topMainWindow()->geometry();
        this->moveToCenterByRect(rct);
        this->show();
    }, Qt::QueuedConnection);

}

void CProgressDialog::showInCenter()
{
    QRect rect = qApp->desktop()->screenGeometry();

    QPoint center = rect.center();
    QPoint pos = center - QPoint(this->width() / 2, this->height() / 2);
    this->move(pos);

    show();

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
