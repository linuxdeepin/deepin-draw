#ifndef CSVGLABEL_H
#define CSVGLABEL_H

#include <DLabel>
#include <QSvgRenderer>
DWIDGET_USE_NAMESPACE
class QWidget;

class CSvgLabel : public DLabel
{
public:
    CSvgLabel(QString str = QString(), QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer m_svgRender;
};

#endif // CSVGLABEL_H
