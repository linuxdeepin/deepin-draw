#ifndef FILLSTYLEWIDGET_H
#define FILLSTYLEWIDGET_H
#include "styleattriwidget.h"
#include <DCheckBox>
DWIDGET_USE_NAMESPACE

class CColorSettingButton;
class ColorStyleWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit ColorStyleWidget(QWidget *parent = nullptr);
    virtual void setVar(const QVariant &var) override;
    void setTitleText(QString text);
    void setColorFill(int fillStyle);//0 fill 1 border
    void setEnableAttriVisible(bool bVisible);
    bool isEnableAttriVisible();
    bool isAttriApply();
    void setColorTextVisible(bool bVisible);
    void addWidget(QWidget *w);
signals:
    void colorChanged(const QColor &color, int phase);
private:
    void initUi();
    void enableColorEdit(bool bEnable);
private:
    CColorSettingButton *m_fillColor = nullptr;
    DLineEdit           *m_fillColorEdit = nullptr;
    DCheckBox           *m_bEnableAtrri = nullptr;
    DLabel              *m_titleLabel = nullptr;
    bool                m_bEnableAttriVisible = true;
    QHBoxLayout         *m_lFillColor = nullptr;
};

#endif // FILLSTYLEWIDGET_H
