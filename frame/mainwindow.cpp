/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include "utils/shortcut.h"

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


//const QSize WINDOW_MINISIZR = QSize(1280, 800);

//const QSize WINDOW_MINISIZR = QSize(1024, 768);

const int TITLBAR_MENU = 150;
//const int TITLEBAR_HEIGHT = 40;
//const int IMG_ROTATEPOINT_SPACING = 35;

MainWindow::MainWindow(DWidget *parent)
    : DMainWindow(parent)
{
//    setMouseTracking(true);

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

    m_topToolbar = new TopToolbar(this);
    m_topToolbar->setFrameShape(DFrame::NoFrame);
    m_topToolbar->setFixedWidth(width() - TITLBAR_MENU);
    m_topToolbar->setFixedHeight(titlebar()->height());
    //qDebug() << "titlebar()->height()" << titlebar()->height() << endl;

//    titlebar()->setIcon(QIcon (QPixmap(":/theme/common/images/logo.svg").scaled(QSize(32, 32))));
    titlebar()->addWidget(m_topToolbar, Qt::AlignLeft);
    titlebar()->setMenu(m_topToolbar->mainMenu());

//    titlebar()->setStyleSheet("background-color: rgb(0, 255, 0);");

    m_centralWidget = new CCentralwidget(this);
    m_centralWidget->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_centralWidget);

    m_quitQuestionDialog  = new DrawDialog(this);

    //ESC快捷键功能
    m_quitMode = new QAction(this);
    m_quitMode->setShortcut(QKeySequence(Qt::Key_Escape));
    this->addAction(m_quitMode);

    m_showCut = new QAction(this);
    m_showCut->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
    this->addAction(m_showCut);
}



void MainWindow::initConnection()
{
    connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, m_topToolbar, &TopToolbar::updateMiddleWidget);

    connect(this, &MainWindow::signalResetOriginPoint, m_centralWidget, &CCentralwidget::slotResetOriginPoint);
    connect(dApp, &Application::popupConfirmDialog, this, [ = ] {
        CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::QuitApp);
        slotIsNeedSave();
    });
    connect(m_topToolbar, SIGNAL(signalAttributeChanged()), m_centralWidget, SLOT(slotAttributeChanged()));
    connect(m_topToolbar, SIGNAL(signalTextFontFamilyChanged()), m_centralWidget, SLOT(slotTextFontFamilyChanged()));
    connect(m_topToolbar, SIGNAL(signalTextFontSizeChanged()), m_centralWidget, SLOT(slotTextFontSizeChanged()));

    connect(m_centralWidget, &CCentralwidget::signalAttributeChangedFromScene, m_topToolbar, &TopToolbar::slotChangeAttributeFromScene);

    //链接图片选择后相应的操作
    connect(m_topToolbar, SIGNAL(signalPassPictureOperation(int)), m_centralWidget, SIGNAL(signalPassPictureOper(int)));


    connect(m_topToolbar, SIGNAL(signalZoom(qreal)), m_centralWidget, SLOT(slotZoom(qreal)));
    connect(m_centralWidget, SIGNAL(signalSetScale(qreal)), m_topToolbar, SLOT(slotSetScale(qreal)));

    connect(m_topToolbar, SIGNAL(signalShowExportDialog()), m_centralWidget, SLOT(slotShowExportDialog()));

    connect(m_topToolbar, SIGNAL(signalPrint()), m_centralWidget, SLOT(slotPrint()));

    connect(m_topToolbar, SIGNAL(signalNew()), this, SLOT(slotIsNeedSave()));

    connect(m_centralWidget, SIGNAL(signalUpdateCutSize()), m_topToolbar, SLOT(slotSetCutSize()));

    connect(m_topToolbar, SIGNAL(signalSaveToDDF()), m_centralWidget, SLOT(slotSaveToDDF()));

    connect(m_topToolbar, SIGNAL(signalSaveAs()), m_centralWidget, SLOT(slotSaveAs()));

    connect(m_topToolbar, SIGNAL(signalImport()), this, SLOT(slotShowOpenFileDialog()));

    connect(m_quitQuestionDialog, SIGNAL(signalSaveToDDF()), m_centralWidget, SLOT(slotSaveToDDF()));

    connect(m_quitQuestionDialog, SIGNAL(singalDoNotSaveToDDF()), this, SLOT(slotContinueDoSomeThing()));

    connect(m_centralWidget, SIGNAL(signalUpdateTextFont()), m_topToolbar, SLOT(slotSetTextFont()));

    connect(m_centralWidget, SIGNAL(signalContinueDoOtherThing()), this, SLOT(slotContinueDoSomeThing()));

    connect(m_quitMode, SIGNAL(triggered()), m_centralWidget, SLOT(slotOnEscButtonClick()));

    connect(m_topToolbar, SIGNAL(signalQuitCutModeFromTopBarMenu()), m_centralWidget, SIGNAL(signalTransmitQuitCutModeFromTopBarMenu()));

    connect(m_topToolbar, SIGNAL(signalCutLineEditIsfocus(bool)), m_centralWidget, SLOT(slotCutLineEditeFocusChange(bool)));

    connect(m_showCut, SIGNAL(triggered()), this, SLOT(onViewShortcut()));

    connect(CDrawScene::GetInstance(), SIGNAL(signalUpdateColorPanelVisible(QPoint)), m_topToolbar, SLOT(updateColorPanelVisible(QPoint)));

    connect(m_centralWidget, SIGNAL(signalTransmitLoadDragOrPasteFile(QString)), this, SLOT(slotLoadDragOrPasteFile(QString)));
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
    if (CDrawParamSigleton::GetInstance()->getIsModify()) {
        m_quitQuestionDialog->show();
    } else {
        slotContinueDoSomeThing();
    }
}

