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
#include "mainwindow.h"
#include "widgets/dialog/drawdialog.h"
#include "../application.h"
#include "ccentralwidget.h"
#include "toptoolbar.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawparamsigleton.h"
#include "cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "utils/shortcut.h"
#include "utils/global.h"

#include <DTitlebar>
#include <DFileDialog>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QProcess>
#include <QStandardPaths>
#include <QSettings>
#include <QScrollBar>


//const QSize WINDOW_MINISIZR = QSize(1280, 800);

//const QSize WINDOW_MINISIZR = QSize(1024, 768);

const int TITLBAR_MENU = 150;
//const int TITLEBAR_HEIGHT = 40;
//const int IMG_ROTATEPOINT_SPACING = 35;

MainWindow::MainWindow(DWidget *parent)
    : DMainWindow(parent)
{
    m_centralWidget = new CCentralwidget(this);
    initUI();
    initConnection();
}

MainWindow::MainWindow(QStringList filePaths)
{
    m_centralWidget = new CCentralwidget(filePaths, this);

    initUI();
    initConnection();
}


void MainWindow::initUI()
{
    setWindowTitle(tr("Draw"));
    //根据屏幕分辨率进行最小化窗口的设置
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    int screenWidth;
    screenWidth = screenRect.width();
    if (screenWidth < 1152) {
        setMinimumSize(QSize(1024, 768));
    } else {
        setMinimumSize(QSize(1152, 768));
    }

    m_centralWidget->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_centralWidget);

    m_topToolbar = new TopToolbar(this);
    m_topToolbar->setFrameShape(DFrame::NoFrame);
    m_topToolbar->setFixedWidth(width() - TITLBAR_MENU);
    m_topToolbar->setFixedHeight(titlebar()->height());
    //qDebug() << "titlebar()->height()" << titlebar()->height() << endl;

//    titlebar()->setIcon(QIcon (QPixmap(":/theme/common/images/logo.svg").scaled(QSize(32, 32))));
    titlebar()->addWidget(m_topToolbar, Qt::AlignLeft);
    titlebar()->setMenu(m_topToolbar->mainMenu());
    titlebar()->setFocusPolicy(Qt::NoFocus);

//    titlebar()->setStyleSheet("background-color: rgb(0, 255, 0);");

    //ESC快捷键功能
    m_quitMode = new QAction(this);
    m_quitMode->setShortcut(QKeySequence(Qt::Key_Escape));
    this->addAction(m_quitMode);

    m_showCut = new QAction(this);
    m_showCut->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
    this->addAction(m_showCut);

    // 标签关闭提示框
    m_dialog.setIconPixmap(QPixmap(":/theme/common/images/deepin-draw-64.svg"));
    m_dialog.setMessage(tr("Is Close Draw?"));
    m_dialog.addButton(tr("OK"), true, DDialog::ButtonNormal);
    m_dialog.addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    connect(&m_dialog, &DDialog::buttonClicked, this, [ = ](int id) {
        if (0 == id) {
            // 此处代码需要严谨，需要判断文件保存，如果需要保存，则需要保存成功后才能退出
            qApp->quit();
        }
    });
}

void MainWindow::showDragOrOpenFile(QStringList files, bool isOPenFile)
{
    Application *pApp = dynamic_cast<Application *>(qApp);
    if (pApp != nullptr) {
        files = pApp->getRightFiles(files);
    }

    if (files.isEmpty()) {
        return;
    }

    QString ddfPath = "";
    QStringList picturePathList;
    for (int i = 0; i < files.size(); i++) {
        QFileInfo info(files[i]);
        if (info.suffix().toLower() == ("ddf")) {
            ddfPath = files[i].replace("file://", "");
            if (!ddfPath.isEmpty()) {

                bool isOpened = CManageViewSigleton::GetInstance()->isDdfFileOpened(ddfPath);
                if (isOpened)
                    continue;

                // 创建一个新的窗口用于显示拖拽的图像
                m_centralWidget->createNewScenseByscencePath(ddfPath);

                if (isOPenFile) {
                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::StartByDDF);
                } else {
                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
                }

                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath(ddfPath);
                slotIsNeedSave();
            }
        } else if (files[i].endsWith(".png") || files[i].endsWith(".jpg")
                   || files[i].endsWith(".bmp") || files[i].endsWith(".tif") ) {
            //图片格式："*.png *.jpg *.bmp *.tif"
            picturePathList.append(files[i].replace("file://", ""));
        }
    }

    if (!CManageViewSigleton::GetInstance()->isEmpty()) {
        if (cut == CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode()) {
            m_centralWidget->getGraphicsView()->slotQuitCutMode();
        }
    }

    if (picturePathList.count() > 0) {
        m_centralWidget->slotPastePicture(picturePathList);
    }
}

