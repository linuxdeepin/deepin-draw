#include "lefttoolbar.h"

#include "widgets/cpushbutton.h"

#include <QVBoxLayout>


const int BTN_SPACING = 1;

LeftToolBar::LeftToolBar(QWidget *parent) : DFrame(parent)
{
    setMaximumWidth(50);

    initUI();
    initConnection();

}

LeftToolBar::~LeftToolBar()
{

}

void LeftToolBar::initUI()
{

    QMap<CPushButton::CButtonSattus, QString> pictureMap;

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/picture tools_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/picture tools_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/picture tools_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/picture tools_active.svg");
    m_picBtn = new CPushButton(pictureMap, this);
    m_picBtn->setToolTip(tr("Import"));
    m_actionButtons.append(m_picBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/rectangle tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/rectangle tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/rectangle tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/rectangle tool_active.svg");
    m_rectBtn = new CPushButton(pictureMap, this);
    m_rectBtn->setToolTip(tr("Rectangle"));
    m_actionButtons.append(m_rectBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/round tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/round tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/round tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/round tool_active.svg");
    m_roundBtn = new CPushButton(pictureMap, this);
    m_roundBtn->setToolTip(tr("Ellipse"));
    m_actionButtons.append(m_roundBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/triangle tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/triangle tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/triangle tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/triangle tool_active.svg");
    m_triangleBtn = new CPushButton(pictureMap, this);
    m_triangleBtn->setToolTip(tr("Triangle"));
    m_actionButtons.append(m_triangleBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/star tool_normal.png");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/star tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/star tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/star tool_active.svg");
    m_starBtn = new CPushButton(pictureMap, this);
    m_starBtn->setToolTip(tr("Star"));
    m_actionButtons.append(m_starBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/hexagon tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/hexagon tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/hexagon tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/hexagon tool_active.svg");
    m_polygonBtn = new CPushButton(pictureMap, this);
    m_polygonBtn->setToolTip(tr("Polygon"));
    m_actionButtons.append(m_polygonBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/line tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/line tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/line tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/line tool_active.svg");
    m_lineBtn = new CPushButton(pictureMap, this);
    m_lineBtn->setToolTip(tr("Line"));
    m_actionButtons.append(m_lineBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/brush tool_normal.png");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/brush tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/brush tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/brush tool_active.svg");
    m_penBtn = new CPushButton(pictureMap, this);
    m_penBtn->setToolTip(tr("Pencil"));
    m_actionButtons.append(m_penBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/text tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/text tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/text tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/text tool_active.svg");
    m_textBtn = new CPushButton(pictureMap, this);
    m_textBtn->setToolTip(tr("Text"));
    m_actionButtons.append(m_textBtn);



    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/smudge tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/smudge tool_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/smudge tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/smudge tool_active.svg");
    m_blurBtn = new CPushButton(pictureMap, this);
    m_blurBtn->setToolTip(tr("Blur"));
    m_actionButtons.append(m_blurBtn);


    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/screenshot tool_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/screenshot too_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/screenshot tool_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/screenshot tool_active.svg");
    m_cutBtn = new CPushButton(pictureMap, this);
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

void LeftToolBar::clearOtherSelections(CPushButton *clickedButton)
{
    foreach (CPushButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}

void LeftToolBar::initConnection()
{

    connect(m_picBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_picBtn);
        emit setCurrentDrawTool(ImportPicture);
//        DrawTool::c_drawShape = rectangle;
    });

    connect(m_rectBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_rectBtn);
        emit setCurrentDrawTool(CommonShape);
        DrawTool::c_drawShape = rectangle;
    });

    connect(m_roundBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_roundBtn);
        emit setCurrentDrawTool(CommonShape);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_triangleBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_triangleBtn);
        emit setCurrentDrawTool(CommonShape);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_starBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_starBtn);
        emit setCurrentDrawTool(DrawPolygonalStar);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_polygonBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_polygonBtn);
        emit setCurrentDrawTool(DrawPolygon);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_lineBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_lineBtn);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_penBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_penBtn);
        emit setCurrentDrawTool(DrawPen);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_textBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_textBtn);
        emit setCurrentDrawTool(DrawText);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_blurBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_blurBtn);
        emit setCurrentDrawTool(DrawBlur);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_cutBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_cutBtn);
        emit setCurrentDrawTool(Cut);
//        DrawTool::c_drawShape = ellipse;
    });
}
