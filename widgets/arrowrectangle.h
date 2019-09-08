#ifndef ARROWRECTANGLE_H
#define ARROWRECTANGLE_H

#include <darrowrectangle.h>

#include <QHideEvent>
#include <DWidget>


DWIDGET_USE_NAMESPACE
class ArrowRectangle : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit ArrowRectangle(ArrowDirection direction, DWidget *parent = nullptr);
    ~ArrowRectangle();

signals:
    void hideWindow();

protected:
    void hideEvent(QHideEvent *e);
};

#endif // ARROWRECTANGLE_H