int MainWindow::showSaveQuestionDialog()
{
    //int  ret = 0;  //0 cancel 1 discal 2baocun
    DrawDialog quitQuestionDialog(this);

    int ret = quitQuestionDialog.exec();/*showSaveQuestionDialog();*/
    if (ret <= 0) {
        //结束关闭，同时结束其他标签页的关闭(因为取消了)
    } else if (ret == 1) {
        //放弃这个标签页的保存 抛弃
        m_centralWidget->closeCurrentScenseView();
    } else if (ret == 2) {
        //保存起来(传入保存起来后自动关闭)
        m_centralWidget->slotSaveToDDF(true);
    }
    return ret;
}

void MainWindow::closeTabViews()
{
    qDebug() << "close views begin = " << m_closeViews << m_closeUUids;

    // 此函数的作用是关闭 m_closeTabList 中的标签
    // 需要每次在保存或者不保存后进行调用判断

    int count = m_closeViews.size();

    if (m_closeUUids.size() != count)
        return;

    for (int i = 0; i < count; i++) {
        QString current_name = m_closeViews[i];
        QString current_uuid = m_closeUUids[i];

        m_centralWidget->setCurrentViewByUUID(current_uuid);
        CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
        CManageViewSigleton::GetInstance()->setCurView(pView);
        CGraphicsView *closeView = CManageViewSigleton::GetInstance()->getViewByUUID(current_uuid);
        if (closeView == nullptr) {
            qDebug() << "close error view:" << current_name;
            continue;
        } else {

            bool editFlag = closeView->getDrawParam()->getModify();
            if (editFlag) {

                // [0] 关闭标签前需要判断是否保存裁剪状态
                if (!m_centralWidget->slotJudgeCutStatusAndPopSaveDialog()) {
                    continue;
                }

                int ret = showSaveQuestionDialog();
                if (ret <= 0) {
                    //结束关闭，同时结束其他标签页的关闭(因为取消了)
                    qDebug() << "cancel close view at view = " << closeView->getDrawParam()->viewName() << "uuid = " << closeView->getDrawParam()->uuid();
                    break;
                }
            } else {
                m_centralWidget->closeCurrentScenseView();
            }
        }
    }

    qDebug() << "close views end ------";

    //如果未保存直接抛弃那么这里的判断可以直接推出程序(如果有view标签页需要保存那么就要等待保存完成后推出程序)
    CManageViewSigleton::GetInstance()->quitIfEmpty();
}

