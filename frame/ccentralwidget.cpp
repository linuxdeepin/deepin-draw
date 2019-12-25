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
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsitem.h"
#include "widgets/progresslayout.h"
#include "drawshape/cpictureitem.h"
#include "cgraphicsview.h"
#include "drawshape/cpicturetool.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cgraphicsellipseitem.h"
#include "drawshape/cgraphicstriangleitem.h"
#include "widgets/dialog/cexportimagedialog.h"
#include "widgets/dialog/cprintmanager.h"
#include "drawshape/cpicturetool.h"

#include <DMenu>
#include <DGuiApplicationHelper>
#include <QDebug>
#include <QGraphicsItem>
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QApplication>
#include <QPdfWriter>

DGUI_USE_NAMESPACE

CCentralwidget::CCentralwidget(DWidget *parent)
    : DWidget(parent)
{
    m_exportImageDialog = new CExportImageDialog(this);
    m_printManager = new CPrintManager();

    initUI();
    initConnect();
}


CCentralwidget::~CCentralwidget()
{
    delete m_pictureTool;

}
CLeftToolBar *CCentralwidget::getLeftToolBar()
{
    return m_leftToolbar;
}


CGraphicsView *CCentralwidget::getGraphicsView() const
{
    return m_pGraphicsView;
}

CDrawScene *CCentralwidget::getDrawScene() const
{
    return m_pDrawScene;
}

void CCentralwidget::switchTheme(int type)
{
    if (type == 1) {
        m_pDrawScene->setBackgroundBrush(QColor(248, 248, 251));
    } else if (type == 2) {
        m_pDrawScene->setBackgroundBrush(QColor(35, 35, 35));
    }

    m_pDrawScene->switchTheme(type);
}

void CCentralwidget::resetSceneBackgroundBrush()
{
    int themeType = CDrawParamSigleton::GetInstance()->getThemeType();
    if (themeType == 1) {
        m_pDrawScene->setBackgroundBrush(QColor(248, 248, 251));
    } else if (themeType == 2) {
        m_pDrawScene->setBackgroundBrush(QColor(35, 35, 35));
    }
}

void CCentralwidget::initSceneRect()
{
    QSize size = CDrawParamSigleton::GetInstance()->getCutDefaultSize();
    QRectF rc = QRectF(0, 0, size.width(), size.height());
    m_pDrawScene->setSceneRect(rc);
}

//进行图片导入
void CCentralwidget::importPicture()
{

    DFileDialog *fileDialog = new DFileDialog();
    //设置文件保存对话框的标题
    fileDialog->setWindowTitle(tr("导入图片"));
    QStringList filters;
    filters << "*.png *.jpg *.bmp *.tif";
    fileDialog->setNameFilters(filters);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    if (fileDialog->exec() ==   QDialog::Accepted) {
        QStringList filenames = fileDialog->selectedFiles();
        slotPastePicture(filenames);
    } else {
        m_leftToolbar->slotShortCutSelect();
    }

}


//点击图片进行导入
void CCentralwidget::openPicture(QString path)
{
    QPixmap pixmap = QPixmap(path);
    slotPastePixmap(pixmap);
}

//导入图片
void CCentralwidget::slotPastePicture(QStringList picturePathList)
{

    m_pictureTool->drawPicture(picturePathList, m_pDrawScene, this);
}

void CCentralwidget::slotPastePixmap(QPixmap pixmap)
{

    m_pictureTool->addImages(pixmap, 1, m_pDrawScene, this);

}

