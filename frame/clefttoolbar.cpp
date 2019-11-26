/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "clefttoolbar.h"

#include "widgets/ccheckbutton.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawtoolfactory.h"
#include "drawshape/cdrawtoolmanagersigleton.h"
#include "drawshape/cdrawparamsigleton.h"


#include <DFileDialog>
#include <DWidget>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QVBoxLayout>
#include <QDebug>
#include <QAction>
#include <DApplicationHelper>



DGUI_USE_NAMESPACE

const int BTN_SPACING = 1;

CLeftToolBar::CLeftToolBar(DFrame *parent)
    : DFrame(parent)
{
    this->setFrameRounded(false);
    this->setFrameShape(QFrame::NoFrame);

    setMaximumWidth(50);

    initUI();
    initConnection();
    initDrawTools();
    initShortCut();
    initShortCutConnection();
}

CLeftToolBar::~CLeftToolBar()
{

}

void CLeftToolBar::mouseMoveEvent(QMouseEvent *event)
{
    //禁止拖动
    Q_UNUSED(event)
}

void CLeftToolBar::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    qApp->setOverrideCursor(Qt::ArrowCursor);
    DWidget::enterEvent(event);
}


void CLeftToolBar::initUI()
{
    //设置颜色
    DPalette pa = this->palette();

//    if (CDrawParamSigleton::GetInstance()->getThemeType() == 1) {
//        pa.setColor(DPalette::Background, QColor("#FFFFFF"));
//    } else {
//        pa.setColor(DPalette::Background, QColor("#282828"));
//    }
    pa.setColor(DPalette::Background, DPalette::Base);
    this->setPalette(pa);
    this->setAutoFillBackground(true);

    setFixedWidth(58);
    QMap<int, QMap<CCheckButton::EButtonSattus, QString> > pictureMap;

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/choose tools_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/choose tools_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/choose tools_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/choose tools_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/choose tools_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/choose tools_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/choose tools_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/choose tools_active.svg");

    m_selectBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_picBtn->setFocusPolicy(Qt::NoFocus);
    m_selectBtn->setToolTip(tr("Select(V)"));
    m_actionButtons.append(m_selectBtn);

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/picture tools_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/picture tools_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/picture tools_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/picture tools_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/picture tools_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/picture tools_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/picture tools_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/picture tools_active.svg");

    m_picBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_picBtn->setFocusPolicy(Qt::NoFocus);
    m_picBtn->setToolTip(tr("Import(I)"));
    m_actionButtons.append(m_picBtn);

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/rectangle tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/rectangle tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/rectangle tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/rectangle tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/rectangle tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/rectangle tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/rectangle tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/rectangle tool_active.svg");

    m_rectBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_rectBtn->setFocusPolicy(Qt::NoFocus);
    m_rectBtn->setToolTip(tr("Rectangle(R)"));
    m_actionButtons.append(m_rectBtn);

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/round tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/round tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/round tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/round tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/round tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/round tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/round tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/round tool_active.svg");

    m_roundBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_roundBtn->setFocusPolicy(Qt::NoFocus);
    m_roundBtn->setToolTip(tr("Ellipse(O)"));
    m_actionButtons.append(m_roundBtn);



    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/triangle tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/triangle tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/triangle tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/triangle tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/triangle tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/triangle tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/triangle tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/triangle tool_active.svg");

    m_triangleBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_triangleBtn->setFocusPolicy(Qt::NoFocus);
    m_triangleBtn->setToolTip(tr("Triangle(S)"));
    m_actionButtons.append(m_triangleBtn);



    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/star tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/star tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/star tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/star tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/star tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/star tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/star tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/star tool_active.svg");

    m_starBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_starBtn->setFocusPolicy(Qt::NoFocus);
    m_starBtn->setToolTip(tr("Star(F)"));
    m_actionButtons.append(m_starBtn);



    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/hexagon tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/hexagon tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/hexagon tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/hexagon tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/hexagon tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/hexagon tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/hexagon tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/hexagon tool_active.svg");

    m_polygonBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_polygonBtn->setFocusPolicy(Qt::NoFocus);
    m_polygonBtn->setToolTip(tr("Polygon(H)"));
    m_actionButtons.append(m_polygonBtn);



    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/line tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/line tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/line tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/line tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/line tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/line tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/line tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/line tool_active.svg");

    m_lineBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_lineBtn->setFocusPolicy(Qt::NoFocus);
    m_lineBtn->setToolTip(tr("Line(L)"));
    m_actionButtons.append(m_lineBtn);



    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/brush tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/brush tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/brush tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/brush tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/brush tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/brush tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/brush tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/brush tool_active.svg");

    m_penBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_penBtn->setFocusPolicy(Qt::NoFocus);
    m_penBtn->setToolTip(tr("Pencil(P)"));
    m_actionButtons.append(m_penBtn);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/text tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/text tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/text tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/text tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/text tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/text tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/text tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/text tool_active.svg");

    m_textBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_textBtn->setFocusPolicy(Qt::NoFocus);
    m_textBtn->setToolTip(tr("Text(T)"));
    m_actionButtons.append(m_textBtn);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/smudge tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/smudge tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/smudge tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/smudge tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/smudge tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/smudge tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/smudge tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/smudge tool_active.svg");

    m_blurBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    //m_blurBtn->setFocusPolicy(Qt::NoFocus);
    m_blurBtn->setToolTip(tr("Blur(B)"));
    m_actionButtons.append(m_blurBtn);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/action/screenshot tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/action/screenshot too_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/action/screenshot tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/action/screenshot tool_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/action/screenshot tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/action/screenshot too_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/action/screenshot tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/action/screenshot tool_active.svg");

    m_cutBtn = new CCheckButton(pictureMap, QSize(48, 48), this);
    m_cutBtn->setFocusPolicy(Qt::NoFocus);
    m_cutBtn->setToolTip(tr("Crop(C)"));
    m_actionButtons.append(m_cutBtn);

    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addStretch();
    m_layout->addSpacing(BTN_SPACING);
    m_layout->addWidget(m_selectBtn);
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
    m_layout->setContentsMargins(5, 0, 5, 0);
    m_layout->addStretch();
//    m_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    setLayout(m_layout);
}