void MainWindow::initConnection()
{
    connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, m_topToolbar, &TopToolbar::updateMiddleWidget);
    //connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, m_topToolbar, &TopToolbar::slotHideColorPanel);
    connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, this, [ = ](int type, bool showSelfPropreWidget) {
        Q_UNUSED(type)
        Q_UNUSED(showSelfPropreWidget)
        m_topToolbar->slotHideColorPanel();
    });

    connect(this, &MainWindow::signalResetOriginPoint, m_centralWidget, &CCentralwidget::slotResetOriginPoint);
    connect(dApp, &Application::popupConfirmDialog, this, [ = ] {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::QuitApp);
        // 关闭所有标签
        QStringList divs = m_centralWidget->getAllTabBarName();
        if (divs.count())
        {
            m_closeViews = divs;
            m_closeUUids = m_centralWidget->getAllTabBarUUID();
            closeTabViews();
        }
    });
    connect(m_topToolbar, SIGNAL(signalTextFontFamilyChanged()), m_centralWidget, SLOT(slotTextFontFamilyChanged()));
    connect(m_topToolbar, SIGNAL(signalTextFontSizeChanged()), m_centralWidget, SLOT(slotTextFontSizeChanged()));

    connect(m_centralWidget, &CCentralwidget::signalAttributeChangedFromScene, m_topToolbar, &TopToolbar::slotChangeAttributeFromScene);

    connect(m_topToolbar, SIGNAL(signalZoom(qreal)), m_centralWidget, SLOT(slotZoom(qreal)));
    connect(m_centralWidget, SIGNAL(signalSetScale(qreal)), m_topToolbar, SLOT(slotSetScale(qreal)));

    connect(m_topToolbar, SIGNAL(signalShowExportDialog()), m_centralWidget, SLOT(slotShowExportDialog()));

    connect(m_topToolbar, SIGNAL(signalPrint()), m_centralWidget, SLOT(slotPrint()));

    connect(m_topToolbar, SIGNAL(signalNew()), this, SLOT(slotNewDrawScence()));

    connect(m_centralWidget, SIGNAL(signalUpdateCutSize()), m_topToolbar, SLOT(slotSetCutSize()));

    connect(m_topToolbar, SIGNAL(signalSaveToDDF()), this, SLOT(slotTopToolBarSaveToDDF()));

    connect(m_topToolbar, SIGNAL(signalSaveAs()), m_centralWidget, SLOT(slotSaveAs()));

    connect(m_topToolbar, SIGNAL(signalImport()), this, SLOT(slotShowOpenFileDialog()));

//    connect(m_quitQuestionDialog, SIGNAL(singalDoNotSaveToDDF()), this, SLOT(slotContinueDoSomeThing()));

    connect(m_centralWidget, SIGNAL(signalUpdateTextFont()), m_topToolbar, SLOT(slotSetTextFont()));

    connect(m_centralWidget, SIGNAL(signalContinueDoOtherThing()), this, SLOT(slotContinueDoSomeThing()));

    connect(m_quitMode, SIGNAL(triggered()), this, SLOT(slotOnEscButtonClick()));

    connect(m_topToolbar, SIGNAL(signalQuitCutModeFromTopBarMenu()), m_centralWidget, SIGNAL(signalTransmitQuitCutModeFromTopBarMenu()));

    connect(m_topToolbar, SIGNAL(signalCutLineEditIsfocus(bool)), m_centralWidget, SLOT(slotCutLineEditeFocusChange(bool)));

    connect(m_showCut, SIGNAL(triggered()), this, SLOT(onViewShortcut()));

    //每次添加标签需要连接
//    if (nullptr != CManageViewSigleton::GetInstance()->getCurView()->scene()) {
//        auto curScene = static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene());
//        connect(curScene, SIGNAL(signalUpdateColorPanelVisible(QPoint)), m_topToolbar, SLOT(updateColorPanelVisible(QPoint)));
//    }

    // 有新的场景创建后需要都进行连接的信号
    connect(m_centralWidget, &CCentralwidget::signalAddNewScence, this, [ = ](CDrawScene * sence) {
        connect(sence, SIGNAL(signalUpdateColorPanelVisible(QPoint)), m_topToolbar, SLOT(updateColorPanelVisible(QPoint)));
    });

    // 关闭当前窗口提示是否需要进行保存
    connect(m_centralWidget, &CCentralwidget::signalCloseModifyScence, this, &MainWindow::slotIsNeedSave);

    // 连接ddf文件已经被打开信号
    connect(m_centralWidget, &CCentralwidget::signalDDFFileOpened, this, &MainWindow::slotDDFFileOpened);

    // 连接最后一个标签被关闭
    connect(m_centralWidget, &CCentralwidget::signalLastTabBarRequestClose, this, &MainWindow::slotLastTabBarRequestClose);

    // 连接需要关闭多个标签信号
    connect(m_centralWidget, &CCentralwidget::signalTabItemsCloseRequested, this, [ = ](QStringList views, const QStringList & uuids) {
        m_closeViews = views;
        m_closeUUids = uuids;
        closeTabViews();
    });

    // 连接文件保存状态信号
    connect(m_centralWidget, &CCentralwidget::signalSaveFileStatus, this, [ = ](bool status) {
        qDebug() << "save status:" << status;
        //doCloseOtherDiv();
    });

    // 连接场景被改变后更新主窗口tittle显示信息
    connect(m_centralWidget, &CCentralwidget::signalScenceViewChanged, m_topToolbar, &TopToolbar::slotScenceViewChanged);

    // 链接剪裁切换场景后需要刷新菜单栏
    //connect(m_centralWidget, &CCentralwidget::signalChangeTittlebarWidget, m_topToolbar, &TopToolbar::updateMiddleWidget);
    connect(m_centralWidget, &CCentralwidget::signalChangeTittlebarWidget, this, [ = ](int type) {
        m_topToolbar->updateMiddleWidget(type);
    });
}

