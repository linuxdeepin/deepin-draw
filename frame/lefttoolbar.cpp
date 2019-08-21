#include "lefttoolbar.h"

#include "widgets/pushbutton.h"

#include <QLabel>
#include <QVBoxLayout>

const int BTN_SPACING = 1;

LeftToolBar::LeftToolBar(QWidget *parent) : DFrame(parent)
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
    m_picBtn = new PushButton(this);
    m_picBtn->setObjectName("PictureBtn");
    m_picBtn->setToolTip(tr("Import"));
    m_actionButtons.append(m_picBtn);

    m_rectBtn = new PushButton(this);
    m_rectBtn->setObjectName("RectBtn");
    m_rectBtn->setToolTip(tr("Rectangle"));
    m_actionButtons.append(m_rectBtn);

    m_roundBtn = new PushButton(this);
    m_roundBtn->setObjectName("RoundBtn");
    m_roundBtn->setToolTip(tr("Ellipse"));
    m_actionButtons.append(m_roundBtn);

    m_triangleBtn = new PushButton(this);
    m_triangleBtn->setObjectName("TriangleBtn");
    m_triangleBtn->setToolTip(tr("Triangle"));
    m_actionButtons.append(m_triangleBtn);


    m_starBtn = new PushButton(this);
    m_starBtn->setObjectName("StarBtn");
    m_starBtn->setToolTip(tr("Star"));
    m_actionButtons.append(m_starBtn);



    m_polygonBtn = new PushButton(this);
    m_polygonBtn->setObjectName("PolygonBtn");
    m_polygonBtn->setToolTip(tr("Polygon"));
    m_actionButtons.append(m_polygonBtn);


    m_lineBtn = new PushButton(this);
    m_lineBtn->setObjectName("LineBtn");
    m_lineBtn->setToolTip(tr("Line"));
    m_actionButtons.append(m_lineBtn);


    m_penBtn = new PushButton(this);
    m_penBtn->setObjectName("PenBtn");
    m_penBtn->setToolTip(tr("Pencil"));
    m_actionButtons.append(m_penBtn);


    m_textBtn = new PushButton(this);
    m_textBtn->setObjectName("TextBtn");
    m_textBtn->setToolTip(tr("Text"));
    m_actionButtons.append(m_textBtn);


    m_blurBtn = new PushButton(this);
    m_blurBtn->setObjectName("BlurBtn");
    m_blurBtn->setToolTip(tr("Blur"));
    m_actionButtons.append(m_blurBtn);


    m_cutBtn = new PushButton(this);
    m_cutBtn->setObjectName("CutBtn");
    m_cutBtn->setToolTip(tr("Cut"));
    m_actionButtons.append(m_cutBtn);


    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_picBtn);
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

void LeftToolBar::clearOtherSelections(PushButton *clickedButton)
{
    foreach (PushButton *button, m_actionButtons) {
        button->setChecked(false);
    };

    clickedButton->setChecked(true);
}

void LeftToolBar::initConnection()
{
    connect(m_picBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_picBtn);
        emit setCurrentDrawTool(ImportPicture);
//        DrawTool::c_drawShape = rectangle;
    });

    connect(m_rectBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_rectBtn);
        emit setCurrentDrawTool(CommonShape);
        DrawTool::c_drawShape = rectangle;
    });

    connect(m_roundBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_roundBtn);
        emit setCurrentDrawTool(CommonShape);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_triangleBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_triangleBtn);
        emit setCurrentDrawTool(CommonShape);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_starBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_starBtn);
        emit setCurrentDrawTool(DrawPolygonalStar);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_polygonBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_polygonBtn);
        emit setCurrentDrawTool(DrawPolygon);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_lineBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_lineBtn);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_penBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_penBtn);
        emit setCurrentDrawTool(DrawPen);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_textBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_textBtn);
        emit setCurrentDrawTool(DrawText);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_blurBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_blurBtn);
        emit setCurrentDrawTool(DrawBlur);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_cutBtn, &PushButton::clicked, this, [this]() {
        clearOtherSelections(m_cutBtn);
        emit setCurrentDrawTool(Cut);
//        DrawTool::c_drawShape = ellipse;
    });
}
