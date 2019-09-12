#include "frame/mainwindow.h"

#include <QCommandLineOption>
#include <QObject>
#include <QTranslator>
#include <DGuiApplicationHelper>

#include "application.h"
//#include "service/dbusdrawservice.h"
//#include "service/dbusdraw.h"

#include <DLog>

DWIDGET_USE_NAMESPACE

static QString g_appPath;//全局路径


//获取配置文件主题类型，并重新设置
DGuiApplicationHelper::ColorType getThemeTypeSetting()
{
    //需要找到自己程序的配置文件路径，并读取配置，这里只是用home路径下themeType.cfg文件举例,具体配置文件根据自身项目情况
    QString t_appDir = g_appPath + QDir::separator() + "themetype.cfg";
    QFile t_configFile(t_appDir);

    t_configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t_readBuf = t_configFile.readAll();
    int t_readType = QString(t_readBuf).toInt();

    //获取读到的主题类型，并返回设置
    switch (t_readType) {
    case 0:
        // 跟随系统主题
        return DGuiApplicationHelper::UnknownType;
    case 1:
//        浅色主题
        return DGuiApplicationHelper::LightType;

    case 2:
//        深色主题
        return DGuiApplicationHelper::DarkType;
    default:
        // 跟随系统主题
        return DGuiApplicationHelper::UnknownType;
    }

}

//保存当前主题类型配置文件
void saveThemeTypeSetting(int type)
{
    //需要找到自己程序的配置文件路径，并写入配置，这里只是用home路径下themeType.cfg文件举例,具体配置文件根据自身项目情况
    QString t_appDir = g_appPath + QDir::separator() + "themetype.cfg";
    QFile t_configFile(t_appDir);

    t_configFile.open(QIODevice::WriteOnly | QIODevice::Text);
    //直接将主题类型保存到配置文件，具体配置key-value组合根据自身项目情况
    QString t_typeStr = QString::number(type);
    t_configFile.write(t_typeStr.toUtf8());
    t_configFile.close();
}





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

    //主题设置
    g_appPath = QDir::homePath() + QDir::separator() + "." + qApp->applicationName();
    QDir t_appDir;
    t_appDir.mkpath(g_appPath);



    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-draw");
    //a.setApplicationVersion("1.0");
    //a.setTheme("light");
    a.setQuitOnLastWindowClosed(true);
    //a.setStyle("chameleon");

    // 应用已保存的主题设置

    DGuiApplicationHelper::instance()->setPaletteType(getThemeTypeSetting());




    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w;
    w.show();


    //监听当前应用主题切换事件
    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
    [] (DGuiApplicationHelper::ColorType type) {
        qDebug() << type;
        // 保存程序的主题设置  type : 0,系统主题， 1,浅色主题， 2,深色主题
        saveThemeTypeSetting(type);
    });


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
