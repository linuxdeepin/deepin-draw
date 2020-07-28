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
//#include "widgets/colorpanel.h"
#include "widgets/dialog/drawdialog.h"
#include "drawshape/cdrawparamsigleton.h"
#include "widgets/csvglabel.h"
#include "widgets/cmenu.h"
#include "widgets/dzoommenucombobox.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
#include "citemattriwidget.h"

#include <DComboBox>
#include <DApplication>
#include <QHBoxLayout>
#include <QString>
#include <QStandardItemModel>
#include <DApplicationHelper>
#include <QWidgetAction>
#include <QTimer>

#include <DLineEdit>

const int Text_Size = 14;

TopToolbar::TopToolbar(DWidget *parent)
    : DFrame(parent)
{
    QMetaObject::invokeMethod(this, [ = ]() {
        initTopMenuUI();
        initConnection();
        changeTopButtonsTheme();
    }, Qt::QueuedConnection);


    m_propertys.clear();
    initUI();
}

TopToolbar::~TopToolbar()
{

}

void TopToolbar::initUI()
{
    ft.setPixelSize(Text_Size);

    // 初始化缩放菜单
    initComboBox();
    initStackWidget();
    initMenu();

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    hLayout->addWidget(m_zoomMenuComboBox);

    //m_stackWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //hLayout->addWidget(m_stackWidget);

    m_stackWidget->hide();
    m_pAtrriWidget = new CComAttrWidget(this);
    m_pAtrriWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hLayout->addWidget(m_pAtrriWidget);

    hLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(hLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void TopToolbar::initComboBox()
{
    m_zoomMenuComboBox = new DZoomMenuComboBox(this);
    m_zoomMenuComboBox->setFont(ft);
    m_zoomMenuComboBox->setMenuFlat(false);
    m_zoomMenuComboBox->setFixedWidth(162);
    m_zoomMenuComboBox->addItem("200%");
    m_zoomMenuComboBox->addItem("100%");
    m_zoomMenuComboBox->addItem("75%");
    m_zoomMenuComboBox->addItem("50%");
    m_zoomMenuComboBox->addItem("25%");
    connect(m_zoomMenuComboBox, &DZoomMenuComboBox::signalCurrentTextChanged, this, [ = ](QString item) {
        slotZoom(item);
    });
    // 初始化大小为 100%
    m_zoomMenuComboBox->setCurrentText("100%");

    // 放大缩小范围10%-2000% ，点击放大缩小，如区间在200%-2000%，则每次点+/-100%；如区间在10%-199%，则每次点击+/-10%
    // 左侧按钮点击信号 (-)
    connect(m_zoomMenuComboBox, &DZoomMenuComboBox::signalLeftBtnClicked, this, [ = ]() {
        qreal current_scale = CManageViewSigleton::GetInstance()->getCurView()->getScale();
        if (current_scale >= 2.0 && current_scale <= 20.0) {
            current_scale -= 1.0;
        } else if (current_scale >= 0.1 && current_scale <= 1.999) {
            current_scale -= 0.1;
        }
        if (current_scale <= 0.1) {
            current_scale = 0.1;
        }
        slotZoom(current_scale);
    });
    // 右侧按钮点击信号 (+)
    connect(m_zoomMenuComboBox, &DZoomMenuComboBox::signalRightBtnClicked, this, [ = ]() {
        qreal current_scale = CManageViewSigleton::GetInstance()->getCurView()->getScale();
        if (current_scale >= 2.0 && current_scale <= 20.0) {
            current_scale += 1.0;
        } else if (current_scale >= 0.1 && current_scale <= 1.999) {
            current_scale += 0.1;
        }
        if (current_scale >= 20.0) {
            current_scale = 20.0;
        }
        slotZoom(current_scale);
    });
}

void TopToolbar::initStackWidget()
{
    m_stackWidget = new DStackedWidget(this);

    //colorPanel.
    //m_colorPanel = new ColorPanel(this);
    //    qApp->setProperty("_d_isDxcb", false);
    //    m_colorARect = new ArrowRectangle(DArrowRectangle::ArrowTop, this);
    //    qApp->setProperty("_d_isDxcb", true);
    //    m_colorARect->setWindowFlags(Qt::Popup /*Widget*/);
    //    m_colorARect->setAttribute(Qt::WA_TranslucentBackground, true /*false*/);
    //    m_colorARect->setArrowWidth(18);
    //    m_colorARect->setArrowHeight(10);
    //    m_colorARect->setContent(m_colorPanel);
    //    m_colorARect->hide();

    //select
    m_titleWidget = new CTitleWidget(this);
    m_stackWidget->addWidget(m_titleWidget);

    //rectangle, triangle,oval
    m_commonShapeWidget = new CommonshapeWidget(this);
    m_stackWidget->addWidget(m_commonShapeWidget);

    m_stackWidget->setCurrentWidget(m_titleWidget);
}

void TopToolbar::initTopMenuUI()
{
    //picture
    m_picWidget = new CPictureWidget(this);
    m_stackWidget->addWidget(m_picWidget);

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
}

void TopToolbar::initMenu()
{
    m_mainMenu = new CMenu(this);
    m_mainMenu->setFixedWidth(162);

    m_newAction = new QAction(tr("New"), this);
    m_newAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    m_mainMenu->addAction(m_newAction);
    this->addAction(m_newAction);

    QAction *importAc = new QAction(tr("Open"), this);
    importAc->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    m_mainMenu->addAction(importAc);
    this->addAction(importAc);
    m_mainMenu->addSeparator();

    QAction *exportAc = new QAction(tr("Export"), this);
    exportAc->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_mainMenu->addAction(exportAc);
    this->addAction(exportAc);

    m_saveAction = new QAction(tr("Save"), this);
    m_saveAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    m_mainMenu->addAction(m_saveAction);
    this->addAction(m_saveAction);

    QAction *saveAsAc = new QAction(tr("Save as"), this);
    saveAsAc->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_mainMenu->addAction(saveAsAc);
    this->addAction(saveAsAc);

    QAction *printAc = new QAction(tr("Print"), this);
    printAc->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    m_mainMenu->addAction(printAc);
    this->addAction(printAc);
    m_mainMenu->addSeparator();

    QIcon t_icon;
//    QPixmap pixmap = QIcon::fromTheme("deepin-draw").pixmap(QSize(64, 64) );
//    t_icon.addPixmap(pixmap);


    t_icon = QIcon::fromTheme("deepin-draw");
    dApp->setProductIcon(t_icon);
    dApp->setProductName(tr("Draw"));

    //画板是一款轻量级的绘图工具，支持在画板上自由绘图和简单的图片编辑。
    dApp->setApplicationDescription(tr("Draw is a lightweight drawing tool for users to freely draw and simply edit images. "));
    dApp->setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin-draw/");

    connect(importAc, &QAction::triggered, this, &TopToolbar::slotOnImportAction);
//    connect(dApp, &Application::popupConfirmDialog, this, &TopToolbar::showDrawDialog);
    connect(m_saveAction, &QAction::triggered, this, &TopToolbar::slotOnSaveAction);
    connect(saveAsAc, &QAction::triggered, this, &TopToolbar::slotOnSaveAsAction);
    connect(printAc, &QAction::triggered, this, &TopToolbar::signalPrint);
    connect(exportAc, &QAction::triggered, this, &TopToolbar::signalShowExportDialog);
    connect(m_newAction, &QAction::triggered, this, &TopToolbar::slotOnNewConstructAction);

    connect(m_mainMenu, &DMenu::triggered, this, &TopToolbar::slotIsCutMode);
    connect(m_mainMenu, &DMenu::aboutToShow, this, &TopToolbar::slotMenuShow);
}

void TopToolbar::changeTopButtonsTheme()
{
    m_commonShapeWidget->changeButtonTheme();
    m_polygonalStarWidget->changeButtonTheme();
    m_PolygonWidget->changeButtonTheme();
    m_drawLineWidget->changeButtonTheme();
    m_penWidget->changeButtonTheme();
    m_drawBlurWidget->changeButtonTheme();
    m_drawTextWidget->updateTheme();
    //m_colorPanel->changeButtonTheme();
    m_cutWidget->changeButtonTheme();

    //m_colorPanel->changeButtonTheme();

    if (m_picWidget) {
        m_picWidget->changeButtonTheme();
    }
    if (m_polygonalStarWidget) {
        m_polygonalStarWidget->changeButtonTheme();
    }
    if (m_PolygonWidget) {
        m_PolygonWidget->changeButtonTheme();
    }
    if (m_drawLineWidget) {
        m_drawLineWidget->changeButtonTheme();
    }
    if (m_penWidget) {
        m_penWidget->changeButtonTheme();
    }
    if (m_drawBlurWidget) {
        m_drawBlurWidget->changeButtonTheme();
    }
    if (m_drawTextWidget) {
        m_drawTextWidget->updateTheme();
    }
    if (m_cutWidget) {
        m_cutWidget->changeButtonTheme();
    }
}

void TopToolbar::updateMiddleWidget(int type, bool showSelfPropreWidget)
{
    switch (type) {
    case::selection: {
        if (showSelfPropreWidget) {
            m_commonShapeWidget->setRectXRediusSpinboxVisible(false);
            m_titleWidget->updateTitleWidget();

            //先隐藏后显示(底层有一个显示BUG，这样规避一下)
            m_stackWidget->setVisible(false);
            m_stackWidget->setCurrentWidget(m_titleWidget);
            m_stackWidget->setVisible(true);
        }
        break;
    }
    case::importPicture:
        m_commonShapeWidget->setRectXRediusSpinboxVisible(false);
        m_stackWidget->setCurrentWidget(m_picWidget);
        break;
    case::rectangle:
        m_commonShapeWidget->setRectXRedius(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRectXRedius());
        m_commonShapeWidget->updateCommonShapWidget();
        m_commonShapeWidget->setRectXRediusSpinboxVisible(true);
        m_stackWidget->setCurrentWidget(m_commonShapeWidget);
        break;
    case::ellipse:
    case::triangle:
        m_commonShapeWidget->setRectXRediusSpinboxVisible(false);
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
    case::text: {
        m_drawTextWidget->updateTextWidget();
        m_stackWidget->setCurrentWidget(m_drawTextWidget);
        break;
    }
    case::blur:
        m_drawBlurWidget->updateBlurWidget();
        m_stackWidget->setCurrentWidget(m_drawBlurWidget);
        break;
    case::cut:
        m_cutWidget->updateButtonStatus();
        m_stackWidget->setCurrentWidget(m_cutWidget);
        break;
    default:
        break;
    }
    m_stackWidget->currentWidget()->setVisible(true);
}

void TopToolbar::slotChangeAttributeFromScene(bool flag, int primitiveType)
{
    if (primitiveType == QGraphicsItem::UserType) {
        return;
    }
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

    QString scale_num_str = scale;
    scale_num_str = scale_num_str.replace("%", "");

    int scale_num = 1;
    bool flag = false;

    scale_num = scale_num_str.toInt(&flag);

    if (flag) {
        fScale = scale_num / 100.0;
    } else {
        fScale = 1.0;
    }
    slotZoom(fScale);
}

void TopToolbar::slotZoom(const qreal &scale)
{
    emit signalZoom(scale);

    // 更新当前缩放的比例
    slotSetScale(scale);
}

void TopToolbar::slotSetScale(const qreal scale)
{
    QString strScale = QString::number(scale * 100) + "%";
    m_zoomMenuComboBox->setMenuButtonTextAndIcon(strScale, QIcon());
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
    if (cut == CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode()) {
//        emit signalQuitCutModeFromTopBarMenu();
    }
}

void TopToolbar::slotOnImportAction()
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
    emit signalImport();
}

void TopToolbar::slotOnNewConstructAction()
{

    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::NewDrawingBoard);
    emit signalNew();
}

