#ifndef EXPANSIONPANEL_H
#define EXPANSIONPANEL_H

#include "toolbutton.h"

#include <QGraphicsDropShadowEffect>
#include <DBlurEffectWidget>
#include <DToolButton>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class ExpansionPanel : public DBlurEffectWidget
{
    Q_OBJECT

public:
    /**
      * @brief ExpansionPanel 构造函数
      * @param parent
      */
    explicit ExpansionPanel(QWidget *parent = nullptr);

    /**
      * @brief getGroupButton 获取组合按钮
      * @param parent
      */
    ToolButton *getGroupButton();

    /**
      * @brief getUngroupButton 获取释放组合按钮
      * @param parent
      */
    ToolButton *getUngroupButton();

    /**
      * @brief getUngroupButton 主题颜色改变
      * @param parent
      */
    void changeTheme();

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
    // 文字属性栏的组合按钮
    ToolButton *m_groupButton = nullptr;
    // 文字属性栏的释放组合按钮
    ToolButton *m_unGroupButton = nullptr;
    //阴影背景
    QGraphicsDropShadowEffect *shadow_effect = nullptr;
};

#endif // EXPANSIONPANEL_H
