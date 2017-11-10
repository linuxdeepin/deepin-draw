#include "application.h"
#include <DLog>
#include <DWidgetUtil>

#include "frame/mainwindow.h"

#include <QCommandLineOption>
#include <QObject>
#include <QTranslator>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
#if defined(STATIC_LIB)
    DWIDGET_INIT_RESOURCE();
#endif

    Application::loadDXcbPlugin();

    Application a(argc, argv);
    a.loadTranslator(QList<QLocale>() << QLocale::system());
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-draw");
    a.setApplicationVersion("1.0");
    a.setTheme("light");
    a.setQuitOnLastWindowClosed(true);

    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w;
    w.activateWindow();

    QCommandLineOption openImageOption(QStringList() << "o" << "open",
                                        "Specifiy a path to save the screenshot.", "PATH");
    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("deepin-draw");
    cmdParser.addOption(openImageOption);
    cmdParser.process(a);

    QStringList names = cmdParser.optionNames();
    QStringList pas = cmdParser.positionalArguments();
    qDebug() << "cmdParse:" << names <<pas
                     <<  cmdParser.positionalArguments();
    if (names.isEmpty() && pas.isEmpty())
    {
        w.show();
    } else
    {
        QString name;
        QString value;
        QStringList values;
        if (!names.isEmpty())
        {
            name = names.first();
            value = cmdParser.value(name);
            values = cmdParser.values(name);
            qDebug() << "..." << name << value << values;
        } else if (!pas.isEmpty())
        {
            name = "o";
            value = pas.first();

            if (QUrl(value).isLocalFile())
            {
                value = QUrl(value).toLocalFile();
            }
            values = pas;
        }

        qDebug() << name << value << values;
        //TODO: Image support read.
        if (name == "o" || name == "open")
        {
            if (values.length() > 1)
             {
                QStringList aps;
                for (QString path : values)
                {
                    if (QUrl(value).isLocalFile())
                        path = QUrl(value).toLocalFile();
                     const QString ap = QFileInfo(path).absoluteFilePath();
                     if (QFileInfo(path).exists())
                     {
                         aps << ap;
                     }
                }

                if (!aps.isEmpty()) {
                    w.activateWindow();
                    w.show();
                    if (QFileInfo(pas.first()).suffix() == "ddf")
                    {
                        w.parseDdf(pas.first());
                    } else
                    {
                        w.openImage(aps.first());
                    }
                }
            } else if (QFileInfo(value).exists()) {
                qDebug() << "openImage :" << value;
                w.activateWindow();
                w.show();
                if (QFileInfo(value).suffix() == "ddf")
                {
                    w.parseDdf(value);
                } else
                {
                    w.openImage(QFileInfo(value).absoluteFilePath());
                }

              } else {
                w.activateWindow();
                w.show();
                qDebug() << "others";
            }
        }
    }

    return a.exec();
}
