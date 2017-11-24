#ifndef ARROWRECTANGLE_H
#define ARROWRECTANGLE_H

#include <QHideEvent>

#include <darrowrectangle.h>

DWIDGET_USE_NAMESPACE
class ArrowRectangle : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit ArrowRectangle(ArrowDirection direction, QWidget * parent = 0);
    ~ArrowRectangle();

signals:
    void hideWindow();

protected:
    void hideEvent(QHideEvent* e);
};

#endif // ARROWRECTANGLE_H
