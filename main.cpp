#include <DApplication>
#include <DLog>
#include <dutility.h>

#include "mainwindow.h"

#include <QObject>
#include <QTranslator>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
#if defined(STATIC_LIB)
    DWIDGET_INIT_RESOURCE();
#endif

    DApplication::loadDXcbPlugin();

    DApplication a(argc, argv);
    a.loadTranslator(QList<QLocale>() << QLocale::system());
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-draw");
    a.setApplicationVersion("1.0");
    a.setTheme("light");
    a.setQuitOnLastWindowClosed(false);

    using namespace Dtk::Util;
    Dtk::Util::DLogManager::registerConsoleAppender();
    Dtk::Util::DLogManager::registerFileAppender();

    MainWindow w;
    w.activateWindow();
    w.show();

    return a.exec();
}
