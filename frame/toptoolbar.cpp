#include "toptoolbar.h"
#include "application.h"

#include <DApplication>
#include <DComboBox>

#include <QHBoxLayout>

#include "commonshapewidget.h"
#include "polygonalstarattributewidget.h"
#include "polygonattributewidget.h"
#include "ailoringwidget.h"
#include "linewidget.h"
#include "cutwidget.h"
#include "textwidget.h"
#include "blurwidget.h"
#include "adjustsizewidget.h"
#include "widgets/arrowrectangle.h"
#include "widgets/colorpanel.h"
#include "widgets/dialog/drawdialog.h"
#include "widgets/dialog/savedialog.h"
#include "utils/global.h"
#include "utils/tempfile.h"

DWIDGET_USE_NAMESPACE

TopToolbar::TopToolbar(QWidget *parent)
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
    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");

    initComboBox();
    initStackWidget();
    initMenu();

    DLabel *logoLabel = new DLabel(this);
    logoLabel->setFixedSize(QSize(32, 32));
    logoLabel->setObjectName("LogoLabel");

    QHBoxLayout *hLayout = new QHBoxLayout (this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(logoLabel);
    hLayout->addSpacing(20);
    hLayout->addWidget(m_scaleComboBox);
    hLayout->addSpacing(20);
    hLayout->addWidget(m_stackWidget);
    hLayout->addStretch();
    setLayout(hLayout);

//    setStyleSheet("background-color: rgb(255, 0, 0);");
}

void TopToolbar::initStackWidget()
{
    m_stackWidget = new QStackedWidget(this);
    m_emptyWidget = new QWidget(this);
    m_stackWidget->addWidget(m_emptyWidget);

    //cut
    m_cutWidget = new CutWidget(this);
    m_stackWidget->addWidget(m_cutWidget);

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

    //draw text.
    m_drawTextWidget = new TextWidget(this);
    m_stackWidget->addWidget(m_drawTextWidget);

    //draw blur widget.
    m_drawBlurWidget = new BlurWidget(this);
    m_stackWidget->addWidget(m_drawBlurWidget);

    m_ailoringWidget = new AiloringWidget(this);
    m_stackWidget->addWidget(m_ailoringWidget);

    //process  artboard's size.
//    m_adjustsizeWidget = new AdjustsizeWidget(this);
//    m_stackWidget->addWidget(m_adjustsizeWidget);

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

void TopToolbar::initComboBox()
{
    //创建画板放大缩小的combobox
    m_scaleComboBox = new DComboBox(this);
    //QComboBox *m_scaleComboBox = new QComboBox(this);
    QStringList scaleList = {"200%", "100%", "75%", "50%", "25%"};
    m_scaleComboBox->addItems(scaleList);
    m_scaleComboBox->setCurrentIndex(1);
    m_scaleComboBox->setFixedWidth(80);
}

void TopToolbar::showSaveDialog()
{
    SaveDialog *sd = new SaveDialog(TempFile::instance()->savedImage(), this);
    sd->showInCenter(window());
}

void TopToolbar::updateMiddleWidget(int type)
{
    switch (type) {
    case::importPicture:
        m_stackWidget->setCurrentWidget(m_cutWidget);
        break;
    case::rectangle:
    case::ellipse:
    case::triangle:
        m_commonShapeWidget->updateCommonShapWidget();
        m_stackWidget->setCurrentWidget(m_commonShapeWidget);
        break;
    case::polygonalStar:
        m_stackWidget->setCurrentWidget(m_polygonalStarWidget);
        break;
    case::polygon:
        m_stackWidget->setCurrentWidget(m_PolygonWidget);
        break;
    case::pen:
        m_stackWidget->setCurrentWidget(m_drawLineWidget);
        break;
    case::text:
        m_stackWidget->setCurrentWidget(m_drawTextWidget);
        break;
    case::blur:
        m_stackWidget->setCurrentWidget(m_drawBlurWidget);
        break;
    case::cut:
        m_stackWidget->setCurrentWidget(m_ailoringWidget);
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

        m_colorARect->show(pos.x() - mapToGlobal(startPos).x() + 157,
                           pos.y()  + 15 - mapToGlobal(startPos).y());
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


QMenu *TopToolbar::mainMenu()
{
    return m_mainMenu;
}

void TopToolbar::resizeEvent(QResizeEvent *event)
{
    this->updateGeometry();
    m_colorARect->hide();
    QWidget::resizeEvent(event);
}

void TopToolbar::initConnection()
{
    //colorPanel.
    connect(m_colorPanel, &ColorPanel::updateHeight, this, [ = ] {m_colorARect->setContent(m_colorPanel);});

    //rectangle, triangle,ellipse
    connect(m_commonShapeWidget, &CommonshapeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_commonShapeWidget, &CommonshapeWidget::resetColorBtns);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_commonShapeWidget, &CommonshapeWidget::updateCommonShapWidget);
    connect(m_colorPanel, &ColorPanel::signalColorChanged, this, &TopToolbar::signalAttributeChanged);
    connect(m_commonShapeWidget, &CommonshapeWidget::signalCommonShapeChanged, this, &TopToolbar::signalAttributeChanged);
    ///polygonalStar
    connect(m_polygonalStarWidget, &PolygonalStarAttributeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_polygonalStarWidget, &PolygonalStarAttributeWidget::resetColorBtns);

    ///polygonalStar
    connect(m_PolygonWidget, &PolygonAttributeWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_PolygonWidget, &PolygonAttributeWidget::resetColorBtns);

    //draw line.
    connect(m_drawLineWidget, &LineWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);

    //draw text.
    connect(m_drawTextWidget, &TextWidget::showColorPanel, this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_drawTextWidget, &TextWidget::resetColorBtns);
    //draw blur widget.

    connect(TempFile::instance(), &TempFile::saveDialogPopup, this, &TopToolbar::showSaveDialog);
}

