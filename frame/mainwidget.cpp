#include "mainwidget.h"

#include <QLabel>
#include <QDebug>


#include "utils/global.h"
#include "widgets/graphicsgloweffect.h"
#include "utils/shapesutils.h"
#include "lefttoolbar.h"
#include "drawshape/maingraphicsview.h"
#include "drawshape/maingraphicsscene.h"


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
    m_MainGraphicsScene->setSceneRect(QRectF(0,0,1200,800));
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
}


MainWidget::~MainWidget()
{
}

LeftToolBar* MainWidget::getLeftToolBar()
{
    return m_leftToolbar;
}
