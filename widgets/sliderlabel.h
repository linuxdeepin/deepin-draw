#ifndef SLIDERLABEL_H
#define SLIDERLABEL_H

#include <QLabel>
#include <QSlider>

#include <QWidget>

class Slider : public QLabel {
    Q_OBJECT
public:
    Slider(QWidget* parent);
    ~Slider();
signals:
    void valueChanged();

private:
    QSlider* m_slider;
    QLabel* m_valueLabel;
};

class SliderLabel : public QLabel {
    Q_OBJECT
public:
    SliderLabel(QString text, QWidget* parent = 0);
    ~SliderLabel();

    void setTitle(const QString &text);

private:
    QString m_text;

    QLabel* m_titleLabel;
    Slider* m_slider;
};

#endif // SLIDERLABEL_H
