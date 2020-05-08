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
#include "application.h"
#include "frame/mainwindow.h"
#include "frame/cviewmanagement.h"
#include "service/dbusdraw_adaptor.h"

#include <QFileInfo>
#include <QDBusConnection>
#include <DApplicationSettings>

#include <DGuiApplicationHelper>
#include <DApplicationSettings>

#include <DLog>

Application::Application(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
{
    initI18n();

    dApp->setQuitOnLastWindowClosed(true);

    //绑定主题发生变化的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Application::onThemChanged);

}

int Application::execDraw(const QStringList &paths, QString &glAppPath)
{
    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    //判断实例是否已经运行
    if (this->isRunning()) {
        qDebug() << "deepin-draw is already running";

        QString message = paths.join(' ');
        this->sendMessage(message, 2000);

        this->activateWindow();

        return EXIT_SUCCESS;
    }

    static const QDate buildDate = QLocale( QLocale::English )
                                   .toDate( QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy");
    QString t_date = buildDate.toString("MMdd");

    // Version Time
    this->setApplicationVersion(DApplication::buildVersion(t_date));

    //主题设置
    glAppPath = QDir::homePath() + QDir::separator() + "." + qApp->applicationName();
    QDir t_appDir;
    t_appDir.mkpath(glAppPath);


    this->setOrganizationName("deepin");
    this->setApplicationName("deepin-draw");
    this->setQuitOnLastWindowClosed(true);

    // 应用已保存的主题设置
    //DGuiApplicationHelper::ColorType type = getThemeTypeSetting();
    DApplicationSettings saveTheme;
    CManageViewSigleton::GetInstance()->setThemeType(DGuiApplicationHelper::instance()->themeType());

    showMainWindow(paths);

    return exec();
}

QStringList Application::getRightFiles(const QStringList &files)
{
    QStringList resultList;

    QStringList strPicSuffixs;
    strPicSuffixs << "png" << "jpg" << "bmp" << "tif";

    for (int i = 0; i < files.size(); ++i) {
        const QString path = files.at(i);
        QFileInfo     info(path);

        if (info.isFile()) {
            const QString suffix = info.suffix().toLower();
            if (strPicSuffixs.contains(suffix)) {
                //只用判断是否可读
                if (info.isReadable()) {
                    resultList.append(path);
                }
            } else if (suffix == "ddf") {
                if (info.isReadable()/* && info.isWritable()*/) {
                    resultList.append(path);
                }
            }
        }

    }
    return resultList;
}

void Application::onMessageRecived(const QString &message)
{
    activateWindow();

    if (message.isEmpty())
        return;

    MainWindow *pWin = dynamic_cast<MainWindow *>(this->activationWindow());

    if (pWin != nullptr) {

        QStringList files = message.split(' ');

        if (!files.isEmpty()) {
            pWin->slotLoadDragOrPasteFile(files);
        }
    }
}

void Application::onThemChanged(DGuiApplicationHelper::ColorType themeType)
{
    MainWindow *pWin = dynamic_cast<MainWindow *>(this->activationWindow());

    if (pWin != nullptr) {
        pWin->slotOnThemeChanged(themeType);
    }
}

void Application::showMainWindow(const QStringList &paths)
{
#ifdef DEBUSVAILD
    MainWindow *w = new MainWindow;

    this->setActivationWindow(w, true);

    //以dbus的方式传递命令
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService("com.deepin.Draw");
    dbus.registerObject("/com/deepin/Draw", w);
    new dbusdraw_adaptor(w);
#else
    MainWindow *w = new MainWindow(paths);

    this->setActivationWindow(w, true);

    //如果没有通过dbus的方式进行进程通信那么这里要绑定指令
    connect(this, &Application::messageReceived, this, &Application::onMessageRecived, Qt::QueuedConnection);
#endif

    w->initScene();
    w->readSettings();
    w->show();
}

void Application::noticeFileRightProblem(const QStringList &problemfile)
{
    if (problemfile.isEmpty())
        return;

    MainWindow *pWin = dynamic_cast<MainWindow *>(this->activationWindow());
    QWidget *pParent = pWin;

    //证明是被重命名或者删除
    DDialog dia(pParent);
    dia.setModal(true);
    //dia.setMessage(tr("There is %1 file cannot open, insufficient permissions!").arg(problemfile.size()));
    QString message = (problemfile.size() == 1 ?
                       tr("\"%1\" is write-only, thus you cannot open it").arg(QFileInfo(problemfile.first()).fileName()) :
                       tr("Several files are write-only, thus you cannot open them"));
    dia.setMessage(message);
    dia.setIcon(QPixmap(":/theme/common/images/deepin-draw-64.svg"));
    dia.addButton(tr("OK"), true, DDialog::ButtonNormal);
    dia.exec();

    //如果没有任何文件加载成功(没有view就表示没有任何文件加载成功)
    if (pParent == nullptr || CManageViewSigleton::GetInstance()->isEmpty()) {
        this->quit();
    }
}

void Application::handleQuitAction()
{
    emit popupConfirmDialog();
}

void Application::initI18n()
{
    loadTranslator(QList<QLocale>() << QLocale::system());
}
