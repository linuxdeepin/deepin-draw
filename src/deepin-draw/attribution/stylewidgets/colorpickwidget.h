#ifndef COLORPICKWIDGET_H
#define COLORPICKWIDGET_H
#include <DArrowRectangle>

DWIDGET_USE_NAMESPACE
class ColorSettingWgt;

class ColorPickWidget : public DArrowRectangle
{
    Q_OBJECT
public:
    ColorPickWidget(QWidget *parent = nullptr);
    void  setVar(const QVariant &var);
    QWidget *caller();

    void setCaller(QWidget *pCaller);
signals:
    void colorChanged(const QColor &color, int phase);
protected:
    virtual void paintEvent(QPaintEvent *) override;
public slots:
    void setColor(const QColor &color, int phase);
private:
    QColor      m_color;
    ColorSettingWgt *m_colorSettingWgt;
    QWidget      *m_caller = nullptr;
};

#endif // COLORPICKWIDGET_H
