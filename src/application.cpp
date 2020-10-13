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
#include "ccolorpickwidget.h"
#include "globaldefine.h"
#include "cviewmanagement.h"
#include "cgraphicsview.h"
#include "ccentralwidget.h"
#include "cdrawscene.h"
#include "colorpanel.h"

#include <QFileInfo>
#include <QDBusConnection>
#include <DApplicationSettings>

#include <DGuiApplicationHelper>
#include <DApplicationSettings>
#include <QtConcurrent>

#include <malloc.h>

#include <DLog>

//#include "config.h"

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    initI18n();

    dApp->setQuitOnLastWindowClosed(true);

    //绑定主题发生变化的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Application::onThemChanged);

    qRegisterMetaType<EFileClassEnum>("EFileClassEnum");
    qRegisterMetaType<Application::EFileClassEnum>("Application::EFileClassEnum");
    qRegisterMetaType<EChangedPhase>("EChangedPhase");

    qApp->setOverrideCursor(Qt::ArrowCursor);
}

int Application::execDraw(const QStringList &paths/*, QString &glAppPath*/)
{
    // Version Time
    //this->setApplicationVersion(VERSION);

    //主题设置
//    glAppPath = QDir::homePath() + QDir::separator() + "." + qApp->applicationName();
//    QDir t_appDir;
//    t_appDir.mkpath(glAppPath);


    this->setOrganizationName("deepin");
    this->setApplicationName("deepin-draw");
    this->setQuitOnLastWindowClosed(true);

    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    // 应用已保存的主题设置
    //DGuiApplicationHelper::ColorType type = getThemeTypeSetting();
    DApplicationSettings saveTheme;
    CManageViewSigleton::GetInstance()->setThemeType(DGuiApplicationHelper::instance()->themeType());

    showMainWindow(paths);

    return exec();
}

MainWindow *Application::topMainWindow()
{
    return actWin;
}

QWidget *Application::topMainWindowWidget()
{
    return qobject_cast<QWidget *>(actWin);
}

CColorPickWidget *Application::colorPickWidget(bool creatNew, QWidget *pCaller)
{
    if (creatNew) {
        if (_colorPickWidget != nullptr) {
            _colorPickWidget->deleteLater();
            _colorPickWidget = nullptr;
        }
    }
    if (creatNew && _colorPickWidget == nullptr && topMainWindowWidget() != nullptr) {
        setProperty("_d_isDxcb", false);
        _colorPickWidget = new CColorPickWidget(topMainWindowWidget());
        setProperty("_d_isDxcb", true);
    }
    if (pCaller != nullptr)
        _colorPickWidget->setCaller(pCaller);
    return _colorPickWidget;
}

TopToolbar *Application::topToolbar()
{
    if (actWin != nullptr)
        return actWin->getTopToolbar();
    return nullptr;
}

CLeftToolBar *Application::leftToolBar()
{
    if (topMainWindow() != nullptr)
        return topMainWindow()->getCCentralwidget()->getLeftToolBar();
    return nullptr;
}

CDrawScene *Application::currentDrawScence()
{
    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr) {
        return dynamic_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene());
    }
    return nullptr;
}