void MainWindow::activeWindow()
{
    window()->show();
    window()->raise();
    window()->activateWindow();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_topToolbar->setFixedWidth(width() - TITLBAR_MENU);

    emit signalResetOriginPoint();

    DMainWindow::resizeEvent(event);
    this->update();
}

void MainWindow::slotIsNeedSave()
{
    if (CManageViewSigleton::GetInstance()->getCurView()->getModify()) {
        // 此处需要进行适当延时显示才不会出错
        QMetaObject::invokeMethod(this, [ = ]() {
            showSaveQuestionDialog();
        }, Qt::QueuedConnection);
    } else {
        slotContinueDoSomeThing();
    }
}

void MainWindow::slotContinueDoSomeThing()
{
    ESaveDDFTriggerAction triggerType =  CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSaveDDFTriggerAction();
    switch (triggerType) {
    case QuitApp:
        qApp->quit();
        break;
    case LoadDDF:
        m_centralWidget->getGraphicsView()->importData(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getDdfSavePath());
        break;
    case StartByDDF:
        m_centralWidget->getGraphicsView()->importData(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getDdfSavePath(), true);
        break;
    case NewDrawingBoard:
        m_centralWidget->slotNew();
        break;
    case ImportPictrue:
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath("");
        m_centralWidget->getGraphicsView()->clearScene();
        m_centralWidget->getLeftToolBar()->slotShortCutSelect();
        m_centralWidget->openPicture(tmpPictruePath);
        break;
    default:
        break;
    }
}

void MainWindow::slotShowOpenFileDialog()
{
    DFileDialog dialog(this);
    dialog.setWindowTitle(tr("Open"));//设置文件保存对话框的标题
    dialog.setAcceptMode(QFileDialog::AcceptOpen);//设置文件对话框为保存模式
    dialog.setViewMode(DFileDialog::List);
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    //dialog.set
    QStringList nameFilters;
    nameFilters << "*.ddf *.png *.jpg *.bmp *.tif";
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    QStringList picturePathList;
    if (dialog.exec()) {
        QStringList tempfilePathList = dialog.selectedFiles();
        m_centralWidget->slotLoadDragOrPasteFile(tempfilePathList);
    }
}

void MainWindow::slotDDFFileOpened(QString filename)
{
    Q_UNUSED(filename)
//    Dtk::Widget::DDialog dialog(this);
//    dialog.setTextFormat(Qt::RichText);
//    dialog.addButton(tr("OK"));
//    dialog.setIcon(QIcon(":/icons/deepin/builtin/Bullet_window_warning.svg"));
//    dialog.setMessage(filename + tr(" Opened"));
//    dialog.exec();
}

void MainWindow::slotTopToolBarSaveToDDF()
{
    // ctrl+s 直接保存ddf文件
    qDebug() << "ctrl+s";
    m_centralWidget->slotSaveToDDF();
}

void MainWindow::slotLastTabBarRequestClose()
{
    qDebug() << "slotLastTabBarRequestClose: not show quit dialog";
    // 退出程序
    qApp->quit();
    //    m_dialog.exec();
}

void MainWindow::slotNewDrawScence()
{
    m_centralWidget->slotNew();
}

void MainWindow::onViewShortcut()
{
    QRect rect = window()->geometry();
    QPoint pos(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
    Shortcut sc;
    QStringList shortcutString;
    QString param1 = "-j=" + sc.toStr();
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());
    shortcutString << "-b" << param1 << param2;

    QProcess *shortcutViewProc = new QProcess(this);
    shortcutViewProc->startDetached("killall deepin-shortcut-viewer");
    shortcutViewProc->startDetached("deepin-shortcut-viewer", shortcutString);

    connect(shortcutViewProc, SIGNAL(finished(int)), shortcutViewProc, SLOT(deleteLater()));
}