void CCentralwidget::initUI()
{
    m_leftToolbar = new CLeftToolBar();
    m_pGraphicsView = new CGraphicsView(this);
    //m_pGraphicsView->setStyleSheet("border:0px");
    //m_pGraphicsView->setWindowFlags(Qt::FramelessWindowHint);

    m_pictureTool = new CPictureTool();

    m_pGraphicsView->setFrameShape(QFrame::NoFrame);


    m_pDrawScene =  CDrawScene::GetInstance();
    //设置scene大小为屏幕分辨率
    //获取屏幕分辨率
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    CDrawParamSigleton::GetInstance()->setCutDefaultSize(QSize(screenRect.width(), screenRect.height()));

    initSceneRect();

    //根据主题设置
//case 1:
//        浅色主题
//case 2:
//        深色主题
    if (CDrawParamSigleton::GetInstance()->getThemeType() == 1) {
        m_pDrawScene->setBackgroundBrush(QColor(248, 248, 251));
    } else {
        m_pDrawScene->setBackgroundBrush(QColor(35, 35, 35));
    }

    //m_pDrawScene->setForegroundBrush(QColor(100, 100, 100));

    m_pGraphicsView->setScene(m_pDrawScene);
    m_pGraphicsView->setAlignment(Qt::AlignCenter);
    m_pGraphicsView->setRenderHint(QPainter::Antialiasing);//设置反走样

    //自动设置滚动条
    m_pGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_progressLayout = new ProgressLayout();


    QHBoxLayout *layout = new QHBoxLayout;

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_leftToolbar);
    layout->addWidget(m_pGraphicsView);
    setLayout(layout);

    //test

//    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(QRect(360, 360, 300, 300));
//    item->setPen(QPen(Qt::black));
//    item->setBrush(QBrush(Qt::red));

//    m_pDrawScene->addItem(item);

//    CGraphicsTriangleItem *itemrect = new CGraphicsTriangleItem(QRect(300, 300, 300, 300));
//    itemrect->setPen(QPen(Qt::black));
//    itemrect->setBrush(QBrush(Qt::black));

//    m_pDrawScene->addItem(itemrect);

    //初始设置显示为75%；
    m_pGraphicsView->scale(0.75);
}

void CCentralwidget::slotResetOriginPoint()
{
    /*QRect rect = m_pGraphicsView->viewport()->rect();
    m_pGraphicsView->setSceneRect(rect);*/
}

void CCentralwidget::slotAttributeChanged()
{
    if (m_pDrawScene != nullptr) {
        m_pDrawScene->attributeChanged();
    }
}

void CCentralwidget::slotZoom(qreal scale)
{
    m_pGraphicsView->scale(scale);
}

void CCentralwidget::slotSaveToDDF()
{
    m_pGraphicsView->doSaveDDF();
}

void CCentralwidget::slotSaveAs()
{
    m_pGraphicsView->showSaveDDFDialog(false);
}


void CCentralwidget::slotTextFontFamilyChanged()
{
    if (m_pDrawScene != nullptr) {
        m_pDrawScene->textFontFamilyChanged();
    }
}

void CCentralwidget::slotTextFontSizeChanged()
{
    if (m_pDrawScene != nullptr) {
        m_pDrawScene->textFontSizeChanged();
    }
}

void CCentralwidget::slotNew()
{
    CDrawParamSigleton::GetInstance()->setDdfSavePath("");
    CDrawParamSigleton::GetInstance()->setIsModify(false);
    m_pGraphicsView->clearScene();
    m_leftToolbar->slotShortCutSelect();
    initSceneRect();
}

void CCentralwidget::slotPrint()
{
    m_pDrawScene->clearSelection();
    QPixmap pixMap = getSceneImage(1);
    m_printManager->showPrintDialog(pixMap, this);
}

void CCentralwidget::slotShowCutItem()
{
    m_pGraphicsView->setContextMenuAndActionEnable(false);
    m_pDrawScene->showCutItem();
}

void CCentralwidget::onEscButtonClick()
{
    //如果当前是裁剪模式则退出裁剪模式　退出裁剪模式会默认设置工具栏为选中
    if (cut == CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode()) {
        m_pGraphicsView->slotQuitCutMode();
    } else {
        m_leftToolbar->slotShortCutSelect();
    }
    ///清空场景中选中图元
    m_pDrawScene->clearSelection();
}


