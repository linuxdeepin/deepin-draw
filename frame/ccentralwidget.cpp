#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsitem.h"
#include "widgets/progresslayout.h"

#include <QLabel>
#include <QDebug>
#include <QGraphicsView>
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
//进行图片导入
void CCentralwidget::getPicPath(QStringList pathList)
{

    qDebug() << pathList << pathList.size() << endl;
    m_picNum = pathList.size();
    m_progressLayout->setRange(0, pathList.size());
    // m_progressLayout->setProgressValue(0);
    m_progressLayout->show();

    //启动图片导入线程
    QtConcurrent::run([ = ] {
        for (int i = 0; i < pathList.size(); i++)
        {

            QPixmap pixmap = QPixmap (pathList[i]);

            emit sendImageItem(pixmap);
            emit loadImageNum(i + 1);
            //qDebug() << "importProcessbar" << i + 1 << endl;
        }
    });



}

void CCentralwidget::addImageItem( QPixmap pixMap)
{

    //qDebug() << "entered the  addImageItem function" << endl;
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixMap);
    m_pDrawScene->addItem(pixmapItem);
}


void CCentralwidget::setProcessBarValue(int value)
{
    //qDebug() << "entered the  setProcessBarValue function" << endl;
    m_progressLayout->setProgressValue(value);

    if (value == m_picNum) {
        m_progressLayout->hide();
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
    m_pGraphicsView->setRenderHint(QPainter::Antialiasing);//设置反走样

    m_progressLayout = new ProgressLayout(this);


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
    connect(m_leftToolbar, SIGNAL(sendPicPath(QStringList)), this, SLOT(getPicPath(QStringList)));
    //connect(m_contextMenu,SIGNAL(conte))
    connect(this, SIGNAL(sendImageItem(QPixmap)), this, SLOT(addImageItem(QPixmap)));
    connect(this, SIGNAL(loadImageNum(int)), this, SLOT(setProcessBarValue(int)));

    connect(m_pDrawScene, &CDrawScene::signalAttributeChanged, this, &CCentralwidget::signalAttributeChangedFromScene);


}
