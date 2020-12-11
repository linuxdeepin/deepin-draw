#ifndef EXPANSIONPANEL_H
#define EXPANSIONPANEL_H

#include <DBlurEffectWidget>
#include <DToolButton>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class ExpansionPanel : public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit ExpansionPanel(QWidget *parent = nullptr);

signals:
    /**
     * @description: signalItemGroup 组合框选图元信号
    */
    void signalItemGroup();


    /**
     * @description: signalItemGroup 释放组合框选图元信号
    */
    void signalItemgUngroup();


private:
    DToolButton *groupButton;
    DToolButton *unGroupButton;
};

#endif // EXPANSIONPANEL_H
