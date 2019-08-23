#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <DPushButton>

DWIDGET_USE_NAMESPACE

class ToolButton : public DPushButton
{
    Q_OBJECT
public:
    ToolButton(QWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
    }

    ToolButton(QString text, QWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
        setText(text);
    }
    ~ToolButton() {}

};

#endif // TOOLBUTTON_H

