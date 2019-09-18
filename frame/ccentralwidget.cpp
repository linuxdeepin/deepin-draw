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
#include <QDebug>

#include <QGraphicsItem>
#include <QtConcurrent>

#include <DMenu>


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

void CCentralwidget::initUI()
{
    m_leftToolbar = new CLeftToolBar(this);

//    m_leftToolbar->setStyleSheet("background-color: rgb(0,255, 0);");

    m_pGraphicsView = new CGraphicsView(this);

//    m_pGraphicsView->setStyleSheet("padding: 0px; border: 0px;");
//    m_pGraphicsView->setStyleSheet("background-color: rgb(255,0, 0);");
    m_pDrawScene = new CDrawScene(this);
    QRectF rc = QRectF(0, 0, 800, 600);
    m_pDrawScene->setSceneRect(rc);
    m_pDrawScene->setBackgroundBrush(QColor(248, 248, 251));

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
    m_pGraphicsView->showSaveDDFDialog();
}

void CCentralwidget::slotImport()
{
    m_pGraphicsView->doImport();
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
    m_pGraphicsView->setIsShowContext(false);
    m_pDrawScene->showCutItem();
}

void CCentralwidget::slotQuitCutMode()
{
    m_pGraphicsView->setIsShowContext(true);
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

    connect(m_pDrawScene, SIGNAL(signalQuitCutMode()), m_leftToolbar, SLOT(slotQuitCutMode()));
    connect(m_pDrawScene, SIGNAL(signalQuitCutMode()), this, SLOT(slotQuitCutMode()));

    connect(m_pDrawScene, SIGNAL(signalDoCut(QRectF)), m_pGraphicsView, SLOT(slotDoCut(QRectF)));
    connect(m_pDrawScene, &CDrawScene::signalUpdateCutSize, this, &CCentralwidget::signalUpdateCutSize);
}
bool CCentralwidget::getTextEditable()
{
    //如果是文字图元则显示其自己的右键菜单
    if (!m_pDrawScene->selectedItems().isEmpty()) {
        QGraphicsItem *item =  m_pDrawScene->selectedItems().first();
        CGraphicsItem *tmpitem = static_cast<CGraphicsItem *>(item);
        if (TextType == item->type() &&  static_cast<CGraphicsTextItem *>(tmpitem)->isEditable()) {
            //文字图元的显示需要获取view的大小，才能保证显示完整
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}
