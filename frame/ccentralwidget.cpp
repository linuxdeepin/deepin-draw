#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsitem.h"
#include "widgets/progresslayout.h"
#include "drawshape/cpictureitem.h"
#include "cgraphicsview.h"
#include "drawshape/cpicturetool.h"

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


//进行图片导入
void CCentralwidget::importPicture()
{
    CPictureTool *pictureTool = new CPictureTool();
    pictureTool->drawPicture(m_pDrawScene, this);

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
    m_pDrawScene->setBackgroundBrush(Qt::darkGray);

    m_pGraphicsView->setScene(m_pDrawScene);
    m_pGraphicsView->setAlignment(Qt::AlignCenter);
    m_pGraphicsView->setRenderHint(QPainter::Antialiasing);//设置反走样

//    m_pGraphicsView->fitInView(rc, Qt::KeepAspectRatio);

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

void CCentralwidget::slotNew()
{
    m_pDrawScene->clear();
}

void CCentralwidget::slotPrint()
{
    m_pGraphicsView->showPrintDialog();
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
}
