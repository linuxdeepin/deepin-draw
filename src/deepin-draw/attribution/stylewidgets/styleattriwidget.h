#ifndef STYLEATTRIWIDGET_H
#define STYLEATTRIWIDGET_H
#include "attributewidget.h"
#include <DLineEdit>
#include <DLabel>
DWIDGET_USE_NAMESPACE
class QLabel;
class StyleAttriWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit StyleAttriWidget(int attri, QWidget *parent = nullptr);
    QVBoxLayout *getLayout();
private:
    void initUi();
public:
    StyleAttriWidget   *m_extend = nullptr;
private:
    DLabel             *m_label = nullptr;
    QVBoxLayout        *m_layout = nullptr;
};

#endif // STYLEATTRIWIDGET_H