void MainWindow::slotLoadDragOrPasteFile(QStringList files)
{
    showDragOrOpenFile(files, false);
}

void MainWindow::slotOnEscButtonClick()
{
    m_topToolbar->slotHideColorPanel();
    m_centralWidget->onEscButtonClick();
}

void MainWindow::showDrawDialog()
{
    if (CManageViewSigleton::GetInstance()->getCurView()->getModify()) {
        showSaveQuestionDialog();
    } else {
        qApp->quit();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_topToolbar->updateColorPanelVisible(event->pos());
    DMainWindow::mousePressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString fileName = Global::configPath() + "/config.conf";
    QSettings settings(fileName, QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("opened", "true");

    emit dApp->popupConfirmDialog();
    event->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setShiftKeyStatus(true);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAltKeyStatus(true);
    } else if (event->key() == Qt::Key_Control) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCtlKeyStatus(true);
    } else {
    }

    //    if (qApp->focusWidget() != nullptr) {
    //        qApp->focusWidget()->clearFocus();
    //        //qApp->focusWidget()->hide();
    //    }
    //    if (qApp->focusWidget() != nullptr) {
    //        qApp->focusWidget()->hide();
    //    }

    DMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setShiftKeyStatus(false);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAltKeyStatus(false);
    } else if (event->key() == Qt::Key_Control) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCtlKeyStatus(false);
        m_contrlKey = true;
    }  else {
        ;
    }
    DMainWindow::keyReleaseEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    CGraphicsView *pCurView   = CManageViewSigleton::GetInstance()->getCurView();
    int            delayValue = event->delta();
    if (pCurView != nullptr) {
        if (event->modifiers() == Qt::NoModifier) {
            //滚动view的垂直scrollbar
            int curValue = pCurView->verticalScrollBar()->value();
            pCurView->verticalScrollBar()->setValue(curValue - delayValue / 12);
        } else if (event->modifiers() == Qt::ShiftModifier) {
            //滚动view的水平scrollbar
            int curValue = pCurView->horizontalScrollBar()->value();
            pCurView->horizontalScrollBar()->setValue(curValue - delayValue / 12);
        } else if (event->modifiers()& Qt::ControlModifier) {
            //如果按住CTRL那么就是放大缩小
            if (event->delta() > 0) {
                pCurView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                pCurView->zoomOut();
            } else {
                pCurView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                pCurView->zoomIn();
            }
        }
    }
    DMainWindow::wheelEvent(event);
}

//static void printLab(QWidget *pWidget, QWidget *pMainWindow)
//{
//    QList<QObject *> children = pWidget->children();
//    for (QObject *pObj : children) {
//        QLabel *pLabel = qobject_cast<QLabel *>(pObj);
//        bool finish = false;
//        if (pLabel != nullptr) {
//            qDebug() << "printLab pLabel text ========= " << pLabel->text() << "object name = " << pLabel->objectName();
//            finish = true;

//            if (pLabel->objectName() == "WebsiteLabel") {
//                pLabel->installEventFilter(pMainWindow);
//            }

