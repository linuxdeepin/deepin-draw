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
#include <QtConcurrent>

#include <malloc.h>

#include <DLog>

#include "config.h"

Application::Application(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
{
    initI18n();

    dApp->setQuitOnLastWindowClosed(true);

    //绑定主题发生变化的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Application::onThemChanged);

    qRegisterMetaType<EFileClassEnum>("EFileClassEnum");
    qRegisterMetaType<Application::EFileClassEnum>("Application::EFileClassEnum");

    qApp->setOverrideCursor(Qt::ArrowCursor);
}

int Application::execDraw(const QStringList &paths, QString &glAppPath)
{
    //判断实例是否已经运行
    if (this->isRunning()) {
        qDebug() << "deepin-draw is already running";

        QString message = paths.join(_joinFlag);

        this->sendMessage(message, 2000);

        this->activateWindow();

        return EXIT_SUCCESS;
    }

    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    QtConcurrent::run([]() {
        QWidget *w = new QWidget;
        w->setWindowIcon(QIcon::fromTheme("dde-file-manager"));
        w->winId();
        w->deleteLater();
    });

    // Version Time
    this->setApplicationVersion(VERSION);

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
    //过滤文件
    //判断文件是否是可读或者可写的(图片判断是否可读 ddf判断是否可读可写)
    Application::QFileClassedMap outFiles;
    QStringList paths  = this->doFileClassification(files, outFiles);

    outFiles.remove(Application::EDrawAppSup);

    for (auto it = outFiles.begin(); it != outFiles.end(); ++it) {

        Application::EFileClassEnum classTp = it.key();

        QStringList &problemFiles = it.value();

        if (!problemFiles.isEmpty()) {

            //提示有文件的有问题的文件集(为什么检查是否关闭程序的标记为是否等于第一个key而不是最后一个key呢？因为最上层弹窗是最后一次noticeFileRightProblem)
            //所以用户点击关闭的最后一个弹窗是最先执行的noticeFileRightProblem，所以要在用户关闭完所有的弹窗后再判断是否需要推出程序。
            QMetaObject::invokeMethod(this, "noticeFileRightProblem", Qt::QueuedConnection,
                                      Q_ARG(const QStringList &, problemFiles),
                                      Q_ARG(Application::EFileClassEnum, classTp),
                                      Q_ARG(bool, classTp == outFiles.firstKey()));

        }
    }
    return paths;
}

QStringList Application::doFileClassification(const QStringList &inFilesPath, Application::QFileClassedMap &out)
{
    out.clear();

    QStringList supFiles;
    for (int i = 0; i < inFilesPath.size(); ++i) {
        const QString path = inFilesPath.at(i);
        QFileInfo     info(path);

        if (info.isFile()) {
            const QString suffix = info.suffix().toLower();
            if (supPictureSuffix().contains(suffix) || supDdfStuffix().contains(suffix)) {
                out[EDrawAppSup].append(path);
                if (!info.isReadable()) {
                    out[EDrawAppSupButNotReadable].append(path);
                } else {
                    out[EDrawAppSupAndReadable].append(path);
                    supFiles.append(path);
                }
            } else {
                out[EDrawAppNotSup].append(path);
            }
        } else {
            out[ENotFile].append(path);
        }
    }
    return supFiles;
}

QStringList &Application::supPictureSuffix()
{
    static QStringList supPictureSuffixs = QStringList() << "png" << "jpg" << "bmp" << "tif"/* << "jpeg"*/;
    return supPictureSuffixs;
}

QStringList &Application::supDdfStuffix()
{
    static QStringList supDdfSuffixs = QStringList() << "ddf";
    return supDdfSuffixs;
}

QRegExp Application::fileNameRegExp(bool ill, bool containDirDelimiter)
{
    //实际需要去掉的字符是
    //版本1      '/' , '\' , ':' , '*'     , '?'   , '"', '<', '>' ，'|'

    //但是在C++代码中，这些字符可能是代码的特殊字符，需要转义一下(用\)
    //版本2      '/',  '\\',   ':' , '*'   , '?'   ,'\"' '<', '>' '|'

    //但是在正则表达式中，\，*，?，|也是特殊字符，所以还要用\转义一下(写在代码里面就要用\\了)
    //版本3      '/',  '\\\\', ':' , '\\*' , '\\?' ,'\"' '<', '>' , '\\|'


    //最终 ([/\\\\:\\*\\?\"<>\\|])*

    QString exgStr = QString(ill ? "^" : "") + QString("([%1:\\*\\?\"<>\\|])*").arg(containDirDelimiter ? "/\\\\" : "");

    QRegExp regExg(exgStr);

    return regExg;
}

bool Application::isFileNameLegal(const QString &path, int *outErrorReson)
{
    if (path.isEmpty()) {
        if (outErrorReson != nullptr) {
            *outErrorReson = 0;
        }
        return false;
    }

    QRegExp regExp("[:\\*\\?\"<>\\|]");

    if (path.contains(regExp)) {

        if (outErrorReson != nullptr) {
            *outErrorReson  = 1;
        }
        return false;
    }

    QRegExp splitExp("[/\\\\]");


    int pos = splitExp.indexIn(path, 0);

    while (pos != -1) {
        QString dirStr = path.left(pos + 1);
        if (dirStr.count() > 1) {
            QDir dir(dirStr);
            if (!dir.exists()) {
                if (outErrorReson != nullptr) {
                    *outErrorReson  = 2;
                }
                return false;
            }
        }
        pos = splitExp.indexIn(path, pos + 1);
    }

    bool isdir = (path.endsWith('/') || path.endsWith('\\'));
    return !isdir;
}

void Application::setApplicationCursor(const QCursor &cur)
{
    if (qApp->overrideCursor() == nullptr) {
        qApp->setOverrideCursor(cur);
    } else {
        qApp->changeOverrideCursor(cur);
    }
}

void Application::onMessageRecived(const QString &message)
{
    activateWindow();

    if (message.isEmpty())
        return;

    MainWindow *pWin = dynamic_cast<MainWindow *>(this->activationWindow());

    if (pWin != nullptr) {

        QStringList files = message.split(_joinFlag);

        if (!files.isEmpty()) {
            pWin->openFiles(files);
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
    MainWindow *w = new MainWindow(paths);

    this->setActivationWindow(w, true);

    //以dbus的方式传递命令
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService("com.deepin.Draw");
    dbus.registerObject("/com/deepin/Draw", w);
    new dbusdraw_adaptor(w);

    connect(this, &Application::messageReceived, this, &Application::onMessageRecived, Qt::QueuedConnection);


    // 手动设置内存优化选项
//    mallopt(M_MXFAST, 0); // 禁止 fast bins
//    mallopt(M_MMAP_MAX, 0); // 禁止malloc调用mmap分配内存
//    mallopt(M_TRIM_THRESHOLD, 0); // 禁止内存缩进，sbrk申请的内存释放后不会归还给操作系统

    // [BUG 27979]   need call show first otherwise due window max size icon show error
    w->show();
    w->initScene();
    w->readSettings();
}

void Application::noticeFileRightProblem(const QStringList &problemfile, Application::EFileClassEnum classTp, bool checkQuit)
{
    if (problemfile.isEmpty())
        return;

    MainWindow *pWin = dynamic_cast<MainWindow *>(this->activationWindow());
    QWidget *pParent = pWin;

    DDialog dia(pParent);
    dia.setFixedSize(404, 163);
    dia.setModal(true);
    QString shortenFileName = QFontMetrics(dia.font()).elidedText(QFileInfo(problemfile.first()).fileName(), Qt::ElideMiddle, dia.width() / 2);

    QString message;

    switch (classTp) {
    case ENotFile:
    case EDrawAppNotSup:
        message = (problemfile.size() == 1 ?
                   tr("Unable to open \"%1\", unsupported file format").arg(shortenFileName) :
                   tr("Several files are unsupported, thus you cannot open them"));
        break;
    case EDrawAppSupButNotReadable:
        message = (problemfile.size() == 1 ?
                   tr("\"%1\" is write-only, thus you cannot open it").arg(shortenFileName) :
                   tr("Several files are write-only, thus you cannot open them"));
        break;
    default:
        return;
    }

    dia.setMessage(message);
    dia.setIcon(QPixmap(":/theme/common/images/deepin-draw-64.svg"));
    dia.addButton(tr("OK"), true, DDialog::ButtonNormal);
    dia.exec();

    if (checkQuit) {
        //如果没有任何文件加载成功(没有view就表示没有任何文件加载成功)
        if (pParent == nullptr || CManageViewSigleton::GetInstance()->isEmpty()) {
            this->quit();
        }
    }
}

void Application::handleQuitAction()
{
    emit popupConfirmDialog();
}

void Application::initI18n()
{
    loadTranslator(QList<QLocale>() << QLocale::system());

    _joinFlag = "?><:File0a0b0c0d";
}
