#ifndef GROUPOPERATION_H
#define GROUPOPERATION_H

#include "expansionpanel.h"

#include <DIconButton>
#include <QHBoxLayout>
#include <DWidget>

class GroupOperation : public QWidget
{
    Q_OBJECT
public:
    explicit GroupOperation(QWidget *parent = nullptr);

    /**
     * @brief   setMode 设置组合操作的显示模式
     * @param   mode值为true显示组合、释放按钮，false显示下拉按钮
     */
    void setMode(bool mode);

    void creatGroupButton();

    void cancelGroupButton();

    void showExpansionPanel();

    void clearUi();

    QHBoxLayout *getLayout();

    ExpansionPanel *getExpansionPanel();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    //下拉按钮
    DIconButton *openGroup = nullptr;
    //组合
    DIconButton *groupButton = nullptr;
    //释放组合
    DIconButton *unGroupButton = nullptr;
    //扩展面板
    ExpansionPanel *panel = nullptr;

    QHBoxLayout *layout   = nullptr;

};

#endif // GROUPOPERATION_H
