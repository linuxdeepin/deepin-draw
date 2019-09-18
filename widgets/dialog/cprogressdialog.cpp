#include "cprogressdialog.h"

#include <DLabel>

#include <QVBoxLayout>

CProgressDialog::CProgressDialog(DWidget *parent)
    : DDialog(parent)
{
    initUI();
}

void CProgressDialog::initUI()
{
    setFixedSize(QSize(400, 80));
    setModal(true);
    setCloseButtonVisible(false);

    m_label = new DLabel(this);
    m_label->setText(tr("正在导出"));

    m_progressBar = new DProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setAlignment(Qt::AlignHCenter);


    DWidget *widget = new DWidget(this);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addStretch();
    hLayout->addWidget(m_label);
    hLayout->addStretch();


    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addStretch();
    layout->addLayout(hLayout);
    layout->addWidget(m_progressBar);
    layout->addStretch();

    addContent(widget);
}

void CProgressDialog::slotupDateProcessBar(int value)
{
    m_progressBar->setValue(value);
}

void CProgressDialog::showProgressDialog(EProgressDialogType type)
{
    if (SaveDDF == type) {
        m_label->setText(tr("正在导出"));
    } else if (LoadDDF == type) {
        m_label->setText(tr("正在导入"));
    }
    m_progressBar->reset();

    show();
}
