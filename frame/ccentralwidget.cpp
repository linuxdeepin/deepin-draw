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
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsitem.h"
#include "widgets/progresslayout.h"
#include "drawshape/cpictureitem.h"
#include "cgraphicsview.h"
#include "drawshape/cpicturetool.h"
#include "drawshape/cgraphicstextitem.h"

#include <DLabel>
#include <DMenu>
#include <DGuiApplicationHelper>
#include <QDebug>
#include <QGraphicsItem>
#include <QtConcurrent>

DGUI_USE_NAMESPACE

CCentralwidget::CCentralwidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnect();
}


CCentralwidget::~CCentralwidget()
{

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
}


//进行图片导入
void CCentralwidget::importPicture()
{
    CPictureTool *pictureTool = new CPictureTool();
    pictureTool->drawPicture(m_pDrawScene, this);
}
//addImages(QPixmap pixmap, int itemNumber, CDrawScene *scene, CCentralwidget *centralWindow)

//点击图片进行导入
void CCentralwidget::openPicture(QString path)
{
    QPixmap pixmap = QPixmap(path);
    CPictureTool *pictureTool = new CPictureTool();
    pictureTool->addImages(pixmap, 1, m_pDrawScene, this);
}
#include "drawshape/cgraphicsellipseitem.h"
#include "drawshape/cgraphicstriangleitem.h"
void CCentralwidget::initUI()
{
    m_leftToolbar = new CLeftToolBar(this);
    m_pGraphicsView = new CGraphicsView(this);
    m_pDrawScene = new CDrawScene(this);
    QRectF rc = QRectF(0, 0, 1362, 790);
    m_pDrawScene->setSceneRect(rc);
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

void CCentralwidget::slotShowExportDialog()
{
    m_pGraphicsView->showExportDialog();
}

void CCentralwidget::slotSaveToDDF()
{
    m_pGraphicsView->doSaveDDF();
}

void CCentralwidget::slotSaveAs()
{
    m_pGraphicsView->showSaveDDFDialog(false);
}

void CCentralwidget::slotImport()
{
    m_pGraphicsView->doImport();
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
    m_pGraphicsView->clearScene();
}

void CCentralwidget::slotPrint()
{
    m_pGraphicsView->showPrintDialog();
}

void CCentralwidget::slotShowCutItem()
{
    m_pGraphicsView->setContextMenuAndActionEnable(false);
    m_pDrawScene->showCutItem();
}

void CCentralwidget::slotQuitCutMode()
{
    m_pGraphicsView->slotQuitCutMode();
}

void CCentralwidget::slotSetScale(const qreal scale)
{
    emit signalSetScale(scale);
}


void CCentralwidget::initConnect()
{
    //图片选中后相应操作
    connect(this, SIGNAL(signalPassPictureOper(int )), m_pDrawScene, SLOT(picOperation(int )));
    //导入图片信号槽
    connect(m_leftToolbar, SIGNAL(importPic()), this, SLOT(importPicture()));

    connect(m_leftToolbar, SIGNAL(signalBegainCut()), this, SLOT(slotShowCutItem()));

    connect(m_pDrawScene, &CDrawScene::signalAttributeChanged, this, &CCentralwidget::signalAttributeChangedFromScene);
    connect(m_pDrawScene, &CDrawScene::signalChangeToSelect, m_leftToolbar, &CLeftToolBar::slotChangedStatusToSelect);
    connect(m_pDrawScene, &CDrawScene::signalChangeToSelect, m_pGraphicsView, &CGraphicsView::slotStopContinuousDrawing);

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

    connect(m_pDrawScene, SIGNAL(signalQuitCutMode()), m_leftToolbar, SLOT(slotQuitCutMode()));

    connect(m_pDrawScene, &CDrawScene::signalUpdateCutSize, this, &CCentralwidget::signalUpdateCutSize);
    connect(m_pDrawScene, &CDrawScene::signalUpdateTextFont, this, &CCentralwidget::signalUpdateTextFont);


    connect(m_pGraphicsView, SIGNAL(signalTransmitContinueDoOtherThing()), this, SIGNAL(signalContinueDoOtherThing()));

    //主菜单栏中点击打开导入图片
    connect(m_pGraphicsView, SIGNAL(signalImportPicture(QString)), this, SLOT(openPicture(QString)));

    connect(m_leftToolbar, SIGNAL(setCurrentDrawTool(int)), m_pDrawScene, SLOT(drawToolChange(int)));

    //左边工具按钮点击退出裁剪模式
    connect(m_leftToolbar, SIGNAL(singalQuitCutModeFromLeftToolBar()), this, SLOT(slotQuitCutMode()));
}


