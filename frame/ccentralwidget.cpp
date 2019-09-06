#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsitem.h"
#include "widgets/progresslayout.h"
#include "drawshape/cpictureitem.h"
#include "cgraphicsview.h"
#include "drawshape/cpicturetool.h"

#include <QLabel>
#include <QDebug>

#include <QGraphicsItem>
#include <QtConcurrent>

#include <DMenu>


CCentralwidget::CCentralwidget(QWidget *parent)
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

void CCentralwidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)

    m_contextMenu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
    m_contextMenu->show();
}

CGraphicsView *CCentralwidget::getGraphicsView() const
{
    return m_pGraphicsView;
}
//进行图片导入
void CCentralwidget::importPicture()
{
    CPictureTool *pictureTool = new CPictureTool();
    pictureTool->drawPicture(m_pDrawScene, window());

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

    initContextMenu();

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

void CCentralwidget::slotSetScale(const qreal scale)
{
    emit signalSetScale(scale);
}

void CCentralwidget::initContextMenu()
{
    m_contextMenu = new DMenu(this);
    QAction *cutAc = m_contextMenu->addAction(tr("Cut"));
    QAction *copyAc = m_contextMenu->addAction(tr("Copy"));
    QAction *parseAc = m_contextMenu->addAction(tr("Parse"));
    QAction *allAc = m_contextMenu->addAction(tr("All"));
    m_contextMenu->addSeparator();
    QAction *deleteAc = m_contextMenu->addAction(tr("Delete"));
    QAction *undoAc = m_contextMenu->addAction(tr("Undo"));
    m_contextMenu->addSeparator();
    QAction *oneLayerUpAc = m_contextMenu->addAction(tr("One layer up"));
    QAction *oneLayerDownAc = m_contextMenu->addAction(tr("One layer down"));
    QAction *bringToFrontAc = m_contextMenu->addAction(tr("Bring to front"));
    QAction *sendTobackAc = m_contextMenu->addAction(tr("Send to back"));
    QAction *leftAlignAc = m_contextMenu->addAction(tr("Left align"));
    QAction *topAlignAc = m_contextMenu->addAction(tr("Top align"));
    QAction *rightAlignAc = m_contextMenu->addAction(tr("Right align"));
    QAction *centerAlignAc = m_contextMenu->addAction(tr("Center align"));
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

}
