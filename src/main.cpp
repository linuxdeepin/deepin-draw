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
#include <QDBusMetaType>

//#include "service/dbusdrawservice.h"
//#include "service/dbusdraw.h"

#include "service/dbusdraw_adaptor.h"

#include <DLog>

//DWIDGET_USE_NAMESPACE

static QString g_appPath;//全局路径

QStringList getFilesFromQCommandLineParser(const QCommandLineParser &parser)
{
    QStringList files;
    QStringList pas = parser.positionalArguments();
    for (int  i = 0; i < pas.count(); i++) {
        QString file = pas.at(i);
        files.append(file);
    }
    return files;
}

int main(int argc, char *argv[])
{
#if defined(STATIC_LIB)
    DWIDGET_INIT_RESOURCE();
#endif
    DApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    Application a(argc, argv);

    QCommandLineOption openImageOption(QStringList() << "o" << "open",
                                       "Specify a path to load an image.", "PATH");
    QCommandLineOption activeWindowOption(QStringList() << "s" << "show",
                                          "Show deepin draw.");
    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("deepin-draw");
    cmdParser.addOption(openImageOption);
    cmdParser.addOption(activeWindowOption);
    cmdParser.process(*a.dApplication());

    QStringList paths = getFilesFromQCommandLineParser(cmdParser);
    return a.execDraw(paths, g_appPath);
}
