#include "toptoolbar.h"
#include "application.h"

#include <DApplication>

#include <QHBoxLayout>

#include "commonshapewidget.h"
#include "polygonalstarattributewidget.h"
#include "polygonattributewidget.h"
#include "ailoringwidget.h"
#include "utils/configsettings.h"
#include "utils/global.h"
#include "utils/imageutils.h"
#include "utils/tempfile.h"
#include "drawshape/drawtool.h"
#include "widgets/pushbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/dialog/drawdialog.h"
#include "widgets/dialog/savedialog.h"
#include "widgets/textfontlabel.h"
#include "controller/importer.h"

DWIDGET_USE_NAMESPACE



TopToolbar::TopToolbar(QWidget *parent)
    : DFrame(parent)
{

    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");

    DLabel *logoLabel = new DLabel(this);
    logoLabel->setFixedSize(QSize(32, 32));
    logoLabel->setObjectName("LogoLabel");

    initStackWidget();

    QHBoxLayout *hLayout = new QHBoxLayout (this);
    hLayout->addWidget(logoLabel, 0, Qt::AlignLeft);

    hLayout->addWidget(m_stackWidget, 80, Qt::AlignHCenter);

    setLayout(hLayout);



//    PushButton* exportBtn = new PushButton(this);
//    exportBtn->setObjectName("ExportBtn");
//    exportBtn->setToolTip(tr("Save"));
//    exportBtn->setCheckable(false);

    initMenu();

//    connect(exportBtn, &PushButton::clicked, this, &TopToolbar::generateSaveImage);
    connect(TempFile::instance(), &TempFile::saveDialogPopup, this, &TopToolbar::showSaveDialog);
}



void TopToolbar::initStackWidget()
{
    m_stackWidget = new QStackedWidget(this);
    m_emptyWidget = new QWidget(this);
    m_stackWidget->addWidget(m_emptyWidget);

    //cut
    m_cutWidget = new CutWidget(this);
    m_stackWidget->addWidget(m_cutWidget);
    connect(m_cutWidget, &CutWidget::rotateImage, this, &TopToolbar::rotateImage);
    connect(m_cutWidget, &CutWidget::cutImage, this, [ = ] {
        drawShapes("cutImage");
    });
    connect(m_cutWidget, &CutWidget::mirroredImage, this, &TopToolbar::mirroredImage);
    connect(this, &TopToolbar::cutImageFinished, m_cutWidget, &CutWidget::cutImageBtnReset);

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
    connect(m_colorPanel, &ColorPanel::updateHeight, this, [ = ] {
        m_colorARect->setContent(m_colorPanel);
    });

    //rectangle, triangle,oval
    m_commonShapeWidget = new CommonshapeWidget(this);
    m_stackWidget->addWidget(m_commonShapeWidget);
    connect(m_commonShapeWidget, &CommonshapeWidget::showColorPanel,
            this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_commonShapeWidget,
            &CommonshapeWidget::resetColorBtns);
    ///polygonalStar
    m_polygonalStarWidget = new PolygonalStarAttributeWidget(this);
    m_stackWidget->addWidget(m_polygonalStarWidget);
    connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::showColorPanel,
            this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_polygonalStarWidget,
            &PolygonalStarAttributeWidget::resetColorBtns);

    ///polygonalStar
    m_PolygonWidget = new PolygonAttributeWidget(this);
    m_stackWidget->addWidget(m_PolygonWidget);
    connect(m_PolygonWidget, &PolygonAttributeWidget::showColorPanel,
            this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_PolygonWidget,
            &PolygonAttributeWidget::resetColorBtns);

    //draw line.
    m_drawLineWidget = new LineWidget(this);
    m_stackWidget->addWidget(m_drawLineWidget);
    connect(m_drawLineWidget, &LineWidget::showColorPanel,
            this, &TopToolbar::showColorfulPanel);

    //draw text.
    m_drawTextWidget = new TextWidget(this);
    m_stackWidget->addWidget(m_drawTextWidget);
    connect(m_drawTextWidget, &TextWidget::showColorPanel,
            this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_drawTextWidget,
            &TextWidget::resetColorBtns);
    //draw blur widget.
    m_drawBlurWidget = new BlurWidget(this);
    m_stackWidget->addWidget(m_drawBlurWidget);

    m_ailoringWidget = new AiloringWidget(this);
    m_stackWidget->addWidget(m_ailoringWidget);

    //process  artboard's size.
    m_adjustsizeWidget = new AdjustsizeWidget(this);
    m_stackWidget->addWidget(m_adjustsizeWidget);

    connect(this, &TopToolbar::adjustArtBoardSize, m_adjustsizeWidget,
            &AdjustsizeWidget::updateCanvasSize);
    connect(this, &TopToolbar::resizeArtboard,
            m_adjustsizeWidget, &AdjustsizeWidget::resizeCanvasSize);
    connect(m_adjustsizeWidget, &AdjustsizeWidget::autoCrop, this,
            &TopToolbar::autoCrop);

    m_stackWidget->setCurrentWidget(m_emptyWidget);
}

