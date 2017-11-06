#ifndef SLIDERLABEL_H
#define SLIDERLABEL_H

#include <QLabel>
#include <QSlider>
#include <QWidget>

#include "colorslider.h"
#include "utils/baseutils.h"

class Slider : public QLabel {
    Q_OBJECT
public:
    Slider(QWidget* parent);
    ~Slider();

    void setAlphaValue(int value);
    int alphaValue();

signals:
    void valueChanged(int value);

private:
    QSlider* m_slider;
    QLabel* m_valueLabel;
};

class SliderLabel : public QLabel {
    Q_OBJECT
public:
    SliderLabel(QString text, DrawStatus status, QWidget* parent = 0);
    ~SliderLabel();

    void setTitle(const QString &text);
    int alpha ();
    void updateDrawStatus(DrawStatus status);

signals:
    void alphaChanged(int value);

private:
    QString m_text;
    DrawStatus m_drawStatus;

    QLabel* m_titleLabel;
    Slider* m_slider;
};

#endif // SLIDERLABEL_H
