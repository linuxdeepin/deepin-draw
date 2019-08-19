#include "lefttoolbar.h"

#include "widgets/pushbutton.h"

#include <QLabel>
#include <QVBoxLayout>

const int BTN_SPACING = 1;

LeftToolBar::LeftToolBar(QWidget *parent) : QFrame(parent)
{
    setMaximumWidth(50);

    DRAW_THEME_INIT_WIDGET("LeftToolbar");
    setObjectName("LeftToolbar");

    initUI();
    initConnection();

}

LeftToolBar::~LeftToolBar()
{

}

void LeftToolBar::initUI()
{
    PushButton *picBtn = new PushButton(this);
    picBtn->setObjectName("PictureBtn");
    picBtn->setToolTip(tr("Import"));
    m_actionPushButtons.insert(picBtn, false);

    m_rectBtn = new PushButton(this);
    m_rectBtn->setObjectName("RectBtn");
    m_rectBtn->setToolTip(tr("Rectangle"));
    m_actionPushButtons.insert(m_rectBtn, false);


    m_roundBtn = new PushButton(this);
    m_roundBtn->setObjectName("RoundBtn");
    m_roundBtn->setToolTip(tr("Ellipse"));
    m_actionPushButtons.insert(m_roundBtn, false);

    m_triangleBtn = new PushButton(this);
    m_triangleBtn->setObjectName("TriangleBtn");
    m_triangleBtn->setToolTip(tr("Triangle"));
    m_actionPushButtons.insert(m_triangleBtn, false);

    m_starBtn = new PushButton(this);
    m_starBtn->setObjectName("StarBtn");
    m_starBtn->setToolTip(tr("Star"));
    m_actionPushButtons.insert(m_starBtn, false);


    m_polygonBtn = new PushButton(this);
    m_polygonBtn->setObjectName("PolygonBtn");
    m_polygonBtn->setToolTip(tr("Polygon"));
    m_actionPushButtons.insert(m_polygonBtn, false);

    m_lineBtn = new PushButton(this);
    m_lineBtn->setObjectName("LineBtn");
    m_lineBtn->setToolTip(tr("Line"));
    m_actionPushButtons.insert(m_lineBtn, false);

    m_penBtn = new PushButton(this);
    m_penBtn->setObjectName("PenBtn");
    m_penBtn->setToolTip(tr("Pencil"));
    m_actionPushButtons.insert(m_penBtn, false);

    m_textBtn = new PushButton(this);
    m_textBtn->setObjectName("TextBtn");
    m_textBtn->setToolTip(tr("Text"));
    m_actionPushButtons.insert(m_textBtn, false);

    m_blurBtn = new PushButton(this);
    m_blurBtn->setObjectName("BlurBtn");
    m_blurBtn->setToolTip(tr("Blur"));
    m_actionPushButtons.insert(m_blurBtn, false);

    m_cutBtn = new PushButton(this);
    m_cutBtn->setObjectName("CutBtn");
    m_cutBtn->setToolTip(tr("Cut"));
    m_actionPushButtons.insert(m_cutBtn, false);

    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(picBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_rectBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_roundBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_triangleBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_starBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_polygonBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_lineBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_penBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_textBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_blurBtn);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_cutBtn);
    m_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    setLayout(m_layout);
}

void LeftToolBar::clearOtherSelection()
{

}

void LeftToolBar::initConnection()
{
    connect(m_rectBtn, &PushButton::clicked, this, [this]() {
//        m_actionPushButtons[m_rectBtn] = true;
        clearOtherSelection();
        emit setCurrentDrawTool(rectangle);
        DrawTool::c_drawShape = rectangle;
    });

    connect(m_roundBtn, &PushButton::clicked, this, [this]() {
//        m_actionPushButtons[m_rectBtn] = true;
        clearOtherSelection();
//        emit setCurrentDrawTool(rectangle);
        DrawTool::c_drawShape = rotation;
    });
}
