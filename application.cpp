#include "application.h"

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    initI18n();
}

void Application::handleQuitAction()
{
    emit popupConfirmDialog();
}

void Application::initI18n()
{
    loadTranslator(QList<QLocale>() << QLocale::system());
}
