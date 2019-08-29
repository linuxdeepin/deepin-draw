#include "clefttoolbar.h"

#include "widgets/cpushbutton.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawtoolfactory.h"
#include "drawshape/cdrawtoolmanagersigleton.h"
#include "drawshape/cdrawparamsigleton.h"

#include <DFileDialog>

#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

const int BTN_SPACING = 1;

CLeftToolBar::CLeftToolBar(QWidget *parent)
    : DFrame(parent)
{
    setMaximumWidth(50);

    initUI();
    initConnection();
    initDrawTools();
}

CLeftToolBar::~CLeftToolBar()
{

}

void CLeftToolBar::initUI()
{

    QMap<CPushButton::CButtonSattus, QString> pictureMap;

    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/picture tools_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/picture tools_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/picture tools_press.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/picture tools_active.svg");
    m_picBtn = new CPushButton(pictureMap, this);
    m_picBtn->setToolTip(tr("Import"));
    m_actionButtons.append(m_picBtn);


    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/action/selected_fill_normal.svg");
    pictureMap[CPushButton::Hover] = QString(":/theme/light/images/action/selected_fill_hover.svg");
    pictureMap[CPushButton::Press] = QString(":/theme/light/images/action/selected_fill_active.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/action/selected_fill_active.svg");
    m_selectBtn = new CPushButton(pictureMap, this);
    m_selectBtn->setToolTip(tr("Select"));
    m_actionButtons.append(m_selectBtn);

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
    m_layout->addWidget(m_selectBtn);
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

void CLeftToolBar::clearOtherSelections(CPushButton *clickedButton)
{
    foreach (CPushButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}

void CLeftToolBar::importImage()
{
    qDebug() << "importImage begin" << endl;
    DFileDialog *fileDialog = new DFileDialog(this);
    QStringList filters;
    filters << "Image files (*.png *.jpg *.bmp *.tif *.pdf *.ddf)";
    fileDialog->setNameFilters(filters);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    if (fileDialog->exec() ==   QDialog::Accepted) {
        QStringList filenames = fileDialog->selectedFiles();
        qDebug() << filenames << endl;
        emit sendPicPath(filenames);

    }

}

void CLeftToolBar::initConnection()
{

    connect(m_picBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_picBtn);
        emit setCurrentDrawTool(importPicture);
        importImage();

    });

    //链接图片导入按钮和图片导入功能
    // connect(m_picBtn, SIGNAL(clicked()), this, SLOT(importImage()));

    connect(m_selectBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_selectBtn);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
    });

    connect(m_rectBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_rectBtn);
        emit setCurrentDrawTool(rectangle);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(rectangle);
    });

    connect(m_roundBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_roundBtn);
        emit setCurrentDrawTool(ellipse);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(ellipse);
    });

    connect(m_triangleBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_triangleBtn);
        emit setCurrentDrawTool(triangle);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(triangle);
    });

    connect(m_starBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_starBtn);
        emit setCurrentDrawTool(polygonalStar);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(polygonalStar);
    });

    connect(m_polygonBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_polygonBtn);
        emit setCurrentDrawTool(polygon);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_lineBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_lineBtn);
        emit setCurrentDrawTool(line);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(line);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_penBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_penBtn);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(pen);
        emit setCurrentDrawTool(pen);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_textBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_textBtn);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(text);
        emit setCurrentDrawTool(text);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_blurBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_blurBtn);
        emit setCurrentDrawTool(blur);
//        DrawTool::c_drawShape = ellipse;
    });

    connect(m_cutBtn, &CPushButton::buttonClick, [this]() {
        clearOtherSelections(m_cutBtn);
        emit setCurrentDrawTool(cut);
//        DrawTool::c_drawShape = ellipse;
    });
}

void CLeftToolBar::initDrawTools()
{
    IDrawTool *pTool = CDrawToolFactory::Create(selection);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(selection, pTool);
    pTool = CDrawToolFactory::Create(rectangle);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(rectangle, pTool);
    pTool = CDrawToolFactory::Create(ellipse);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(ellipse, pTool);
    pTool = CDrawToolFactory::Create(line);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(line, pTool);
    pTool = CDrawToolFactory::Create(triangle);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(triangle, pTool);
    pTool = CDrawToolFactory::Create(text);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(text, pTool);
    pTool = CDrawToolFactory::Create(polygonalStar);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(polygonalStar, pTool);
}
