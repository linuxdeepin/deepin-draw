#include "mainwidget.h"

#include <QLabel>
#include <QDebug>


#include "utils/global.h"
#include "widgets/graphicsgloweffect.h"
#include "utils/shapesutils.h"
#include "lefttoolbar.h"
#include "drawshape/maingraphicsview.h"
#include "drawshape/maingraphicsscene.h"
#include "drawshape/imagegraphicsitem.h"

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
void MainWidget::getPicPath(QStringList path)
{
    qDebug() << path << path.size() << endl;
    for (int i = 0; i < path.size(); i++) {
        QPixmap pixmap = QPixmap (path[i]);
        //QPointF pointf;
        ImageGraphicsItem *graphicsItem = new ImageGraphicsItem();
        graphicsItem->setPainter(QPointF(0, 0), pixmap);
        m_MainGraphicsScene->addItem(graphicsItem);
        //graphicsItem->setVisible(true);

    }
}