void CLeftToolBar::slotClearToolSelection()
{
    foreach (CCheckButton *button, m_actionButtons) {
        if (button->isChecked() /*&& button != m_cutBtn*/) {
            button->setChecked(false);
            return;
        }
    }
}

void CLeftToolBar::slotAfterQuitCut()
{
    if (m_cutBtn->isChecked()) {
        slotShortCutSelect();
    }
}

void CLeftToolBar::changeButtonTheme()
{
    int themeType = CDrawParamSigleton::GetInstance()->getThemeType();
    foreach (CCheckButton *button, m_actionButtons) {
        button->setCurrentTheme(themeType);
    };
}

void CLeftToolBar::clearOtherSelections(CCheckButton *clickedButton)
{
    foreach (CCheckButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}



void CLeftToolBar::initConnection()
{

    connect(m_selectBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_selectBtn);
        isCutMode();
        emit setCurrentDrawTool(selection);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
    });

    connect(m_picBtn, &CCheckButton::buttonClick, [this]() {

        clearOtherSelections(m_picBtn);
        isCutMode();
        emit setCurrentDrawTool(importPicture);//modify to set currentDrawTool
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
        emit importPic();
        m_picBtn->setChecked(false);

    });

    connect(m_rectBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_rectBtn);
        isCutMode();
        emit setCurrentDrawTool(rectangle);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(rectangle);
    });

    connect(m_roundBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_roundBtn);
        isCutMode();
        emit setCurrentDrawTool(ellipse);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(ellipse);
    });

    connect(m_triangleBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_triangleBtn);
        isCutMode();
        emit setCurrentDrawTool(triangle);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(triangle);
    });

    connect(m_starBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_starBtn);
        isCutMode();
        emit setCurrentDrawTool(polygonalStar);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(polygonalStar);
    });

    connect(m_polygonBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_polygonBtn);
        isCutMode();
        emit setCurrentDrawTool(polygon);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(polygon);

    });

    connect(m_lineBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_lineBtn);
        isCutMode();
        emit setCurrentDrawTool(line);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(line);

    });

    connect(m_penBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_penBtn);
        isCutMode();
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(pen);
        emit setCurrentDrawTool(pen);

    });

    connect(m_textBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_textBtn);
        isCutMode();
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(text);
        emit setCurrentDrawTool(text);

    });

    connect(m_blurBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_blurBtn);
        isCutMode();
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(blur);
        emit setCurrentDrawTool(blur);

    });

    connect(m_cutBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_cutBtn);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(cut);
        emit setCurrentDrawTool(cut);
        emit signalBegainCut();
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
    pTool = CDrawToolFactory::Create(polygon);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(polygon, pTool);
    pTool = CDrawToolFactory::Create(polygonalStar);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(polygonalStar, pTool);
    pTool = CDrawToolFactory::Create(pen);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(pen, pTool);
    pTool = CDrawToolFactory::Create(cut);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(cut, pTool);
    pTool = CDrawToolFactory::Create(blur);
    CDrawToolManagerSigleton::GetInstance()->insertDrawTool(blur, pTool);
}

void CLeftToolBar::slotShortCutSelect()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_selectBtn->setChecked(true);
    emit m_selectBtn->buttonClick();
}

void CLeftToolBar::slotShortCutPictrue()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_picBtn->setChecked(true);
    emit m_picBtn->buttonClick();
}

