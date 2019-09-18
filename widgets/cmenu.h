#ifndef CMENU_H
#define CMENU_H
#include <DMenu>

DWIDGET_USE_NAMESPACE
class CMenu : public DMenu
{
public:
    explicit CMenu(QWidget *parent = nullptr);
    explicit CMenu(const QString &title, QWidget *parent = nullptr);

protected:
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
//    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
};

#endif // CMENU_H
