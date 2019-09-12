#ifndef CEMPTYWIDGET_H
#define CEMPTYWIDGET_H

#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class CTitleWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CTitleWidget(DWidget *parent = nullptr);
    ~CTitleWidget();

    void setTitle(const QString &title);

private:
    DLabel *m_title;
};

#endif // CEMPTYWIDGET_H
