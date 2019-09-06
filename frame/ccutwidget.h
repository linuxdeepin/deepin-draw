#ifndef AILORINGWIDGET_H
#define AILORINGWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE


class CCutWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CCutWidget(DWidget *parent = nullptr);
    ~CCutWidget();
signals:

public slots:

private:
    DPushButton *m_scaleBtn1_1;
    DPushButton *m_scaleBtn2_3;
    DPushButton *m_scaleBtn8_5;
    DPushButton *m_scaleBtn16_9;

    DPushButton *m_freeBtn;
    DPushButton *m_originalBtn;

private:
    void initUI();
    void initConnection();
};

#endif // AILORINGWIDGET_H
