#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QPushButton>

class ToolButton : public QPushButton {
    Q_OBJECT
public:
    ToolButton(QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
    }

    ToolButton(QString text, QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
        setText(text);
    }
    ~ToolButton() {}
};

#endif // TOOLBUTTON_H