void MainWindow::slotContinueDoSomeThing()
{
    ESaveDDFTriggerAction triggerType =  CDrawParamSigleton::GetInstance()->getSaveDDFTriggerAction();
    switch (triggerType) {
    case QuitApp:
        qApp->quit();
        break;
    case LoadDDF:
        m_centralWidget->getGraphicsView()->importData(CDrawParamSigleton::GetInstance()->getDdfSavePath());
        break;
    case StartByDDF:
        m_centralWidget->getGraphicsView()->importData(CDrawParamSigleton::GetInstance()->getDdfSavePath(), true);
        break;
    case NewDrawingBoard:
        m_centralWidget->slotNew();
        break;
    case ImportPictrue:
        CDrawParamSigleton::GetInstance()->setDdfSavePath("");
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
    dialog.setFileMode(DFileDialog::ExistingFile);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    //dialog.set
    QStringList nameFilters;
    nameFilters << "*.DDF *.png *.jpg *.bmp *.tif";
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    if (dialog.exec()) {
        QString path = dialog.selectedFiles().first();
        if (!path.isEmpty()) {
            if (QFileInfo(path).suffix() == "DDF") {
                CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
                CDrawParamSigleton::GetInstance()->setDdfSavePath(path);
            } else {
                CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::ImportPictrue);
                tmpPictruePath = path;
            }
            slotIsNeedSave();
        }
    }
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

void MainWindow::slotLoadDragOrPasteFile(QString files)
{
    QStringList tempfilePathList = files.split("\n");
    QString ddfPath = "";
    QStringList picturePathList;
    for (int i = 0; i < tempfilePathList.size(); i++) {
        if (tempfilePathList[i].endsWith(".DDF")) {
            ddfPath = tempfilePathList[i].replace("file://", "");
            break;
        } else if (tempfilePathList[i].endsWith(".png") || tempfilePathList[i].endsWith(".jpg")
                   || tempfilePathList[i].endsWith(".bmp") || tempfilePathList[i].endsWith(".tif") ) {
            //图片格式："*.png *.jpg *.bmp *.tif"
            picturePathList.append(tempfilePathList[i].replace("file://", ""));
        }
    }

    if (cut == CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode()) {
        m_centralWidget->getGraphicsView()->slotQuitCutMode();
    }

    if (!ddfPath.isEmpty()) {
        CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
        CDrawParamSigleton::GetInstance()->setDdfSavePath(ddfPath);
        slotIsNeedSave();
    } else if (picturePathList.count() > 0) {
        m_centralWidget->slotPastePicture(picturePathList);
    }
}


void MainWindow::showDrawDialog()
{
    if (CDrawParamSigleton::GetInstance()->getIsModify()) {
        m_quitQuestionDialog->show();
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
    emit dApp->popupConfirmDialog();
    event->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CDrawParamSigleton::GetInstance()->setShiftKeyStatus(true);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CDrawParamSigleton::GetInstance()->setAltKeyStatus(true);
    } else if (event->key() == Qt::Key_Control) {
        CDrawParamSigleton::GetInstance()->setCtlKeyStatus(true);
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
        CDrawParamSigleton::GetInstance()->setShiftKeyStatus(false);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CDrawParamSigleton::GetInstance()->setAltKeyStatus(false);
    } else if (event->key() == Qt::Key_Control) {
        CDrawParamSigleton::GetInstance()->setCtlKeyStatus(false);
        m_contrlKey = true;
    }  else {
        ;
    }
    DMainWindow::keyReleaseEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    //如果按住CTRL
    if (CDrawParamSigleton::GetInstance()->getCtlKeyStatus()) {
        if (event->delta() > 0) {
            m_centralWidget->getGraphicsView()->zoomOut();
        } else {
            m_centralWidget->getGraphicsView()->zoomIn();
        }
    }
    DMainWindow::wheelEvent(event);
}

void MainWindow::openImage(QString path, bool isStartByDDF)
{
    if (!path.isEmpty()) {
        if (cut == CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode()) {
            m_centralWidget->getGraphicsView()->slotQuitCutMode();
        }
        if (QFileInfo(path).suffix() == "DDF") {
            if (isStartByDDF) {
                CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::StartByDDF);
            } else {
                CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
            }
            CDrawParamSigleton::GetInstance()->setDdfSavePath(path);
        } else {
            CDrawParamSigleton::GetInstance()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::ImportPictrue);
            tmpPictruePath = path;
        }
        slotIsNeedSave();
    }
}

void MainWindow::initScene()
{
    QSize size = CDrawParamSigleton::GetInstance()->getCutDefaultSize();
    QRectF rect(0, 0, 0, 0);
    rect.setSize(size);
    m_centralWidget->getDrawScene()->setSceneRect(rect);
    emit m_centralWidget->getDrawScene()->signalUpdateCutSize();
}

void MainWindow::slotOnThemeChanged(DGuiApplicationHelper::ColorType type)
{
    CDrawParamSigleton::GetInstance()->setThemeType(type);
    ///改变场景的主题
    m_centralWidget->switchTheme(type);
    //改变左边工具栏按钮主题
    m_centralWidget->getLeftToolBar()->changeButtonTheme();
    //改变顶部属性栏按钮主题
    m_topToolbar->changeTopButtonsTheme();
}

MainWindow::~MainWindow()
{
}