//        } else {
//            QAbstractButton *pBtn = qobject_cast<QAbstractButton *>(pObj);
//            if (pBtn != nullptr) {
//                qDebug() << "printLab pBtn text ========= " << pBtn->text();
//                finish = true;
//            }
//        }
//    }
//}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ChildAdded) {
        QChildEvent *addEvent = dynamic_cast<QChildEvent *>(event);
        if (addEvent != nullptr) {
            QWidget *pWidget = dynamic_cast<QWidget *>(addEvent->child());

            if (pWidget != nullptr && !pWidget->isModal()) {
                QLabel *pLabe = pWidget->findChild<QLabel *>("WebsiteLabel");
                if (pLabe != nullptr) {
                    m_pWebsiteLabe = pLabe;
                }
                pWidget->installEventFilter(this);
                m_allChilds.insert(pWidget);
            }
        }

    } else if (event->type() == QEvent::ChildRemoved) {
        QChildEvent *addEvent = dynamic_cast<QChildEvent *>(event);
        if (addEvent != nullptr) {
            QWidget *pWidget = qobject_cast<QWidget *>(addEvent->child());
            if (pWidget != nullptr) {
                m_allChilds.remove(pWidget);
            }
        }
    }
    return DMainWindow::event(event);
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o->isWidgetType()) {
        QWidget *pWidget = qobject_cast<QWidget *>(o);
        if (m_pWebsiteLabe == o) {
            //找到这个东西了
            static QCursor s_temp;
            if (e->type() == QEvent::Enter) {
                s_temp = *qApp->overrideCursor();
                qApp->setOverrideCursor(Qt::PointingHandCursor);
            } else if (e->type() == QEvent::Leave) {
                qApp->setOverrideCursor(s_temp);
            }
        } else if (m_allChilds.contains(pWidget)) {

            if (e->type() == QEvent::ChildAdded) {
                QChildEvent *addEvent = dynamic_cast<QChildEvent *>(e);

                QWidget *pWidget = qobject_cast<QWidget *>(addEvent->child());
                if (pWidget != nullptr) {
                    QLabel *pLabe = pWidget->findChild<QLabel *>("WebsiteLabel");
                    if (pLabe != nullptr) {
                        m_pWebsiteLabe = pLabe;
                        m_pWebsiteLabe->installEventFilter(this);
                    }
                }
            } else if (e->type() == QEvent::ChildRemoved) {
                QChildEvent *addEvent = dynamic_cast<QChildEvent *>(e);
                if (addEvent != nullptr && addEvent->child() == m_pWebsiteLabe) {
                    m_pWebsiteLabe->removeEventFilter(this);
                    m_pWebsiteLabe = nullptr;
                }
            }
        }
    }
    return DMainWindow::eventFilter(o, e);
}

void MainWindow::openImage(QString path, bool isStartByDDF)
{
    // 此函数是命令行调用进行处理的相关代码
    if (!path.isEmpty()) {
        // 新建一个标签页
        showDragOrOpenFile(QStringList(path), isStartByDDF);

//        if (cut == CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode()) {
//            m_centralWidget->getGraphicsView()->slotQuitCutMode();
//        }
//        if (QFileInfo(path).suffix().toLower()  == "ddf") {
//            if (isStartByDDF) {
//                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::StartByDDF);
//            } else {
//                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
//            }
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath(path);
//        } else {
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::ImportPictrue);
//            tmpPictruePath = path;
//        }
//        slotIsNeedSave();
    }
}

void MainWindow::initScene()
{
    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr) {
        QSize size = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutDefaultSize();
        QRectF rect(0, 0, 0, 0);
        rect.setSize(size);
        m_centralWidget->getDrawScene()->setSceneRect(rect);
        emit m_centralWidget->getDrawScene()->signalUpdateCutSize();
    }
}

void MainWindow::readSettings()
{
    QString fileName = Global::configPath() + "/config.conf";
    QSettings settings(fileName, QSettings::IniFormat);

    // [0] judge is first load draw process
    bool opened = settings.value("opened").toBool();
    if (!opened) {
        Dtk::Widget::moveToCenter(this);
        this->showMaximized();
    } else {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());
    }
}

void MainWindow::openFiles(QStringList filePaths)
{
    for (int i = 0; i < filePaths.count(); i++) {
        QFile file(filePaths.at(i));
        if (!file.exists()) { // 文件不存在，需要剔除这个文件
            filePaths.removeAt(i);
        }
    }

    m_centralWidget->slotLoadDragOrPasteFile(filePaths);
}

void MainWindow::openImage(QImage image, const QByteArray &srcData)
{
    m_centralWidget->slotPastePixmap(QPixmap::fromImage(image), srcData);
}

void MainWindow::slotOnThemeChanged(DGuiApplicationHelper::ColorType type)
{
    CManageViewSigleton::GetInstance()->setThemeType(type);

    ///改变场景的主题
    m_centralWidget->switchTheme(type);
    //改变左边工具栏按钮主题
//    m_centralWidget->getLeftToolBar()->changeButtonTheme();
    //改变顶部属性栏按钮主题
    m_topToolbar->changeTopButtonsTheme();

    CManageViewSigleton::GetInstance()->updateTheme();
}

MainWindow::~MainWindow()
{

}