void CLeftToolBar::slotShortCutRect()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_rectBtn->setChecked(true);
    emit m_rectBtn->buttonClick();
}

void CLeftToolBar::slotShortCutRound()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_roundBtn->setChecked(true);
    emit m_roundBtn->buttonClick();
}

void CLeftToolBar::slotShortCutTriangle()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_triangleBtn->setChecked(true);
    emit m_triangleBtn->buttonClick();
}

void CLeftToolBar::slotShortCutPolygonalStar()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_starBtn->setChecked(true);
    emit m_starBtn->buttonClick();
}

void CLeftToolBar::slotShortCutPolygon()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_polygonBtn->setChecked(true);
    emit m_polygonBtn->buttonClick();
}

void CLeftToolBar::slotShortCutLine()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_lineBtn->setChecked(true);
    emit m_lineBtn->buttonClick();
}

void CLeftToolBar::slotShortCutPen()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_penBtn->setChecked(true);
    emit m_penBtn->buttonClick();
}

void CLeftToolBar::slotShortCutText()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_textBtn->setChecked(true);
    emit m_textBtn->buttonClick();
}

void CLeftToolBar::slotShortCutBlur()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_blurBtn->setChecked(true);
    emit m_blurBtn->buttonClick();
}

void CLeftToolBar::slotShortCutCut()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    m_cutBtn->setChecked(true);
    emit m_cutBtn->buttonClick();
}

void CLeftToolBar::initShortCut()
{
    m_selectAction = new QAction(this);
    m_selectAction->setShortcut(QKeySequence(Qt::Key_V));
    this->addAction(m_selectAction);

    m_pictureAction = new QAction(this);
    m_pictureAction->setShortcut(QKeySequence(Qt::Key_I));
    this->addAction(m_pictureAction);

    m_rectAction = new QAction(this);
    m_rectAction->setShortcut(QKeySequence(Qt::Key_R));
    this->addAction(m_rectAction);

    m_roundAction = new QAction(this);
    m_roundAction->setShortcut(QKeySequence(Qt::Key_O));
    this->addAction(m_roundAction);

    m_triangleAction = new QAction(this);
    m_triangleAction->setShortcut(QKeySequence(Qt::Key_S));
    this->addAction(m_triangleAction);

    m_starAction = new QAction(this);
    m_starAction->setShortcut(QKeySequence(Qt::Key_F));
    this->addAction(m_starAction);

    m_polygonAction = new QAction(this);
    m_polygonAction->setShortcut(QKeySequence(Qt::Key_H));
    this->addAction(m_polygonAction);

    m_lineAction = new QAction(this);
    m_lineAction->setShortcut(QKeySequence(Qt::Key_L));
    this->addAction(m_lineAction);

    m_penAction = new QAction(this);
    m_penAction->setShortcut(QKeySequence(Qt::Key_P));
    this->addAction(m_penAction);

    m_textAction = new QAction(this);
    m_textAction->setShortcut(QKeySequence(Qt::Key_T));
    this->addAction(m_textAction);

    m_blurAction = new QAction(this);
    m_blurAction->setShortcut(QKeySequence(Qt::Key_B));
    this->addAction(m_blurAction);

    m_cutAction = new QAction(this);
    m_cutAction->setShortcut(QKeySequence(Qt::Key_C));
    this->addAction(m_cutAction);
}

void CLeftToolBar::initShortCutConnection()
{
    connect(m_selectAction, SIGNAL(triggered()), this, SLOT(slotShortCutSelect()));
    connect(m_pictureAction, SIGNAL(triggered()), this, SLOT(slotShortCutPictrue()));
    connect(m_rectAction, SIGNAL(triggered()), this, SLOT(slotShortCutRect()));
    connect(m_roundAction, SIGNAL(triggered()), this, SLOT(slotShortCutRound()));
    connect(m_triangleAction, SIGNAL(triggered()), this, SLOT(slotShortCutTriangle()));
    connect(m_starAction, SIGNAL(triggered()), this, SLOT(slotShortCutPolygonalStar()));
    connect(m_polygonAction, SIGNAL(triggered()), this, SLOT(slotShortCutPolygon()));
    connect(m_lineAction, SIGNAL(triggered()), this, SLOT(slotShortCutLine()));
    connect(m_penAction, SIGNAL(triggered()), this, SLOT(slotShortCutPen()));
    connect(m_textAction, SIGNAL(triggered()), this, SLOT(slotShortCutText()));
    connect(m_blurAction, SIGNAL(triggered()), this, SLOT(slotShortCutBlur()));
    connect(m_cutAction, SIGNAL(triggered()), this, SLOT(slotShortCutCut()));
}

void CLeftToolBar::isCutMode()
{
    if (cut == CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode()) {
        emit singalDoCutFromLeftToolBar();
    }
}

