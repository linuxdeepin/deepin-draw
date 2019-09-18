#ifndef AILORINGWIDGET_H
#define AILORINGWIDGET_H

#include <DWidget>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE


class CCutWidget : public DWidget
{
    Q_OBJECT
public:


public:
    explicit CCutWidget(DWidget *parent = nullptr);
    ~CCutWidget();

    void updateCutSize();

signals:
    void signalCutAttributeChanged();

public slots:

private:
    DPushButton *m_scaleBtn1_1;
    DPushButton *m_scaleBtn2_3;
    DPushButton *m_scaleBtn8_5;
    DPushButton *m_scaleBtn16_9;

    DPushButton *m_freeBtn;
    DPushButton *m_originalBtn;

    DLineEdit *m_widthEdit;
    DLineEdit *m_heightEdit;

private:
    void initUI();
    void initConnection();
};

#endif // AILORINGWIDGET_H