QStringList Application::getRightFiles(const QStringList &files, bool notice)
{
    //过滤文件
    //判断文件是否是可读或者可写的(图片判断是否可读 ddf判断是否可读可写)
    Application::QFileClassedMap outFiles;
    QStringList paths  = this->doFileClassification(files, outFiles);

    outFiles.remove(Application::EDrawAppSup);

    for (auto it = outFiles.begin(); it != outFiles.end(); ++it) {

        Application::EFileClassEnum classTp = it.key();

        QStringList &problemFiles = it.value();

        if (!problemFiles.isEmpty() && notice) {

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
        QString path = inFilesPath.at(i);

        if (!isFileExist(path)) {
            out[ENotExist].append(path);
            continue;
        }

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

void Application::setWidgetAllPosterityNoFocus(QWidget *pW)
{
    if (pW == nullptr)
        return;

    QList<QObject *> list0 = pW->children();
    for (int i  = 0; i < list0.size(); ++i) {
        QObject *o = list0[i];
        if (o->isWidgetType()) {
            QWidget *pWidget = qobject_cast<QWidget *>(o);
            if (pWidget != nullptr) {
                pWidget->setFocusPolicy(Qt::NoFocus);
                setWidgetAllPosterityNoFocus(pWidget);
            }
        }
    }
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

void Application::setApplicationCursor(const QCursor &cur, bool force)
{
    if (!force) {

        if (CManageViewSigleton::GetInstance()->getCurView() == nullptr)
            return;

        bool isPressSpace = CManageViewSigleton::GetInstance()->getCurView()->isKeySpacePressed();
        if (isPressSpace)
            return;
    }

    if (qApp->overrideCursor() == nullptr) {
        qApp->setOverrideCursor(cur);
    } else {
        qApp->changeOverrideCursor(cur);
    }
}

void Application::onThemChanged(DGuiApplicationHelper::ColorType themeType)
{
    if (actWin != nullptr) {
        actWin->slotOnThemeChanged(themeType);
    }
}

void Application::activateWindow()
{
    if (actWin) {
        actWin->setWindowState(actWin->windowState() & ~Qt::WindowMinimized);
        actWin->raise();
        actWin->activateWindow();
    }
}

void Application::showMainWindow(const QStringList &paths)
{
    MainWindow *w = new MainWindow(paths);

    actWin = w;

    //以dbus的方式传递命令
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService("com.deepin.Draw");
    dbus.registerObject("/com/deepin/Draw", w);
    new dbusdraw_adaptor(w);

    // 手动设置内存优化选项
//    mallopt(M_MXFAST, 0); // 禁止 fast bins
//    mallopt(M_MMAP_MAX, 0); // 禁止malloc调用mmap分配内存
//    mallopt(M_TRIM_THRESHOLD, 0); // 禁止内存缩进，sbrk申请的内存释放后不会归还给操作系统

    // [BUG 27979]   need call show first otherwise due window max size icon show error
    w->show();
    w->readSettings();
}

void Application::noticeFileRightProblem(const QStringList &problemfile, Application::EFileClassEnum classTp, bool checkQuit)
{
    if (problemfile.isEmpty())
        return;

    QWidget *pParent = actWin;

    DDialog dia(pParent);
    dia.setFixedSize(404, 163);
    dia.setModal(true);
    QString shortenFileName = QFontMetrics(dia.font()).elidedText(QFileInfo(problemfile.first()).fileName(), Qt::ElideMiddle, dia.width() / 2);

    QString message;

    switch (classTp) {
    case ENotExist: {
        message = tr("The file does not exist");
        break;
    }
    case ENotFile:
    case EDrawAppNotSup:
        message = (problemfile.size() == 1 ?
                   tr("Unable to open \"%1\", unsupported file format").arg(shortenFileName) :
                   tr("Unsupported file format"));
        break;
    case EDrawAppSupButNotReadable:
        message = (problemfile.size() == 1 ?
                   tr("Unable to open the write-only file \"%1\"").arg(shortenFileName) :
                   tr("Unable to open the write-only files"));
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
bool Application::notify(QObject *o, QEvent *e)
{
    //点击或者触控点击需要隐藏颜色板，另外，颜色板调用者隐藏时，颜色板也应该隐藏
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::TouchBegin || e->type() == QEvent::Hide) {
        if (o->isWidgetType() && actWin != nullptr) {
            CColorPickWidget *pColor = colorPickWidget();
            if (pColor != nullptr) {
                ColorPanel *pColorPanel = pColor->colorPanel();
                if (!pColor->isHidden()) {
                    if (!(o == pColorPanel ||
                            pColorPanel->isAncestorOf(qobject_cast<QWidget *>(o)))) {
                        pColor->hide();

                        //点击的是调起颜色板的控件那么直接隐藏就好 不再继续传递(因为继续传递会再次显示颜色板)
                        if (pColor->caller() == o && e->type() != QEvent::Hide) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return DApplication::notify(o, e);
}
void Application::handleQuitAction()
{
    emit popupConfirmDialog();
}

void Application::initI18n()
{
    loadTranslator(QList<QLocale>() << QLocale::system());
}

bool Application::isFileExist(QString &filePath)
{
    QFileInfo info(filePath);
    if (!info.exists()) {
        QUrl url(filePath);
        bool isExist = false;
        if (url.isLocalFile()) {
            filePath = url.toLocalFile();
            QFileInfo newf(filePath);
            if (newf.exists()) {
                isExist = true;
            }
        }
        return isExist;
    }
    return true;
}