void CCentralwidget::slotCutLineEditeFocusChange(bool isFocus)
{
    m_pGraphicsView->disableCutShortcut(isFocus);
}

void CCentralwidget::slotDoSaveImage(QString completePath)
{
    int type = m_exportImageDialog->getImageType();
    if (type == CExportImageDialog::PDF) {
        QPixmap pixmap = getSceneImage(1);
        QPdfWriter writer(completePath);
        int ww = pixmap.width();
        int wh = pixmap.height();
        writer.setResolution(96);
        writer.setPageSizeMM(QSizeF(25.4 * ww / 96, 25.4 * wh / 96));
        QPainter painter(&writer);
        painter.drawPixmap(0, 0, pixmap);
    } else if (type == CExportImageDialog::PNG) {
        QString format = m_exportImageDialog->getImageFormate();
        int quality = m_exportImageDialog->getQuality();
        QPixmap pixmap = getSceneImage(2);
        pixmap.save(completePath, format.toUpper().toLocal8Bit().data(), quality);
    } else {
        QString format = m_exportImageDialog->getImageFormate();
        int quality = m_exportImageDialog->getQuality();
        QPixmap pixmap = getSceneImage(1);
        pixmap.save(completePath, format.toUpper().toLocal8Bit().data(), quality);
    }
}

void CCentralwidget::slotShowExportDialog()
{
    m_pDrawScene->clearSelection();
    m_exportImageDialog->showMe();
}


void CCentralwidget::slotSetScale(const qreal scale)
{
    emit signalSetScale(scale);
}

QPixmap CCentralwidget::getSceneImage(int type)
{
    QPixmap pixmap(m_pDrawScene->sceneRect().width(), m_pDrawScene->sceneRect().height());
    CDrawParamSigleton::GetInstance()->setRenderImage(type);
    if (type == 2) {
        pixmap.fill(Qt::transparent);
        m_pDrawScene->setBackgroundBrush(Qt::transparent);
    }
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    m_pDrawScene->render(&painter);
    if (type == 2) {
        resetSceneBackgroundBrush();
    }
    CDrawParamSigleton::GetInstance()->setRenderImage(0);

    return  pixmap;
}

