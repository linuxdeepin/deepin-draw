#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsitem.h"

#include <QLabel>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsItem>

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
//进行图片导入
void CCentralwidget::getPicPath(QStringList path)
{
    qDebug() << path << path.size() << endl;
    for (int i = 0; i < path.size(); i++) {
        QPixmap pixmap = QPixmap (path[i]);
        //QPointF pointf;
//        ImageGraphicsItem *graphicsItem = new ImageGraphicsItem();
//        graphicsItem->setPainter(QPointF(0, 0), pixmap);
//        m_MainGraphicsScene->addItem(graphicsItem);
        //graphicsItem->setVisible(true);

    }
}


void CCentralwidget::initUI()
{
    m_leftToolbar = new CLeftToolBar(this);

//    m_leftToolbar->setStyleSheet("background-color: rgb(0,255, 0);");

    m_pGraphicsView = new QGraphicsView(this);
//    m_pGraphicsView->setStyleSheet("padding: 0px; border: 0px;");
//    m_pGraphicsView->setStyleSheet("background-color: rgb(255,0, 0);");
    m_pDrawScene = new CDrawScene();
    m_pGraphicsView->setScene(m_pDrawScene);


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
    QRect rect = m_pGraphicsView->viewport()->rect();
    m_pGraphicsView->setSceneRect(rect);
}

void CCentralwidget::slotAttributeChanged()
{
    if (m_pDrawScene != nullptr) {
        m_pDrawScene->attributeChanged();
    }
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
//    connect(m_leftToolbar, SIGNAL(sendPicPath(QStringList)), this, SLOT(getPicPath(QStringList)));
    //connect(m_contextMenu,SIGNAL(conte))

}