void TopToolbar::slotOnSaveAction()
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::SaveAction);
    emit signalSaveToDDF();
}

void TopToolbar::slotOnSaveAsAction()
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::SaveAction);
    emit signalSaveAs();
}

void TopToolbar::slotMenuShow()
{
    //slotHideColorPanel();
    //    m_newAction->setEnabled(CManageViewSigleton::GetInstance()->getCurView()->getModify());
}

DMenu *TopToolbar::mainMenu()
{
    return m_mainMenu;
}

CComAttrWidget *TopToolbar::attributWidget()
{
    return m_pAtrriWidget;
}

//void TopToolbar::slotHideColorPanel()
//{
//    if (!m_colorARect->isHidden()) {
//        m_colorARect->hide();
//    }
//}

void TopToolbar::slotRectRediusChanged(int value)
{
    qDebug() << "CTopToolbar::slotRectRediusChanged value = " << value;
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setRectXRedius(value);
}

void TopToolbar::updateMiddleWidgetMult(EGraphicUserType mode,
                                        QMap<EDrawProperty, QVariant> propertys,
                                        bool write2Cache)
{
    if (propertys.size() > 0) {
        m_propertys = propertys;
        m_stackWidget->currentWidget()->setVisible(true);
    } else {
        if (m_stackWidget->currentWidget() != m_titleWidget &&
                m_cutWidget != m_stackWidget->currentWidget()) {
            m_stackWidget->currentWidget()->setVisible(false);
        }
    }
    switch (mode) {
    case::RectType://矩形
    case::EllipseType://圆形
    case::TriangleType://三角形
        // 25039 解决设置图元alpha值后会不断刷新缓存，因此需要一个标记进行提示是否需要写入缓存
        m_commonShapeWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_commonShapeWidget);
        break;
    case::PolygonalStarType://多角星
        m_polygonalStarWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_polygonalStarWidget);
        break;
    case::PolygonType://多边形
        m_PolygonWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_PolygonWidget);
        break;
    case::LineType://线
        m_drawLineWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_drawLineWidget);
        break;
    case::PenType://画笔
        m_penWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_penWidget);
        break;
    case::TextType://文本
        m_drawTextWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_drawTextWidget);
        break;
    case::BlurType://模糊
        m_drawBlurWidget->updateMultCommonShapWidget(propertys, write2Cache);
        m_stackWidget->setCurrentWidget(m_drawBlurWidget);
        break;
    default:
        break;
    }
}

