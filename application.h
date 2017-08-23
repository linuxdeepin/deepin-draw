#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>

class Application;

#if  defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<Application *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT
public:
    Application(int &argc, char** argv);

signals:
    void popupConfirmDialog();

protected:
    void handleQuitAction();

private:
    void initI18n();
};
#endif // APPLICATION_H