void TopToolbar::initMenu()
{

    m_mainMenu = new DMenu(this);
    //m_mainMenu->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    //m_mainMenu->setWindowFlags(Qt::FramelessWindowHint);
    //m_mainMenu->setBackgroundColor(QColor(248, 168, 0));

    QAction *newConstructAc = m_mainMenu->addAction(tr("New"));
    QAction *importAc = m_mainMenu->addAction(tr("Import"));
    m_mainMenu->addSeparator();
    QAction *exportAc = m_mainMenu->addAction(tr("Export"));
    QAction *saveAc = m_mainMenu->addAction(tr("Save"));
    QAction *saveAsAc = m_mainMenu->addAction(tr("Save as"));
    QAction *printAc = m_mainMenu->addAction(tr("Print"));
    m_mainMenu->addSeparator();
    QAction *themeAc = m_mainMenu->addAction(tr("Theme"));
    QAction *helpAc = m_mainMenu->addAction(tr("Help"));


//    Q_UNUSED(themeAc);
    Q_UNUSED(helpAc);
    dApp->setProductIcon(QIcon(QPixmap(":/theme/common/images/deepin-draw-96.svg")));
    dApp->setApplicationDescription(tr("Deepin Draw is a lightweight drawing tool."
                                       " You can freely draw on the layer or simply edit images. "));
    dApp->setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin-draw/");

    connect(importAc, &QAction::triggered, this, &TopToolbar::importPicBtnClicked);
//    connect(dApp, &Application::popupConfirmDialog, this, &TopToolbar::showDrawDialog);
    connect(saveAc, &QAction::triggered, this, &TopToolbar::generateSaveImage);
    connect(printAc, &QAction::triggered, this, &TopToolbar::printImage);
}


void TopToolbar::showSaveDialog()
{
    SaveDialog *sd = new SaveDialog(TempFile::instance()->savedImage(), this);
    sd->showInCenter(window());
}

void TopToolbar::updateMiddleWidget(int type)
{
    switch (type) {
    case::ImportPicture:
        m_stackWidget->setCurrentWidget(m_cutWidget);
        break;
    case::CommonShape:
        m_stackWidget->setCurrentWidget(m_commonShapeWidget);
        break;
    case::DrawPolygonalStar:
        m_stackWidget->setCurrentWidget(m_polygonalStarWidget);
        break;
    case::DrawPolygon:
        m_stackWidget->setCurrentWidget(m_PolygonWidget);
        break;
    case::DrawPen:
        m_stackWidget->setCurrentWidget(m_drawLineWidget);
        break;
    case::DrawText:
        m_stackWidget->setCurrentWidget(m_drawTextWidget);
        break;
    case::DrawBlur:
        m_stackWidget->setCurrentWidget(m_drawBlurWidget);
        break;
    case::Cut:
        m_stackWidget->setCurrentWidget(m_ailoringWidget);
        break;
    default:
        break;
    }
//    if (type == "image")
//    {
//        emit updateSelectedBtn(true);
//        setMiddleStackWidget(MiddleWidgetStatus::Cut);
//        m_cutWidget->cutImageBtnReset();
//    }
//    else if (type == "rectangle" || type == "oval")
//    {
//        setMiddleStackWidget(MiddleWidgetStatus::FillShape);
//    } else if (type == "arrow" || type == "straightLine" || type == "arbitraryCurve")
//    {
//        setMiddleStackWidget(MiddleWidgetStatus::DrawLine);
//    } else if (type == "blur")
//    {
//        setMiddleStackWidget(MiddleWidgetStatus::DrawBlur);
//    } else if (type == "text")
//    {
//        setMiddleStackWidget(MiddleWidgetStatus::DrawText);
//    } else if (type == "adjustSize")
//    {
//        setMiddleStackWidget(MiddleWidgetStatus::AdjustSize);
//    }
}

