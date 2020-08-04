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
#include "drawTools/cdrawtoolfactory.h"
#include "drawTools/cdrawtoolmanagersigleton.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "application.h"

#include <DFileDialog>
#include <DWidget>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QVBoxLayout>
#include <QDebug>
#include <QAction>
#include <DApplicationHelper>

DGUI_USE_NAMESPACE

const int BTN_SPACING = 12;

CLeftToolBar::CLeftToolBar(DFrame *parent)
    : DFrame(parent)
{
    this->setFrameRounded(false);
    this->setFrameShape(QFrame::NoFrame);

    setMaximumWidth(50);

    initUI();
    QMetaObject::invokeMethod(this, [ = ]() {
        initConnection();
        initDrawTools();
        initShortCut();
        initShortCutConnection();
    }, Qt::QueuedConnection);
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
    dApp->setApplicationCursor(Qt::ArrowCursor);
    DWidget::enterEvent(event);
}

void CLeftToolBar::initUI()
{
    //设置颜色
    DPalette pa = this->palette();
    pa.setColor(DPalette::Background, DPalette::Base);
    this->setPalette(pa);
    this->setAutoFillBackground(true);

    setFixedWidth(58);

    m_selectBtn = new DToolButton(this);
    m_selectBtn->setToolTip(tr("Select(V)"));
    m_selectBtn->setIconSize(QSize(48, 48));
    m_selectBtn->setFixedSize(QSize(37, 37));
    m_selectBtn->setCheckable(true);
    m_actionButtons.append(m_selectBtn);
    m_selectBtn->setVisible(false);

    m_picBtn = new DToolButton(this);
    m_picBtn->setToolTip(tr("Import(I)"));
    m_picBtn->setIconSize(QSize(48, 48));
    m_picBtn->setFixedSize(QSize(37, 37));
    m_picBtn->setCheckable(true);
    m_actionButtons.append(m_picBtn);

    m_rectBtn = new DToolButton(this);
    m_rectBtn->setToolTip(tr("Rectangle(R)"));
    m_rectBtn->setIconSize(QSize(48, 48));
    m_rectBtn->setFixedSize(QSize(37, 37));
    m_rectBtn->setCheckable(true);
    m_actionButtons.append(m_rectBtn);

    m_roundBtn = new DToolButton(this);
    m_roundBtn->setToolTip(tr("Ellipse(O)"));
    m_roundBtn->setIconSize(QSize(48, 48));
    m_roundBtn->setFixedSize(QSize(37, 37));
    m_roundBtn->setCheckable(true);
    m_actionButtons.append(m_roundBtn);

    m_triangleBtn = new DToolButton(this);
    m_triangleBtn->setToolTip(tr("Triangle(S)"));
    m_triangleBtn->setIconSize(QSize(48, 48));
    m_triangleBtn->setFixedSize(QSize(37, 37));
    m_triangleBtn->setCheckable(true);
    m_actionButtons.append(m_triangleBtn);

    m_starBtn = new DToolButton(this);
    m_starBtn->setToolTip(tr("Star(F)"));
    m_starBtn->setIconSize(QSize(48, 48));
    m_starBtn->setFixedSize(QSize(37, 37));
    m_starBtn->setCheckable(true);
    m_actionButtons.append(m_starBtn);

    m_polygonBtn = new DToolButton(this);
    m_polygonBtn->setToolTip(tr("Polygon(H)"));
    m_polygonBtn->setIconSize(QSize(48, 48));
    m_polygonBtn->setFixedSize(QSize(37, 37));
    m_polygonBtn->setCheckable(true);
    m_actionButtons.append(m_polygonBtn);

    m_lineBtn = new DToolButton(this);
    m_lineBtn->setToolTip(tr("Line(L)"));
    m_lineBtn->setIconSize(QSize(48, 48));
    m_lineBtn->setFixedSize(QSize(37, 37));
    m_lineBtn->setCheckable(true);
    m_actionButtons.append(m_lineBtn);

    m_penBtn = new DToolButton(this);
    m_penBtn->setToolTip(tr("Pencil(P)"));
    m_penBtn->setIconSize(QSize(48, 48));
    m_penBtn->setFixedSize(QSize(37, 37));
    m_penBtn->setCheckable(true);
    m_actionButtons.append(m_penBtn);

    m_textBtn = new DToolButton(this);
    m_textBtn->setToolTip(tr("Text(T)"));
    m_textBtn->setIconSize(QSize(48, 48));
    m_textBtn->setFixedSize(QSize(37, 37));
    m_textBtn->setCheckable(true);
    m_actionButtons.append(m_textBtn);

    m_blurBtn = new DToolButton(this);
    m_blurBtn->setToolTip(tr("Blur(B)"));
    m_blurBtn->setIconSize(QSize(48, 48));
    m_blurBtn->setFixedSize(QSize(37, 37));
    m_blurBtn->setCheckable(true);
    m_actionButtons.append(m_blurBtn);

    m_cutBtn = new DToolButton(this);
    m_cutBtn->setToolTip(tr("Crop(C)"));
    m_cutBtn->setIconSize(QSize(48, 48));
    m_cutBtn->setFixedSize(QSize(37, 37));
    m_cutBtn->setCheckable(true);
    m_actionButtons.append(m_cutBtn);


    m_selectBtn->setIcon(QIcon::fromTheme("ddc_choose tools_normal"));
    m_picBtn->setIcon(QIcon::fromTheme("ddc_picture tools_normal"));
    m_rectBtn->setIcon(QIcon::fromTheme(("ddc_rectangle tool_normal")));
    m_roundBtn->setIcon(QIcon::fromTheme("ddc_round tool_normal"));
    m_triangleBtn->setIcon(QIcon::fromTheme("ddc_triangle tool_normal"));
    m_starBtn->setIcon(QIcon::fromTheme("ddc_star tool_normal"));
    m_polygonBtn->setIcon(QIcon::fromTheme("ddc_hexagon tool_normal"));
    m_lineBtn->setIcon(QIcon::fromTheme("ddc_line tool_normal"));
    m_penBtn->setIcon(QIcon::fromTheme("ddc_brush tool_normal"));
    m_textBtn->setIcon(QIcon::fromTheme("ddc_text tool_normal"));
    m_blurBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_normal"));
    m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_normal"));

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
    m_layout->setContentsMargins(10, 0, 0, 0);
    m_layout->addStretch();
    setLayout(m_layout);
}

