#ifndef CMENU_H
#define CMENU_H
#include <DMenu>

DWIDGET_USE_NAMESPACE
class CMenu : public DMenu
{
public:
    explicit CMenu(QWidget *parent = nullptr);
    explicit CMenu(const QString &title, QWidget *parent = nullptr);
    virtual void setVisible(bool visible) override;


protected:
    virtual void enterEvent(QEvent *event) override;
//    virtual void enterEvent(QEvent *event) override;
};

#endif // CMENU_H
