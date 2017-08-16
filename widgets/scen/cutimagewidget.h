#ifndef CUTIMAGEWIDGET_H
#define CUTIMAGEWIDGET_H

#include <QWidget>
#include <QLabel>

class CutImageWidget : public QWidget
{
    Q_OBJECT
public:
    CutImageWidget(QWidget *parent = 0);
    ~CutImageWidget();

    void showTips();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QLabel* m_settingTips;
};

#endif // CUTIMAGEWIDGET_H
