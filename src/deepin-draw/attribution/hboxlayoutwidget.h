#ifndef COMCOBOXGROUPWIDGET_H
#define COMCOBOXGROUPWIDGET_H
#include "attributewidget.h"
#include <DLineEdit>
#include <DLabel>
class QLabel;
/**
 * @brief 通用水平布局窗口，用于属性水平布局
 */
class HBoxLayoutWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit HBoxLayoutWidget(QWidget *parent = nullptr);
    // 设置属性控件的标题
    void setTitle(const QString &title);
    void addWidget(QWidget *widget, int stretch = 0);
private:
    void initUi();
private:
    DLabel             *m_label = nullptr;
    QHBoxLayout *       m_lay = nullptr;
};
#endif // COMCOBOXGROUPWIDGET_H
