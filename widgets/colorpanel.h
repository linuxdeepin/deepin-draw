#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QPushButton>
#include <QWidget>
#include <QColor>
#include <QPaintEvent>

class ColorButton : public QPushButton {
    Q_OBJECT
public:
    ColorButton(const QColor &color, QWidget* parent = 0);
    ~ColorButton();

protected:
    void paintEvent(QPaintEvent *);

private:
    QColor m_color;
};

class ColorPanel : public QWidget {
    Q_OBJECT
public:
    ColorPanel(QWidget* parent = 0);
    ~ColorPanel();

private:

};

#endif // COLORPANEL_H