void TopToolbar::slotIsAllPictureItem(bool isEnable, bool single)
{
    m_stackWidget->setCurrentWidget(m_picWidget);
    m_picWidget->setAdjustmentIsEnable(isEnable);
    m_picWidget->setRotationEnable(single);
    m_stackWidget->currentWidget()->setVisible(true);
}

void TopToolbar::slotScenceViewChanged(QString viewname)
{
    m_titleWidget->setTittleText(viewname);
}

void TopToolbar::resizeEvent(QResizeEvent *event)
{
    this->updateGeometry();
    //m_colorARect->hide();
    QWidget::resizeEvent(event);
}

void TopToolbar::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    dApp->setApplicationCursor(Qt::ArrowCursor);
    DFrame::enterEvent(event);
}

void TopToolbar::initConnection()
{
    //rectangle, triangle,ellipse
    //connect(m_commonShapeWidget, &CommonshapeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    //connect(m_colorARect, &ArrowRectangle::hideWindow, m_commonShapeWidget, &CommonshapeWidget::resetColorBtns);
    connect(m_commonShapeWidget, SIGNAL(signalRectRediusChanged(int)), this, SLOT(slotRectRediusChanged(int)));
    connect(m_commonShapeWidget, &CommonshapeWidget::signalRectRediusIsfocus, this, &TopToolbar::signalCutLineEditIsfocus);
    ///polygonalStar
    //connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    //connect(m_colorARect, &ArrowRectangle::hideWindow, m_polygonalStarWidget, &PolygonalStarAttributeWidget::resetColorBtns);
    connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::signalAnchorvalueIsfocus, this, &TopToolbar::signalCutLineEditIsfocus);
    connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::signalRadiusvalueIsfocus, this, &TopToolbar::signalCutLineEditIsfocus);
    ///polygon
    //connect(m_PolygonWidget, &PolygonAttributeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    //connect(m_colorARect, &ArrowRectangle::hideWindow, m_PolygonWidget, &PolygonAttributeWidget::resetColorBtns);
    connect(m_PolygonWidget, &PolygonAttributeWidget::signalSideValueIsfocus, this, &TopToolbar::signalCutLineEditIsfocus);
    //draw line.
    //connect(m_drawLineWidget, &LineWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    //connect(m_colorARect, &ArrowRectangle::hideWindow, m_drawLineWidget, &LineWidget::resetColorBtns);
    //draw pen.
    //connect(m_penWidget, &CPenWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    //connect(m_colorARect, &ArrowRectangle::hideWindow, m_penWidget, &CPenWidget::resetColorBtns);
    //draw text.
    //connect(m_drawTextWidget, &TextWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    //connect(m_colorARect, &ArrowRectangle::hideWindow, m_drawTextWidget, &TextWidget::resetColorBtns);
    connect(m_drawTextWidget, &TextWidget::signalTextFontSizeChanged, this, &TopToolbar::signalTextFontSizeChanged);
    //draw blur widget.

    //cut
    connect(m_cutWidget, &CCutWidget::signalCutLineEditIsfocus, this, &TopToolbar::signalCutLineEditIsfocus);

    //CManagerAttributeService
    connect(CManagerAttributeService::getInstance(), SIGNAL(signalShowWidgetCommonProperty(EGraphicUserType, QMap<EDrawProperty, QVariant>, bool)),
            this, SLOT(updateMiddleWidgetMult(EGraphicUserType, QMap<EDrawProperty, QVariant>, bool)));
    connect(CManagerAttributeService::getInstance(), SIGNAL(signalIsAllPictureItem(bool, bool)),
            this, SLOT(slotIsAllPictureItem(bool, bool)));
}

