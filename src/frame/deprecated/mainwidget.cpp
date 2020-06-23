#include "mainwidget.h"
#include "utils/global.h"
#include "widgets/graphicsgloweffect.h"
#include "utils/shapesutils.h"
#include "lefttoolbar.h"
#include "drawshape/maingraphicsview.h"
#include "drawshape/maingraphicsscene.h"
#include "drawshape/imagegraphicsitem.h"
#include "widgets/progresslayout.h"



#include <DProgressBar>
#include <QtConcurrent>
#include <QLabel>
#include <QDebug>


DWIDGET_USE_NAMESPACE

//const int ARTBOARD_MARGIN = 25;
//const int TITLEBAR_HEIGHT = 40;
//const int MARGIN = 25;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    DRAW_THEME_INIT_WIDGET("MainWidget");
    this->setObjectName("MainWidget");

    m_leftToolbar = new LeftToolBar(this);

    QHBoxLayout *layout = new QHBoxLayout;

    m_MainGraphicsScene = new MainGraphicsScene(this);
    m_MainGraphicsScene->setBackgroundBrush(QBrush(Qt::white));
    m_MainGraphicsScene->setSceneRect(QRectF(0, 0, 1200, 800));
    m_MainGraphicsView = new MainGraphicsView(m_MainGraphicsScene);
    m_MainGraphicsView->showMaximized();
    m_MainGraphicsScene->setView(m_MainGraphicsView);

    auto effect = new GraphicsGlowEffect();

    effect->setBlurRadius(16);
    effect->setColor(QColor("#ececef"));
    effect->setColor(Qt::red);
    m_MainGraphicsView->setGraphicsEffect(effect);

    layout->addWidget(m_leftToolbar);
    layout->addWidget(m_MainGraphicsView);

    setLayout(layout);

    m_importProgressbar = new DProgressBar();


    // m_importProgressbar->se;
    m_importProgressbar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_importProgressbar->hide();

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



    connect(m_leftToolbar, SIGNAL(sendPicPath(QStringList)), this, SLOT(getPicPath(QStringList)));
    //connect(m_contextMenu,SIGNAL(conte))
    connect(this, SIGNAL(sendImageItem(QPixmap)), this, SLOT(addImageItem(QPixmap)));
    connect(this, SIGNAL(loadImageNum(int)), this, SLOT(setProcessBarValue(int)));

//    QWidget *widget = new QWidget(this);
//    QPalette pa1;
////    pa1.setColor(QPalette::WindowText, Qt::black);
////    widget->setPalette(pa1);
//    widget->setGeometry(0, 0, 300, 100);
//    QPalette pal(widget->palette());
//    pal.setColor(QPalette::Background, Qt::gray);
//    widget->setAutoFillBackground(true);
//    widget->setPalette(pal);
//    widget->show();
//    widget->setStyleSheet("background-color: rgb(255,25, 155)");
//    widget->show();

    ProgressLayout *testlayout = new ProgressLayout(this);
    testlayout->setRange(0, 100);
    testlayout->setProgressValue(50);
    testlayout->show();

}


MainWidget::~MainWidget()
{
}

LeftToolBar *MainWidget::getLeftToolBar()
{
    return m_leftToolbar;
}

void MainWidget::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
    m_contextMenu->show();
}
//进行图片导入
void MainWidget::getPicPath(QStringList pathList)
{
    qDebug() << pathList << pathList.size() << endl;

    m_importProgressbar->setMinimum(0);
    m_importProgressbar->setMaximum(pathList.size());
    m_importProgressbar->setValue(0);
    m_importProgressbar->show();

    //启动图片导入线程
    QtConcurrent::run([ = ] {
        for (int i = 0; i < pathList.size(); i++)
        {

            QPixmap pixmap = QPixmap (pathList[i]);

            emit sendImageItem(pixmap);
            emit loadImageNum(i + 1);
            //importProcessbar->setValue(i + 1);
            qDebug() << "importProcessbar" << i + 1 << endl;
        }
    });


}

void MainWidget::setProcessBarValue(int value)
{
    m_importProgressbar->setValue(value);
    m_importProgressbar->setFormat(QString::fromLocal8Bit("已导入%1/%2张").arg(value).arg(m_importProgressbar->maximum()));
    if (value == m_importProgressbar->maximum()) {
        m_importProgressbar->hide();
    }
}


//void MainWidget::importPicture(QString path)
//{
//    QPixmap pixmap = QPixmap (path);

//    emit sendImageItem(pixmap);
//    //m_MainGraphicsScene->addItem(graphicsItem);


//}

void MainWidget::addImageItem( QPixmap pixMap)
{
    ImageGraphicsItem *graphicsItem = new ImageGraphicsItem();
    graphicsItem->setPainter(QPointF(0, 0), pixMap);
    m_MainGraphicsScene->addItem(graphicsItem);
}
