#ifndef HBOXLAYOUTWIDGET_H
#define HBOXLAYOUTWIDGET_H

#include <QWidget>

class QBoxLayout;
class BoxLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoxLayoutWidget(QWidget *parent = nullptr);
    BoxLayoutWidget(QColor c, QWidget *parent = nullptr);
    void setColor(QColor c);
    void addWidget(QWidget *w);
    void resetLayout(QBoxLayout *l);
protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    void init();
signals:

private:
    QColor  m_color;//背景色
    QBoxLayout  *m_layout = nullptr;
};

#endif // HBOXLAYOUTWIDGET_H
