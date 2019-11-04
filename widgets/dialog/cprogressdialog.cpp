#include "cprogressdialog.h"

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

    //setTitle(tr("正在导出"));
    setTitle(tr("Exporting"));

    m_progressBar = new DProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setAlignment(Qt::AlignHCenter);
    m_progressBar->setFixedHeight(20);


    DWidget *widget = new DWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(widget);
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
        setTitle(tr("Exporting"));
    } else if (LoadDDF == type) {
        setTitle(tr("Importing"));
    }
    m_progressBar->reset();

    if (isOpenByDDF) {
        showInCenter();
    } else {
        show();

        QWidget *mainWindow = this->parentWidget()->parentWidget()->window();
        QPoint gp = mainWindow->mapToGlobal(QPoint(0, 0));
        move((mainWindow->width() - this->width()) / 2 + gp.x(),
             (mainWindow->height() - this->sizeHint().height()) / 2 + gp.y());
    }
}

void CProgressDialog::showInCenter()
{
    show();

    QRect rect = qApp->desktop()->screenGeometry();

    QPoint center = rect.center();
    QPoint pos = center - QPoint(this->width() / 2, this->height() / 2);
    this->move(pos);

}