void CLeftToolBar::slotAfterQuitCut()
{
    if (m_cutBtn->isChecked()) {
        slotShortCutSelect();
    }
}

void CLeftToolBar::slotEnterCut()
{
    if (!m_cutBtn->isChecked()) {
        m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_active"));
        m_cutBtn->setChecked(true);
    }
}

void CLeftToolBar::clearOtherSelections(DToolButton *clickedButton)
{
    foreach (DToolButton *button, m_actionButtons) {
        if (button != clickedButton && button->isChecked()) {
            if (button == m_selectBtn) {
                button->setIcon(QIcon::fromTheme("ddc_choose tools_normal"));
                button->setChecked(false);
            } else if (button == m_picBtn) {
                button->setIcon(QIcon::fromTheme("ddc_picture tools_normal"));
                button->setChecked(false);
            } else if (button == m_rectBtn) {
                button->setIcon(QIcon::fromTheme("ddc_rectangle tool_normal"));
                button->setChecked(false);
            } else if (button == m_roundBtn) {
                button->setIcon(QIcon::fromTheme("ddc_round tool_normal"));
                button->setChecked(false);
            } else if (button == m_triangleBtn) {
                m_triangleBtn->setIcon(QIcon::fromTheme("ddc_triangle tool_normal"));
                m_triangleBtn->setChecked(false);
            } else if (button == m_starBtn) {
                m_starBtn->setIcon(QIcon::fromTheme("ddc_star tool_normal"));
                m_starBtn->setChecked(false);
            } else if (button == m_polygonBtn) {
                m_polygonBtn->setIcon(QIcon::fromTheme("ddc_hexagon tool_normal"));
                m_polygonBtn->setChecked(false);
            } else if (button == m_lineBtn) {
                m_lineBtn->setIcon(QIcon::fromTheme("ddc_line tool_normal"));
                m_lineBtn->setChecked(false);
            } else if (button == m_penBtn) {
                m_penBtn->setIcon(QIcon::fromTheme("ddc_brush tool_normal"));
                m_penBtn->setChecked(false);
            } else if (button == m_textBtn) {
                m_textBtn->setIcon(QIcon::fromTheme("ddc_text tool_normal"));
                m_textBtn->setChecked(false);
            } else if (button == m_blurBtn) {
                m_blurBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_normal"));
                m_blurBtn->setChecked(false);
            } else if (button == m_cutBtn) {
                m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_normal"));
                m_cutBtn->setChecked(false);
            }
            break;
        }
    }
}

