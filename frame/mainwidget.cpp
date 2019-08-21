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
    connect(m_leftToolbar, SIGNAL(sendPicPath(QStringList)), this, SLOT(getPicPath(QStringList)));
}




MainWidget::~MainWidget()
{
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
        //画图



    }
}


LeftToolBar *MainWidget::getLeftToolBar()
{
    return m_leftToolbar;
}
