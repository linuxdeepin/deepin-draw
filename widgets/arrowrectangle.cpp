#include "arrowrectangle.h"

#include <application.h>

ArrowRectangle::ArrowRectangle(ArrowDirection direction, QWidget *parent)
    : DArrowRectangle(direction, parent)
{
    connect(dApp, &Application::focusChanged, this, [=](QWidget* old, QWidget* now){
        Q_UNUSED(old);
        if (now != this && !this->isAncestorOf(now))
        {
            hide();
        }
    });
}

void ArrowRectangle::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);
    emit hideWindow();
}

ArrowRectangle::~ArrowRectangle()
{}