void CLeftToolBar::initConnection()
{
    connect(m_selectBtn, &DToolButton::clicked, [this]() {
        doSelectToolChanged(true);
    });

    connect(m_picBtn, &DToolButton::clicked, [this]() {
        m_picBtn->setIcon(QIcon::fromTheme("ddc_picture tools_disable"));
        m_picBtn->setChecked(true);
        clearOtherSelections(m_picBtn);
        isCutMode();
        emit importPic();
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
    });

    connect(m_rectBtn, &DToolButton::clicked, [this]() {
        m_rectBtn->setIcon(QIcon::fromTheme("ddc_rectangle tool_active"));
        m_rectBtn->setChecked(true);
        clearOtherSelections(m_rectBtn);
        isCutMode();
        emit setCurrentDrawTool(rectangle);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(rectangle);
    });

    connect(m_roundBtn, &DToolButton::clicked, [this]() {
        m_roundBtn->setIcon(QIcon::fromTheme("ddc_round tool_active"));
        m_roundBtn->setChecked(true);
        clearOtherSelections(m_roundBtn);
        isCutMode();
        emit setCurrentDrawTool(ellipse);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(ellipse);
    });

    connect(m_triangleBtn, &DToolButton::clicked, [this]() {
        m_triangleBtn->setIcon(QIcon::fromTheme("ddc_triangle tool_active"));
        m_triangleBtn->setChecked(true);
        clearOtherSelections(m_triangleBtn);
        isCutMode();
        emit setCurrentDrawTool(triangle);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(triangle);
    });

    connect(m_starBtn, &DToolButton::clicked, [this]() {
        m_starBtn->setIcon(QIcon::fromTheme("ddc_star tool_active"));
        m_starBtn->setChecked(true);
        clearOtherSelections(m_starBtn);
        isCutMode();
        emit setCurrentDrawTool(polygonalStar);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(polygonalStar);
    });

    connect(m_polygonBtn, &DToolButton::clicked, [this]() {
        m_polygonBtn->setIcon(QIcon::fromTheme("ddc_hexagon tool_active"));
        m_polygonBtn->setChecked(true);
        clearOtherSelections(m_polygonBtn);
        isCutMode();
        emit setCurrentDrawTool(polygon);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(polygon);
    });

    connect(m_lineBtn, &DToolButton::clicked, [this]() {
        m_lineBtn->setIcon(QIcon::fromTheme("ddc_line tool_active"));
        m_lineBtn->setChecked(true);
        clearOtherSelections(m_lineBtn);
        isCutMode();
        emit setCurrentDrawTool(line);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(line);
    });

    connect(m_penBtn, &DToolButton::clicked, [this]() {
        m_penBtn->setIcon(QIcon::fromTheme("ddc_brush tool_active"));
        m_penBtn->setChecked(true);
        clearOtherSelections(m_penBtn);
        isCutMode();
        emit setCurrentDrawTool(pen);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(pen);
    });

    connect(m_textBtn, &DToolButton::clicked, [this]() {
        m_textBtn->setIcon(QIcon::fromTheme("ddc_text tool_active"));
        m_textBtn->setChecked(true);
        clearOtherSelections(m_textBtn);
        isCutMode();
        emit setCurrentDrawTool(text);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(text);
    });

    connect(m_blurBtn, &DToolButton::clicked, [this]() {
        m_blurBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_active"));
        m_blurBtn->setChecked(true);
        clearOtherSelections(m_blurBtn);
        isCutMode();
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(blur);
    });

    connect(m_cutBtn, &DToolButton::clicked, [this]() {
        m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_active"));
        m_cutBtn->setChecked(true);
        clearOtherSelections(m_cutBtn);
        CManageViewSigleton::GetInstance()->getCurView()->disableCutShortcut(false);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(cut);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutType(ECutType::cut_free);
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

void CLeftToolBar::slotShortCutSelect(bool showToolMidWidget)
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    if (showToolMidWidget) {
        emit m_selectBtn->clicked();
    } else {
        foreach (DToolButton *button, m_actionButtons) {
            if (button->isChecked()) {
                if (button == m_selectBtn) {
                    button->setIcon(QIcon::fromTheme("ddc_choose tools_normal"));
                    button->setChecked(false);
                } else if (button == m_picBtn) {
                    button->setIcon(QIcon::fromTheme("ddc_picture tools_normal"));
                    button->setChecked(false);
                } else if (button == m_rectBtn) {
                    button->setIcon(QIcon::fromTheme("ddc_rectangle tool_normal"));
                    button->setChecked(false);
                } else if (button == m_roundBtn) {
                    button->setIcon(QIcon::fromTheme("ddc_round tool_normal"));
                    button->setChecked(false);
                } else if (button == m_triangleBtn) {
                    m_triangleBtn->setIcon(QIcon::fromTheme("ddc_triangle tool_normal"));
                    m_triangleBtn->setChecked(false);
                } else if (button == m_starBtn) {
                    m_starBtn->setIcon(QIcon::fromTheme("ddc_star tool_normal"));
                    m_starBtn->setChecked(false);
                } else if (button == m_polygonBtn) {
                    m_polygonBtn->setIcon(QIcon::fromTheme("ddc_hexagon tool_normal"));
                    m_polygonBtn->setChecked(false);
                } else if (button == m_lineBtn) {
                    m_lineBtn->setIcon(QIcon::fromTheme("ddc_line tool_normal"));
                    m_lineBtn->setChecked(false);
                } else if (button == m_penBtn) {
                    m_penBtn->setIcon(QIcon::fromTheme("ddc_brush tool_normal"));
                    m_penBtn->setChecked(false);
                } else if (button == m_textBtn) {
                    m_textBtn->setIcon(QIcon::fromTheme("ddc_text tool_normal"));
                    m_textBtn->setChecked(false);
                } else if (button == m_blurBtn) {
                    m_blurBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_normal"));
                    m_blurBtn->setChecked(false);
                } else if (button == m_cutBtn) {
                    m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_normal"));
                    m_cutBtn->setChecked(false);
                }
            }
        }
    }
}

void CLeftToolBar::slotShortCutPictrue()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_picBtn->setChecked(true);
    emit m_picBtn->clicked();
}

void CLeftToolBar::slotShortCutRect()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_rectBtn->setChecked(true);
    emit m_rectBtn->clicked();
}

