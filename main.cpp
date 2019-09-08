#include "frame/mainwindow.h"

#include <QCommandLineOption>
#include <QObject>
#include <QTranslator>

#include "application.h"
//#include "service/dbusdrawservice.h"
//#include "service/dbusdraw.h"

#include <DLog>

namespace  {
const QString DEEPIN_DRAW_DBUS_PATH = "/com/deepin/Draw";
const QString DEEPIN_DRAW_DBUS_NAME = "com.deepin.Draw";
}

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
#if defined(STATIC_LIB)
    DWIDGET_INIT_RESOURCE();
#endif

    Application::loadDXcbPlugin();
    Application::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    Application a(argc, argv);


    static const QDate buildDate = QLocale( QLocale::English )
                                   .toDate( QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy");
    QString t_date = buildDate.toString("MMdd");
    // Version Time
    a.setApplicationVersion(DApplication::buildVersion(t_date));



    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-draw");
    //a.setApplicationVersion("1.0");
    //a.setTheme("light");
    a.setQuitOnLastWindowClosed(true);
    //a.setStyle("chameleon");

    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w;
    w.show();


    return a.exec();


//    DBusDrawService dbusService(&w);
//    Q_UNUSED(dbusService);
//    //Register deepin-draw's dbus service.
//    QDBusConnection conn = QDBusConnection::sessionBus();
//    if (!conn.registerService(DEEPIN_DRAW_DBUS_NAME) ||
//            !conn.registerObject(DEEPIN_DRAW_DBUS_PATH, &w)) {
//        qDebug() << "deepin-draw is running!";
//    }

    QCommandLineOption openImageOption(QStringList() << "o" << "open",
                                       "Specify a path to load an image.", "PATH");
    QCommandLineOption activeWindowOption(QStringList() << "s" << "show",
                                          "Show deepin draw.");
    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("deepin-draw");
    cmdParser.addOption(openImageOption);
    cmdParser.addOption(activeWindowOption);
    cmdParser.process(a);

    if (cmdParser.isSet(openImageOption)) {
        w.activeWindow();
//        w.openImage(cmdParser.value(openImageOption));
    } else if (cmdParser.isSet(activeWindowOption)) {
        w.activeWindow();
    } else {
        QStringList pas = cmdParser.positionalArguments();
        if (pas.length() >= 1) {
            QString path;
            if (QUrl(pas.first()).isLocalFile())
                path =  QUrl(pas.first()).toLocalFile();
            else
                path = pas.first();
            w.activeWindow();
//            w.openImage(QFileInfo(path).absoluteFilePath());
        } else {
            w.show();

        }
    }

    return a.exec();
}
