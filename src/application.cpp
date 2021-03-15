/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "acobjectlist.h"
#include "clefttoolbar.h"
#include "cdrawtoolmanagersigleton.h"
#include "citemattriwidget.h"
#include "toptoolbar.h"

#include <QFileInfo>
#include <QDBusConnection>
#include <QAccessible>
#include <DApplicationSettings>

#include <DGuiApplicationHelper>
#include <DApplicationSettings>
#include <DVtableHook>

#include <QtConcurrent>

#include <malloc.h>

#include <DLog>
#include <DApplicationHelper>

//#include "config.h"

Application *Application::s_drawApp = nullptr;

Application::Application(int &argc, char **argv)
    : QObject(nullptr)
{
    if (s_drawApp == nullptr) {
        s_drawApp = this;
    }

#if (DTK_VERSION < DTK_VERSION_CHECK(5, 4, 0, 0))
    _dApp = new DApplication(argc, argv);
#else
    _dApp = DApplication::globalApplication(argc, argv);
#endif
#ifdef ENABLE_ACCESSIBILITY
    QAccessible::installFactory(accessibleFactory);
#endif

    _dApp->installEventFilter(this);

    initI18n();

    dApp->setQuitOnLastWindowClosed(true);

    //绑定主题发生变化的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Application::onThemChanged);

    qRegisterMetaType<EFileClassEnum>("EFileClassEnum");
    qRegisterMetaType<Application::EFileClassEnum>("Application::EFileClassEnum");
    qRegisterMetaType<EChangedPhase>("EChangedPhase");

    qApp->setOverrideCursor(Qt::ArrowCursor);


    //当前handlequit虚函数在虚表中16,重写函数的调用地址
    Dtk::Core::DVtableHook::overrideVfptrFun(dApplication(), &DApplication::handleQuitAction, this, &Application::onAppQuit);
}

Application *Application::drawApplication()
{
    return s_drawApp;
}

DApplication *Application::dApplication()
{
    return _dApp;
}

