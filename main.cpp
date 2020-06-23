/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "frame/mainwindow.h"
#include "application.h"
#include "frame/cviewmanagement.h"
#include "frame/ccentralwidget.h"

#include <DGuiApplicationHelper>
#include <DApplicationSettings>

#include <QCommandLineOption>
#include <QObject>
#include <QTranslator>
#include <QMessageBox>
#include <QDebug>

//#include "service/dbusdrawservice.h"
//#include "service/dbusdraw.h"

#include <DLog>

//DWIDGET_USE_NAMESPACE

static QString g_appPath;//全局路径
//static MainWindow *w;


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

    //DApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    Application::loadDXcbPlugin();
    Application::setAttribute(Qt::AA_UseHighDpiPixmaps);
    Application a(argc, argv);

    QCommandLineOption openImageOption(QStringList() << "o" << "open",
                                       "Specify a path to load an image.", "PATH");
    QCommandLineOption activeWindowOption(QStringList() << "s" << "show",
                                          "Show deepin draw.");
    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("deepin-draw");
    cmdParser.addOption(openImageOption);
    cmdParser.addOption(activeWindowOption);
    cmdParser.process(a);

    QStringList paths;
    QStringList pas = cmdParser.positionalArguments();
    for (int  i = 0; i < pas.count(); i++) {
        if (QUrl(pas.at(i)).isLocalFile()) {
            paths.append(QUrl(pas.first()).toLocalFile());
        } else {
            paths.append(pas.at(i));
        }
    }

    if (a.isRunning()) { //判断实例是否已经运行
        qDebug() << "deepin-draw is already running";
        for (int i = 0; i < paths.count(); i++) {
            a.sendMessage(paths.at(i), 2000); //1s后激活前个实例
        }
        return EXIT_SUCCESS;
    }

//    QObject::connect(qApp, &Application::messageReceived, qApp, [ = ](const QString & message) {

//    });

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
    //DGuiApplicationHelper::ColorType type = getThemeTypeSetting();
    //CManageViewSigleton::GetInstance()->setThemeType(type);
    //DGuiApplicationHelper::instance()->setPaletteType(type);
    DApplicationSettings saveTheme;
    CManageViewSigleton::GetInstance()->setThemeType(DGuiApplicationHelper::instance()->themeType());

    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w(paths);


//    QMessageBox::information(&w, "cmdParser.value(openImageOption)", cmdParser.value(openImageOption));

    a.setActivationWindow(&w);

    //QObject::connect(&a, &QtSingleApplication::messageReceived, &a, &QtSingleApplication::activateWindow);
    QObject::connect(&a, &Application::messageReceived, &a, [ = ](const QString & message) {

        //MainWindow *window = static_cast<MainWindow *>(qApp->activeWindow());
        qDebug() << "!!!!!!!!!!!!!!!message=" << message;
        //w.activeWindow();
        //a.activateWindow();
        if (message != "") {
            //w.openImage(QFileInfo(message).absoluteFilePath());
            QWidget *w = static_cast<Application *>(qApp)->activationWindow();
            static_cast<MainWindow *>(w)->openImage(QFileInfo(message).absoluteFilePath());
        }
    });

//监听当前应用主题切换事件

    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, &w, &MainWindow::slotOnThemeChanged);

//    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
//    [] (DGuiApplicationHelper::ColorType type) {
//        // 保存程序的主题设置  type : 0,系统主题， 1,浅色主题， 2,深色主题
//        saveThemeTypeSetting(type);
//        DGuiApplicationHelper::instance()->setPaletteType(type);
//    });


// return a.exec();


//    DBusDrawService dbusService(&w);
//    Q_UNUSED(dbusService);
//    //Register deepin-draw's dbus service.
//    QDBusConnection conn = QDBusConnection::sessionBus();
//    if (!conn.registerService(DEEPIN_DRAW_DBUS_NAME) ||
//            !conn.registerObject(DEEPIN_DRAW_DBUS_PATH, &w)) {
//        qDebug() << "deepin-draw is running!";
//    }

//qDebug() << argc << *argv << endl;
//    for (int i = 0 ; i < 100; i++) {
//        qDebug() << i << (argv[i]) << endl; ;
//    }

//    QCommandLineOption openImageOption(QStringList() << "o" << "open",
//                                       "Specify a path to load an image.", "PATH");
//    QCommandLineOption activeWindowOption(QStringList() << "s" << "show",
//                                          "Show deepin draw.");
//    QCommandLineParser cmdParser;
//    cmdParser.setApplicationDescription("deepin-draw");
//    cmdParser.addOption(openImageOption);
//    cmdParser.addOption(activeWindowOption);
//    cmdParser.process(a);

    if (cmdParser.isSet(openImageOption)) {
        w.activeWindow();
        w.openImage(cmdParser.value(openImageOption), true);
        //QMessageBox::information(&w, "cmdParser.value(openImageOption)", cmdParser.value(openImageOption));
    } else if (cmdParser.isSet(activeWindowOption)) {
        w.activeWindow();
    } else {
        QStringList pas = cmdParser.positionalArguments();
        //QMessageBox::information(&w, "cmdParser.positionalArguments()", cmdParser.positionalArguments()[0]);
        if (pas.length() >= 1) {
            QString path;
            if (QUrl(pas.first()).isLocalFile())
                path =  QUrl(pas.first()).toLocalFile();
            else
                path = pas.first();
            w.activeWindow();
            w.openImage(QFileInfo(path).absoluteFilePath(), true);
            //QMessageBox::information(&w, "path", path);
        } else {
            //QMessageBox::information(&w, "path", "last situation");
            w.show();

        }
    }

    w.initScene();
    //w.showMaximized();
    w.readSettings();

    return a.exec();
}
