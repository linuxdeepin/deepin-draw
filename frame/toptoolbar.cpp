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
#include "toptoolbar.h"
#include "application.h"
#include "commonshapewidget.h"
#include "polygonalstarattributewidget.h"
#include "polygonattributewidget.h"
#include "ccutwidget.h"
#include "linewidget.h"
#include "cpicturewidget.h"
#include "textwidget.h"
#include "cpenwidget.h"
#include "blurwidget.h"
#include "ctitlewidget.h"
#include "widgets/arrowrectangle.h"
#include "widgets/colorpanel.h"
#include "widgets/dialog/drawdialog.h"
#include "widgets/dialog/savedialog.h"
#include "utils/tempfile.h"
#include "drawshape/cdrawparamsigleton.h"

#include <DComboBox>
#include <DApplication>
#include <QHBoxLayout>
#include <QString>
#include <QStandardItemModel>
#include <DApplicationHelper>

#include <DLineEdit>

TopToolbar::TopToolbar(DWidget *parent)
    : DFrame(parent)
{
    initUI();
    initConnection();
}

TopToolbar::~TopToolbar()
{

}

void TopToolbar::initUI()
{
    initComboBox();
    initStackWidget();
    initMenu();

    DLabel *logoLable = new DLabel(this);
    logoLable->setPixmap(QPixmap(":/theme/common/images/logo.svg"));
    logoLable->setFixedSize(QSize(32, 32));

    QHBoxLayout *hLayout = new QHBoxLayout (this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addSpacing(13);
    hLayout->addWidget(logoLable);
    hLayout->addSpacing(20);
    hLayout->addWidget(m_scaleComboBox);
//    hLayout->addSpacing(20);
    hLayout->addWidget(m_stackWidget, 0, Qt::AlignHCenter);
    hLayout->addSpacing(33);
    hLayout->setContentsMargins(0, 0, 0, 0);
//    hLayout->addStretch();
    setLayout(hLayout);

//    m_stackWidget->setStyleSheet("background-color: blue;");
//    setStyleSheet("background-color: rgb(255, 0, 0);");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void TopToolbar::initComboBox()
{

    m_scaleComboBox = new DPushButton("100%");
    DMenu *scaleMenu = new DMenu();

    QAction *scale200 = scaleMenu->addAction("200%");
    QAction *scale100 = scaleMenu->addAction("100%");
    QAction *scale75 = scaleMenu->addAction("75%");
    QAction *scale50 = scaleMenu->addAction("50%");
    QAction *scale25 = scaleMenu->addAction("25%");


    connect(scale200, &QAction::triggered, this, [ = ]() {
        m_scaleComboBox->setText("200%");
        slotZoom("200%");
    });
    connect(scale100, &QAction::triggered, this, [ = ]() {
        m_scaleComboBox->setText("100%");
        slotZoom("100%");
    });
    connect(scale75, &QAction::triggered, this, [ = ]() {
        m_scaleComboBox->setText("75%");
        slotZoom("75%");
    });
    connect(scale50, &QAction::triggered, this, [ = ]() {
        m_scaleComboBox->setText("50%");
        slotZoom("50%");
    });
    connect(scale25, &QAction::triggered, this, [ = ]() {
        m_scaleComboBox->setText("25%");
        slotZoom("25%");
    });

    m_scaleComboBox->setMenu(scaleMenu);
    m_scaleComboBox->setFixedWidth(70);

    //设置字体大小
    QFont ft;
    ft.setPixelSize(12);
    m_scaleComboBox->setFont(ft);
    m_scaleComboBox->setFlat(true);


}

void TopToolbar::initStackWidget()
{
    m_stackWidget = new DStackedWidget(this);
    m_titleWidget = new CTitleWidget(this);
    m_stackWidget->addWidget(m_titleWidget);

    //colorPanel.
    m_colorPanel = new ColorPanel(this);
    qApp->setProperty("_d_isDxcb", false);
    m_colorARect = new ArrowRectangle(DArrowRectangle::ArrowTop, this->parentWidget());
    qApp->setProperty("_d_isDxcb", true);
    m_colorARect->setWindowFlags(Qt::Widget);
    m_colorARect->setAttribute(Qt::WA_TranslucentBackground, false);
    m_colorARect->setArrowWidth(18);
    m_colorARect->setArrowHeight(10);
    m_colorARect->setContent(m_colorPanel);
    m_colorARect->hide();


    //cut
    m_picWidget = new CPictureWidget(this);
    m_stackWidget->addWidget(m_picWidget);

    //rectangle, triangle,oval
    m_commonShapeWidget = new CommonshapeWidget(this);
    m_stackWidget->addWidget(m_commonShapeWidget);

    ///polygonalStar
    m_polygonalStarWidget = new PolygonalStarAttributeWidget(this);
    m_stackWidget->addWidget(m_polygonalStarWidget);


    ///polygonalStar
    m_PolygonWidget = new PolygonAttributeWidget(this);
    m_stackWidget->addWidget(m_PolygonWidget);

    //draw line.
    m_drawLineWidget = new LineWidget(this);
    m_stackWidget->addWidget(m_drawLineWidget);

    //draw pen
    m_penWidget = new CPenWidget(this);
    m_stackWidget->addWidget(m_penWidget);


    //draw text.
    m_drawTextWidget = new TextWidget(this);
    m_stackWidget->addWidget(m_drawTextWidget);

    //draw blur widget.
    m_drawBlurWidget = new BlurWidget(this);
    m_stackWidget->addWidget(m_drawBlurWidget);

    m_cutWidget = new CCutWidget(this);
    m_stackWidget->addWidget(m_cutWidget);

    //process  artboard's size.
//    m_adjustsizeWidget = new AdjustsizeWidget(this);
//    m_stackWidget->addWidget(m_adjustsizeWidget);

    m_stackWidget->setCurrentWidget(m_titleWidget);
}

void TopToolbar::initMenu()
{

    m_mainMenu = new DMenu();
    //m_mainMenu->setFixedWidth(120);
    //m_mainMenu->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    //m_mainMenu->setWindowFlags(Qt::FramelessWindowHint);
    //m_mainMenu->setBackgroundColor(QColor(248, 168, 0));

    QAction *newConstructAc = m_mainMenu->addAction(tr("New"));
    QAction *importAc = m_mainMenu->addAction(tr("Open"));
    m_mainMenu->addSeparator();
    QAction *exportAc = m_mainMenu->addAction(tr("Export"));
    QAction *saveAc = m_mainMenu->addAction(tr("Save"));
    QAction *saveAsAc = m_mainMenu->addAction(tr("Save as"));
    QAction *printAc = m_mainMenu->addAction(tr("Print"));
    m_mainMenu->addSeparator();
//    DMenu *themMenu = new DMenu(tr("Theme"));
//    QAction *themeLightAc = themMenu->addAction(tr("Light theme"));
//    QAction *themeDarkAc = themMenu->addAction(tr("Dark theme"));
//    QAction *themeSystemAc = themMenu->addAction(tr("System theme"));
//    m_mainMenu->addMenu(themMenu);

    // QAction *helpAc = m_mainMenu->addAction(tr("Help"));


//    Q_UNUSED(themeAc);
//   Q_UNUSED(helpAc);
    QIcon t_icon = QIcon::fromTheme("deepin-draw");
    //dApp->setProductIcon(QIcon(QPixmap(":/theme/common/images/deepin-draw-96.svg")));
    dApp->setProductIcon(t_icon);
    dApp->setApplicationDescription(tr("Deepin Draw is a lightweight drawing tool."
                                       " You can freely draw on the layer or simply edit images. "));
    dApp->setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin-draw/");

    connect(importAc, &QAction::triggered, this, &TopToolbar::signalImport);
//    connect(dApp, &Application::popupConfirmDialog, this, &TopToolbar::showDrawDialog);
    connect(saveAc, &QAction::triggered, this, &TopToolbar::signalSaveToDDF);
    connect(saveAsAc, &QAction::triggered, this, &TopToolbar::signalSaveAs);
    connect(printAc, &QAction::triggered, this, &TopToolbar::signalPrint);
    connect(exportAc, &QAction::triggered, this, &TopToolbar::signalShowExportDialog);
    connect(newConstructAc, &QAction::triggered, this, &TopToolbar::signalNew);

    connect(m_mainMenu, &DMenu::triggered, this, &TopToolbar::slotIsCutMode);
}


void TopToolbar::showSaveDialog()
{
    SaveDialog *sd = new SaveDialog(TempFile::instance()->savedImage(), this);
    sd->showInCenter(window());
}

void TopToolbar::changeTopButtonsTheme()
{
    m_picWidget->changeButtonTheme();
    m_commonShapeWidget->changeButtonTheme();
    m_polygonalStarWidget->changeButtonTheme();
    m_PolygonWidget->changeButtonTheme();
    m_drawLineWidget->changeButtonTheme();
    m_penWidget->changeButtonTheme();
    m_drawBlurWidget->changeButtonTheme();
    m_drawTextWidget->updateTheme();
}


void TopToolbar::updateMiddleWidget(int type)
{
    switch (type) {
    case::importPicture:
        m_stackWidget->setCurrentWidget(m_picWidget);
        break;
    case::rectangle:
    case::ellipse:
    case::triangle:
        m_commonShapeWidget->updateCommonShapWidget();
        m_stackWidget->setCurrentWidget(m_commonShapeWidget);
        break;
    case::polygonalStar:
        m_polygonalStarWidget->updatePolygonalStarWidget();
        m_stackWidget->setCurrentWidget(m_polygonalStarWidget);
        break;
    case::polygon:
        m_PolygonWidget->updatePolygonWidget();
        m_stackWidget->setCurrentWidget(m_PolygonWidget);
        break;
    case::line:
        m_drawLineWidget->updateLineWidget();
        m_stackWidget->setCurrentWidget(m_drawLineWidget);
        break;
    case::pen:
        m_penWidget->updatePenWidget();
        m_stackWidget->setCurrentWidget(m_penWidget);
        break;
    case::text:
        m_drawTextWidget->updateTextWidget();
        m_stackWidget->setCurrentWidget(m_drawTextWidget);
        break;
    case::blur:
        m_drawBlurWidget->updateBlurWidget();
        m_stackWidget->setCurrentWidget(m_drawBlurWidget);
        break;
    case::cut:
        m_stackWidget->setCurrentWidget(m_cutWidget);
        break;
    default:
        break;
    }
}

void TopToolbar::showColorfulPanel(DrawStatus drawstatus, QPoint pos, bool visible)
{
    Q_UNUSED(pos);

    m_colorPanel->updateColorPanel(drawstatus);
    m_colorARect->raise();

    if (visible) {

        QPoint startPos = QPoint(0, 0);

        m_colorARect->show(pos.x() - mapToGlobal(startPos).x(),
                           pos.y() - mapToGlobal(startPos).y());
    } else
        m_colorARect->hide();
}


void TopToolbar::updateColorPanelVisible(QPoint pos)
{
    QRect colorPanelGeom = m_colorARect->geometry();
    if (!colorPanelGeom.contains(pos)) {
        m_colorARect->hide();
    }
}

void TopToolbar::slotChangeAttributeFromScene(bool flag, int primitiveType)
{
    if (flag) {
        EDrawToolMode toolType = EDrawToolMode::selection;
        switch (primitiveType) {
        case::EGraphicUserType::RectType:
            toolType = EDrawToolMode::rectangle;
            break;
        case::EGraphicUserType::EllipseType:
            toolType = EDrawToolMode::ellipse;
            break;
        case::EGraphicUserType::TriangleType:
            toolType = EDrawToolMode::triangle;
            break;
        case::EGraphicUserType::PolygonalStarType:
            toolType = EDrawToolMode::polygonalStar;
            break;
        case::EGraphicUserType::PolygonType:
            toolType = EDrawToolMode::polygon;
            break;
        case::EGraphicUserType::LineType:
            toolType = EDrawToolMode::line;
            break;
        case::EGraphicUserType::PenType:
            toolType = EDrawToolMode::pen;
            break;
        case::EGraphicUserType::TextType:
            toolType = EDrawToolMode::text;
            break;
        case::EGraphicUserType::PictureType:
            toolType = EDrawToolMode::importPicture;
            break;
        case::EGraphicUserType::BlurType:
            toolType = EDrawToolMode::blur;
            break;
//        case::EGraphicUserType::CutType:
//            toolType = EDrawToolMode::cut;
//            break;
        }
        updateMiddleWidget(toolType);
    }
}

void TopToolbar::slotZoom(const QString &scale)
{
    qreal fScale = 0.0;
    if (scale == "200%") {
        fScale = 2;
    } else if (scale == "100%") {
        fScale = 1;
    } else if (scale == "75%") {
        fScale = 0.75;
    } else if (scale == "50%") {
        fScale = 0.5;
    } else if (scale == "25%") {
        fScale = 0.25;
    } else {
        fScale = 1;
    }

    emit signalZoom(fScale);
}

void TopToolbar::slotSetScale(const qreal scale)
{
    QString strScale = QString::number(int(scale * 100)) + "%";

    m_scaleComboBox->setText(strScale);
}

void TopToolbar::slotSetCutSize()
{
    m_cutWidget->updateCutSize();
}

void TopToolbar::slotSetTextFont()
{
    m_drawTextWidget->updateTextWidget();
}

void TopToolbar::slotIsCutMode(QAction *action)
{
    Q_UNUSED(action)

//    if (cut == CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode()) {
//        emit signalQuitCutModeFromTopBarMenu();
//    }
}


DMenu *TopToolbar::mainMenu()
{
    return m_mainMenu;
}

void TopToolbar::resizeEvent(QResizeEvent *event)
{
    this->updateGeometry();
    m_colorARect->hide();
    QWidget::resizeEvent(event);
}

void TopToolbar::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    qApp->setOverrideCursor(Qt::ArrowCursor);
    DFrame::enterEvent(event);
}

void TopToolbar::initConnection()
{
    //colorPanel.
    connect(m_colorPanel, &ColorPanel::updateHeight, this, [ = ] {m_colorARect->setContent(m_colorPanel);});
    connect(m_colorPanel, &ColorPanel::signalChangeFinished, this, [ = ] {m_colorARect->hide();});
    connect(m_colorPanel, &ColorPanel::signalColorChanged, this, &TopToolbar::signalAttributeChanged);

    /////传递图片的旋转和翻转信号
    connect(m_picWidget, &CPictureWidget::signalBtnClick, this, &TopToolbar::signalPassPictureOperation);

    //rectangle, triangle,ellipse
    connect(m_commonShapeWidget, &CommonshapeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_commonShapeWidget, &CommonshapeWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_commonShapeWidget, &CommonshapeWidget::updateCommonShapWidget);
    connect(m_commonShapeWidget, &CommonshapeWidget::signalCommonShapeChanged, this, &TopToolbar::signalAttributeChanged);
    ///polygonalStar
    connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_polygonalStarWidget, &PolygonalStarAttributeWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_polygonalStarWidget, &PolygonalStarAttributeWidget::updatePolygonalStarWidget);
    connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::signalPolygonalStarAttributeChanged, this, &TopToolbar::signalAttributeChanged);
    ///polygon
    connect(m_PolygonWidget, &PolygonAttributeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_PolygonWidget, &PolygonAttributeWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_PolygonWidget, &PolygonAttributeWidget::updatePolygonWidget);
    connect(m_PolygonWidget, &PolygonAttributeWidget::signalPolygonAttributeChanged, this, &TopToolbar::signalAttributeChanged);
    //draw line.
    connect(m_drawLineWidget, &LineWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_drawLineWidget, &LineWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_drawLineWidget, &LineWidget::updateLineWidget);
    connect(m_drawLineWidget, &LineWidget::signalLineAttributeChanged, this, &TopToolbar::signalAttributeChanged);
    //draw pen.
    connect(m_penWidget, &CPenWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_penWidget, &CPenWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_penWidget, &CPenWidget::updatePenWidget);
    connect(m_penWidget, &CPenWidget::signalPenAttributeChanged, this, &TopToolbar::signalAttributeChanged);
    //draw text.
    connect(m_drawTextWidget, &TextWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_drawTextWidget, &TextWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_drawTextWidget, &TextWidget::updateTextColor);
    connect(m_drawTextWidget, &TextWidget::signalTextAttributeChanged, this, &TopToolbar::signalAttributeChanged);
    connect(m_drawTextWidget, &TextWidget::signalTextFontFamilyChanged, this, &TopToolbar::signalTextFontFamilyChanged);
    connect(m_drawTextWidget, &TextWidget::signalTextFontSizeChanged, this, &TopToolbar::signalTextFontSizeChanged);
    //draw blur widget.

    connect(m_drawBlurWidget, &BlurWidget::signalBlurAttributeChanged, this, &TopToolbar::signalAttributeChanged);

    //cut
    connect(m_cutWidget, &CCutWidget::signalCutAttributeChanged, this, &TopToolbar::signalAttributeChanged);

    connect(TempFile::instance(), &TempFile::saveDialogPopup, this, &TopToolbar::showSaveDialog);


}

