#include "progresslayout.h"
#include <DPalette>
//#include <DApplicationHelper>
DGUI_USE_NAMESPACE

ProgressLayout::ProgressLayout(DWidget *parent)
    : DWidget (parent)
{
    m_progressVBoxLayout = new QVBoxLayout();
    m_label = new DLabel();
    m_label->setText(QString("正在导入图片，请稍候"));
    QFont ft;
    ft.setPointSize(10);
    m_label->setFont(ft);
    //设置颜色
    DPalette pa1;
    pa1.setColor(DPalette::WindowText, Qt::black);
    m_label->setPalette(pa1);

    m_progressLabel = new DLabel();
    m_progressbar = new DProgressBar();
    m_progressbar->setFixedSize(400, 20);
    //m_progressbar->setTextVisiable(false);
    m_progressVBoxLayout->addWidget(m_label, 50, Qt::AlignLeft);
    m_progressVBoxLayout->addWidget(m_progressLabel, 50, Qt::AlignLeft);
    m_progressVBoxLayout->addWidget(m_progressbar, 100, Qt::AlignLeft);
    this->setFixedSize(420, 100);
    this->setLayout(m_progressVBoxLayout);
    this->setWindowFlags(Qt::FramelessWindowHint);

}

ProgressLayout::~ProgressLayout()
{

}
void ProgressLayout::showInCenter(DWidget *w)
{
    show();

    QPoint gp = w->mapToGlobal(QPoint(0, 0));
    move((w->width() - this->width()) / 2 + gp.x(),
         (w->height() - this->sizeHint().height()) / 2 + gp.y());
}

void ProgressLayout::setRange(int start, int end)
{
    m_start = start;
    m_end = end;
    m_progressbar->setMinimum(start);
    m_progressbar->setMaximum(end);
    m_progressbar->setValue(0);
}

void ProgressLayout::setProgressValue(int value)
{
    m_progressbar->setValue(value);
    m_progressLabel->setText(QString::fromLocal8Bit("已导入%1/%2张").arg(value).arg(m_end));
    //设置字号
    QFont ft2;
    ft2.setPointSize(8);
    m_progressLabel->setFont(ft2);

    //设置颜色
    DPalette pa;
    pa.setColor(DPalette::WindowText, Qt::blue);
    m_progressLabel->setPalette(pa);


}