int Application::execDraw(const QStringList &paths)
{
    _dApp->setOrganizationName("deepin");
    _dApp->setApplicationName("deepin-draw");
    _dApp->loadTranslator(QList<QLocale>() << QLocale::system());
    _dApp->setApplicationDisplayName(tr("Draw"));
    _dApp->setQuitOnLastWindowClosed(true);
    using namespace Dtk::Core;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    // 应用已保存的主题设置
    //DGuiApplicationHelper::ColorType type = getThemeTypeSetting();
    DApplicationSettings saveTheme;
    CManageViewSigleton::GetInstance()->setThemeType(DGuiApplicationHelper::instance()->themeType());

    showMainWindow(paths);


    int ret = _dApp->exec();

    delete actWin;

    actWin = nullptr;

    return ret;
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

void Application::setViewCurrentTool(CGraphicsView *pView, EDrawToolMode tool)
{
    if (pView == nullptr)
        return;

    if (pView == CManageViewSigleton::GetInstance()->getCurView())
        this->leftToolBar()->setCurrentTool(tool);
    else
        pView->getDrawParam()->setCurrentDrawToolMode(tool);
}

bool Application::isViewToolEnable(CGraphicsView *pView, EDrawToolMode tool)
{
    if (pView == nullptr)
        return false;

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(tool);

    return pTool->isEnable(pView);
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

QColor Application::systemThemeColor()
{
    DPalette pa = currentDrawScence()->palette();
    QBrush activeBrush = pa.brush(QPalette::Active, DPalette::Highlight);
    return  activeBrush.color();
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

void Application::setWidgetAccesibleName(QWidget *w, const QString &name)
{
    if (w != nullptr) {
        w->setObjectName(name);
#ifdef ENABLE_ACCESSIBILITY
        w->setAccessibleName(name);
#endif
    }
}

void Application::onAppQuit()
{
    qDebug() << "Application::onAppQuit()";
    emit drawApp->popupConfirmDialog();
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

void Application::saveCursor()
{
    if (qApp->overrideCursor() != nullptr)
        _cursorStack.push(*qApp->overrideCursor());
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

void Application::restoreCursor()
{
    if (!_cursorStack.isEmpty()) {
        QCursor cur = _cursorStack.pop();
        setApplicationCursor(cur);
    }
}

void Application::setTouchFeelingEnhanceValue(int value)
{
    _touchEnchValue = value;
}

int Application::touchFeelingEnhanceValue()
{
    return _touchEnchValue;
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
    //保持弹窗在主窗口中心
    QPoint centerPos =  drawApp->topMainWindow()->mapToGlobal(drawApp->topMainWindow()->rect().center() - dia.rect().center());
    dia.move(centerPos);
    dia.exec();

    if (checkQuit) {
        //如果没有任何文件加载成功(没有view就表示没有任何文件加载成功)
        if (pParent == nullptr || CManageViewSigleton::GetInstance()->isEmpty()) {
            dApp->quit();
        }
    }
}

int Application::exeMessage(const QString &message,
                            Application::EMessageType msgTp,
                            bool autoFitDialogWidth,
                            const QStringList &moreBtns,
                            const QList<int> &btnType)
{
    DDialog dia(this->topMainWindowWidget());
    dia.setFixedSize(404, 163);
    dia.setModal(true);
    QString shortenFileName = autoFitDialogWidth ?
                              QFontMetrics(dia.font()).elidedText(message, Qt::ElideMiddle, dia.width() / 2) : message;
    dia.setMessage(shortenFileName);
    QString iconSvg;
    switch (msgTp) {
    case ENormalMsg:
        iconSvg = ":/theme/common/images/deepin-draw-64.svg";
        break;
    case EWarningMsg:
        iconSvg = ":/icons/deepin/builtin/Bullet_window_warning.svg";
        break;
    case EQuestionMsg:
        iconSvg = ":/icons/deepin/builtin/Bullet_window_warning.svg";
        break;
    }
    dia.setIcon(QPixmap(iconSvg));

    if (moreBtns.size() == btnType.size())
        for (int i = 0; i < moreBtns.size(); ++i)
            dia.addButton(moreBtns.at(i), false, DDialog::ButtonType(btnType.at(i)));

    return dia.exec();
}

bool Application::eventFilter(QObject *o, QEvent *e)
{
    //点击或者触控点击需要隐藏颜色板，另外，颜色板调用者隐藏时，颜色板也应该隐藏
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::TouchBegin) {
        if (o->isWidgetType() && actWin != nullptr && qApp->activePopupWidget() == nullptr) {
            CColorPickWidget *pColor = colorPickWidget();
            if (pColor != nullptr && pColor != o) {
                if (!pColor->isHidden()) {
                    ColorPanel *pColorPanel = pColor->colorPanel();
                    if (!(o == pColorPanel ||
                            pColorPanel->isAncestorOf(qobject_cast<QWidget *>(o)))) {
                        pColor->hide();
                        //隐藏颜色板后 中断后续处理，保证这次仅实现颜色板隐藏，不会触发其他事件
                        return true;
                    }
                }
            }
        }
    } else if (e->type() == QEvent::Hide) {
        //颜色板调用者隐藏时，颜色板也应该隐藏
        CColorPickWidget *pColor = colorPickWidget();
        if (pColor != nullptr && !pColor->isHidden() && pColor->caller() == o) {
            pColor->hide();
        }
    } else if (e->type() == QEvent::FocusOut) {
        //当前如果是针对文字的颜色修改，那么当文字编辑框处于编辑状态时，需要进行焦点转移
        CColorPickWidget *pColor = colorPickWidget();
        if (pColor != nullptr) {
            ColorPanel *pColorPanel = pColor->colorPanel();
            auto currentFocus = qApp->focusWidget();
            if (pColorPanel->isAncestorOf(qobject_cast<QWidget *>(o))) {
                if (!pColorPanel->isAncestorOf(currentFocus))
                    topToolbar()->attributWidget()->ensureTextFocus();
            }
        }
    }
    return QObject::eventFilter(o, e);
}
//bool Application::notify(QObject *o, QEvent *e)
//{
//    //点击或者触控点击需要隐藏颜色板，另外，颜色板调用者隐藏时，颜色板也应该隐藏
//    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::TouchBegin || e->type() == QEvent::Hide) {
//        if (o->isWidgetType() && actWin != nullptr) {
//            CColorPickWidget *pColor = colorPickWidget();
//            if (pColor != nullptr) {
//                ColorPanel *pColorPanel = pColor->colorPanel();
//                if (!pColor->isHidden()) {
//                    if (!(o == pColorPanel ||
//                            pColorPanel->isAncestorOf(qobject_cast<QWidget *>(o)))) {
//                        pColor->hide();

//                        //点击的是调起颜色板的控件那么直接隐藏就好 不再继续传递(因为继续传递会再次显示颜色板)
//                        if (pColor->caller() == o && e->type() != QEvent::Hide) {
//                            return true;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    return DApplication::notify(o, e);
//}
//void Application::handleQuitAction()
//{
//    emit popupConfirmDialog();
//}

void Application::initI18n()
{
    // _dApp->loadTranslator(QList<QLocale>() << QLocale::system());
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
