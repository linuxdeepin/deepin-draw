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
    void addChildAtrri(AttributeWgt *attri, bool bAddLayout = true);
    void removeChildAtrri(AttributeWgt *attri);
    void setVar(const QVariant &var) override;

private:
    void initUi();
public:
    StyleAttriWidget   *m_extend = nullptr;
private:
    DLabel             *m_label = nullptr;
    QVBoxLayout        *m_layout = nullptr;
    QList<AttributeWgt *>         m_childAtrri;
};

#endif // STYLEATTRIWIDGET_H