void CLeftToolBar::slotShortCutRound()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_roundBtn->setChecked(true);
    emit m_roundBtn->clicked();
}

void CLeftToolBar::slotShortCutTriangle()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_triangleBtn->setChecked(true);
    emit m_triangleBtn->clicked();
}

void CLeftToolBar::slotShortCutPolygonalStar()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_starBtn->setChecked(true);
    emit m_starBtn->clicked();
}

void CLeftToolBar::slotShortCutPolygon()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_polygonBtn->setChecked(true);
    emit m_polygonBtn->clicked();
}

void CLeftToolBar::slotShortCutLine()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_lineBtn->setChecked(true);
    emit m_lineBtn->clicked();
}

void CLeftToolBar::slotShortCutPen()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_penBtn->setChecked(true);
    emit m_penBtn->clicked();
}

void CLeftToolBar::slotShortCutText()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_textBtn->setChecked(true);
    emit m_textBtn->clicked();
}

void CLeftToolBar::slotShortCutBlur()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_blurBtn->setChecked(true);
    emit m_blurBtn->clicked();
}

void CLeftToolBar::slotShortCutCut()
{
    dApp->setApplicationCursor(Qt::ArrowCursor);
    m_cutBtn->setChecked(true);
    emit m_cutBtn->clicked();
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
    //connect(m_selectAction, SIGNAL(triggered()), this, SLOT(slotShortCutSelect()));
    connect(m_selectAction, &QAction::triggered, this, [ = ]() {
        CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
        bool selectionsIsEmpty = (pView->scene() == nullptr ? true : pView->scene()->selectedItems().isEmpty());

        bool clearProperWidget = (selectionsIsEmpty || pView->getDrawParam()->getCurrentDrawToolMode() == cut);
        this->doSelectToolChanged(clearProperWidget);
    });
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
    //在点击其他工具时，如果当前是裁切模式则退出裁切
    CGraphicsView *pCurView = CManageViewSigleton::GetInstance()->getCurView();
    if (pCurView != nullptr && cut == pCurView->getDrawParam()->getCurrentDrawToolMode()) {
        if (nullptr != CManageViewSigleton::GetInstance()->getCurView()->scene()) {
            auto curScene = static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene());
            curScene->quitCutMode();
        }
    }
}

void CLeftToolBar::doSelectToolChanged(bool showProperWidget)
{
    m_selectBtn->setIcon(QIcon::fromTheme("ddc_choose tools_active"));
    m_selectBtn->setChecked(true);
    clearOtherSelections(m_selectBtn);
    isCutMode();
    emit setCurrentDrawTool(selection, showProperWidget);
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
}