void CCentralwidget::initConnect()
{
    //图片选中后相应操作
    connect(this, SIGNAL(signalPassPictureOper(int )), m_pDrawScene, SLOT(picOperation(int )));
    //导入图片信号槽
    connect(m_leftToolbar, SIGNAL(importPic()), this, SLOT(importPicture()));

    connect(m_leftToolbar, SIGNAL(signalBegainCut()), this, SLOT(slotShowCutItem()));

    connect(m_pDrawScene, &CDrawScene::signalAttributeChanged, this, &CCentralwidget::signalAttributeChangedFromScene);
    connect(m_pDrawScene, &CDrawScene::signalChangeToSelect, m_leftToolbar, &CLeftToolBar::slotShortCutSelect);
//    connect(m_pDrawScene, &CDrawScene::signalChangeToSelect, m_pGraphicsView, &CGraphicsView::slotStopContinuousDrawing);

    connect(m_pGraphicsView, SIGNAL(signalSetScale(const qreal)), this, SLOT(slotSetScale(const qreal)));

    connect(m_pDrawScene, SIGNAL(itemMoved(QGraphicsItem *, QPointF)),
            m_pGraphicsView, SLOT(itemMoved(QGraphicsItem *, QPointF)));
    connect(m_pDrawScene, SIGNAL(itemAdded(QGraphicsItem *)),
            m_pGraphicsView, SLOT(itemAdded(QGraphicsItem *)));
    connect(m_pDrawScene, SIGNAL(itemRotate(QGraphicsItem *, qreal)),
            m_pGraphicsView, SLOT(itemRotate(QGraphicsItem *, qreal)));
    connect(m_pDrawScene, SIGNAL(itemResize(CGraphicsItem *, CSizeHandleRect::EDirection, QPointF, QPointF, bool, bool )),
            m_pGraphicsView, SLOT(itemResize(CGraphicsItem *, CSizeHandleRect::EDirection, QPointF, QPointF, bool, bool )));
    connect(m_pDrawScene, SIGNAL(itemPropertyChange(CGraphicsItem *, QPen, QBrush, bool, bool)),
            m_pGraphicsView, SLOT(itemPropertyChange(CGraphicsItem *, QPen, QBrush, bool, bool)));

    connect(m_pDrawScene, SIGNAL(itemPolygonPointChange(CGraphicsPolygonItem *, int )),
            m_pGraphicsView, SLOT(itemPolygonPointChange(CGraphicsPolygonItem *, int )));
    connect(m_pDrawScene, SIGNAL(itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *, int, int )),
            m_pGraphicsView, SLOT(itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *, int, int )));

    connect(m_pDrawScene, SIGNAL(itemPenTypeChange(CGraphicsPenItem *, int )),
            m_pGraphicsView, SLOT(itemPenTypeChange(CGraphicsPenItem *, int)));

    connect(m_pDrawScene, SIGNAL(itemBlurChange(CGraphicsMasicoItem *, int, int )),
            m_pGraphicsView, SLOT(itemBlurChange(CGraphicsMasicoItem *, int, int )));

    connect(m_pDrawScene, SIGNAL(itemLineTypeChange(CGraphicsLineItem *, int )),
            m_pGraphicsView, SLOT(itemLineTypeChange(CGraphicsLineItem *, int)));

    connect(m_pDrawScene, SIGNAL(signalQuitCutAndChangeToSelect()), m_leftToolbar, SLOT(slotAfterQuitCut()));
    connect(m_pDrawScene, SIGNAL(signalQuitCutAndChangeToSelect()), m_pGraphicsView, SLOT(slotRestContextMenuAfterQuitCut()));

    connect(m_pDrawScene, &CDrawScene::signalUpdateCutSize, this, &CCentralwidget::signalUpdateCutSize);
    connect(m_pDrawScene, &CDrawScene::signalUpdateTextFont, this, &CCentralwidget::signalUpdateTextFont);


    connect(m_pGraphicsView, SIGNAL(signalTransmitContinueDoOtherThing()), this, SIGNAL(signalContinueDoOtherThing()));
    connect(m_pGraphicsView, SIGNAL(singalTransmitEndLoadDDF()), m_leftToolbar, SLOT(slotShortCutSelect()));

    //主菜单栏中点击打开导入图片
    connect(m_pGraphicsView, SIGNAL(signalImportPicture(QString)), this, SLOT(openPicture(QString)));

    connect(m_leftToolbar, SIGNAL(setCurrentDrawTool(int)), m_pDrawScene, SLOT(drawToolChange(int)));

    //如果是裁剪模式点击左边工具栏按钮则执行裁剪
    connect(m_leftToolbar, SIGNAL(singalDoCutFromLeftToolBar()), m_pGraphicsView, SLOT(slotDoCutScene()));

    //如果是裁剪模式点击工具栏的菜单则执行裁剪
    connect(this, SIGNAL(signalTransmitQuitCutModeFromTopBarMenu()), m_pGraphicsView, SLOT(slotDoCutScene()));

    connect(m_exportImageDialog, SIGNAL(signalDoSave(QString)), this, SLOT(slotDoSaveImage(QString)));

    connect(m_pGraphicsView, SIGNAL(signalLoadDragOrPasteFile(QString)), this, SIGNAL(signalTransmitLoadDragOrPasteFile(QString)));
    connect(m_pGraphicsView, SIGNAL(signalPastePixmap(QPixmap)), this, SLOT(slotPastePixmap(QPixmap)));
    connect(m_pDrawScene, SIGNAL(signalSceneCut(QRectF)), m_pGraphicsView, SLOT(itemSceneCut(QRectF)));

}