void TopToolbar::setMiddleStackWidget(int status)
{
//    m_middleWidgetStatus = status;
//    m_colorPanel->setMiddleWidgetStatus(status);
    switch (status) {
//    case Empty: m_stackWidget->setCurrentWidget(m_emptyWidget); break;
//    case Cut: m_stackWidget->setCurrentWidget(m_cutWidget); break;
//    case DrawLine: m_stackWidget->setCurrentWidget(m_drawLineWidget); break;
    case CommonShape:

        break;
//    case DrawText:   emit m_drawTextWidget->updateColorBtn();
//        m_stackWidget->setCurrentWidget(m_drawTextWidget); break;
//    case DrawBlur: m_stackWidget->setCurrentWidget(m_drawBlurWidget); break;
//    case AdjustSize: m_stackWidget->setCurrentWidget(m_adjustsizeWidget); break;
    default:
        break;
    }
}

void TopToolbar::setDrawStatus(DrawStatus drawstatus)
{
    m_drawStatus = drawstatus;
}

void TopToolbar::showColorfulPanel(DrawStatus drawstatus, QPoint pos, bool visible)
{
    Q_UNUSED(pos);
    m_drawStatus = drawstatus;
    m_colorPanel->setDrawStatus(m_drawStatus);
    m_colorPanel->setMiddleWidgetStatus(m_middleWidgetStatus);
    m_colorPanel->updateColorButtonStatus();

    m_colorARect->raise();
    if (visible) {
        QPoint startPos = QPoint(0, 0);
        m_colorARect->show( cursor().pos().x() - mapToGlobal(startPos).x(),
                            cursor().pos().y() + 25 - mapToGlobal(startPos).y());
    } else
        m_colorARect->hide();
}



void TopToolbar::updateColorPanelVisible(QPoint pos)
{
    QRect colorPanelGeom = m_colorARect->geometry();
    if (!colorPanelGeom.contains(pos)) {
        m_colorARect->hide();
    }

    qDebug() << "updateColorPanelVisible:" << colorPanelGeom << pos;
}

MiddleWidgetStatus TopToolbar::middleWidgetStatus()
{
    return m_middleWidgetStatus;

}

void TopToolbar::drawShapes(QString shape)
{
    emit drawShapeChanged(shape);
}

QMenu *TopToolbar::mainMenu()
{
    return m_mainMenu;
}

void TopToolbar::resizeEvent(QResizeEvent *event)
{
    this->updateGeometry();
    m_colorARect->hide();
    Q_UNUSED(event);
}

void TopToolbar::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == (Qt::AltModifier | Qt::ShiftModifier)) {
//        qDebug() << "combine keyEvent!";
        GlobalShortcut::instance()->setShiftScStatus(true);
        GlobalShortcut::instance()->setAltScStatus(true);
    }
    if (e->key() == Qt::Key_Alt) {
        GlobalShortcut::instance()->setAltScStatus(true);
    }
    if (e->key() == Qt::Key_Shift) {
        GlobalShortcut::instance()->setShiftScStatus(true);
    }
}

void TopToolbar::keyReleaseEvent(QKeyEvent *e)
{
    if (e->modifiers() == (Qt::AltModifier | Qt::ShiftModifier)) {
        qDebug() << "combine keyEvent!";
        GlobalShortcut::instance()->setShiftScStatus(false);
        GlobalShortcut::instance()->setAltScStatus(false);
    }
    if (e->key() == Qt::Key_Alt) {
        GlobalShortcut::instance()->setAltScStatus(false);
    }
    if (e->key() == Qt::Key_Shift) {
        GlobalShortcut::instance()->setShiftScStatus(false);
    }
}

void TopToolbar::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
//    qDebug() << "toptoolbar mousePressEvent:" << m_colorARect->hasFocus();

//    if (!m_colorARect->hasFocus())
//        m_colorARect->hide();
}

TopToolbar::~TopToolbar()
{
}
