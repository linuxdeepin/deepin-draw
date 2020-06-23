#include "csvglabel.h"
#include <QWidget>
#include <QPainter>
CSvgLabel::CSvgLabel(QString str, QWidget *parent)
    : DLabel(parent)
    , m_svgRender(str)
{

}

void CSvgLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    m_svgRender.render(&painter);
}
