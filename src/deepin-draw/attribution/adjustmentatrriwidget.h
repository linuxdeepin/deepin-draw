#ifndef ADJUSTMENTATRRIWIDGET_H
#define ADJUSTMENTATRRIWIDGET_H

#include <QWidget>
#include "attributewidget.h"

class DPushButton;
class AdjustmentAtrriWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit AdjustmentAtrriWidget(QWidget *parent = nullptr);
    DIconButton *button();
    virtual void setVar(const QVariant &var) override;
private:
    void initUi();
signals:

private:
    DIconButton *m_adjustmentBtn = nullptr;
    QLabel *m_titleLabel = nullptr;
};

#endif // ADJUSTMENTATRRIWIDGET_H
