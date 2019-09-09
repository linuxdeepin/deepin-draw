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
#include "adjustsizewidget.h"
#include "widgets/arrowrectangle.h"
#include "widgets/colorpanel.h"
#include "widgets/dialog/drawdialog.h"
#include "widgets/dialog/savedialog.h"
#include "utils/tempfile.h"

#include <DComboBox>
#include <DApplication>
#include <QHBoxLayout>
#include <QString>

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
    hLayout->addSpacing(20);
    hLayout->addWidget(m_stackWidget);
    hLayout->setContentsMargins(0, 0, 0, 0);
//    hLayout->addStretch();
    setLayout(hLayout);

//    m_stackWidget->setStyleSheet("background-color: blue;");
//    setStyleSheet("background-color: rgb(255, 0, 0);");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void TopToolbar::initComboBox()
{
    //创建画板放大缩小的combobox
    m_scaleComboBox = new DComboBox(this);
    //QComboBox *m_scaleComboBox = new QComboBox(this);
    QStringList scaleList = {"200%", "100%", "75%", "50%", "25%"};
    m_scaleComboBox->addItems(scaleList);
    m_scaleComboBox->insertSeparator(1);
    m_scaleComboBox->insertSeparator(3);
    m_scaleComboBox->insertSeparator(5);
    m_scaleComboBox->insertSeparator(7);
    m_scaleComboBox->setCurrentIndex(2);
    m_scaleComboBox->setFixedWidth(100);


    //static_cast<QStandardItemModel *>(static_cast<QComboBox *>(m_scaleComboBox)->view()->model())->item(0)->setTextAlignment(Qt::AlignCenter);
//    static_cast<QStandardItemModel *>(m_scaleComboBox->view()->model())->item(2)->setTextAlignment(Qt::AlignCenter);
//    static_cast<QStandardItemModel *>(m_scaleComboBox->view()->model())->item(4)->setTextAlignment(Qt::AlignCenter);
//    static_cast<QStandardItemModel *>(m_scaleComboBox->view()->model())->item(6)->setTextAlignment(Qt::AlignCenter);
//    static_cast<QStandardItemModel *>(m_scaleComboBox->view()->model())->item(8)->setTextAlignment(Qt::AlignCenter);
    m_scaleComboBox->setEditable(true);

    m_scaleComboBox->setFocusPolicy(Qt::NoFocus);
    m_scaleComboBox->lineEdit()->setReadOnly(true);

    connect(m_scaleComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotZoom(const QString &)));
}

void TopToolbar::initStackWidget()
{
    m_stackWidget = new DStackedWidget(this);
    m_emptyWidget = new DWidget(this);
    m_stackWidget->addWidget(m_emptyWidget);

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

    m_stackWidget->setCurrentWidget(m_emptyWidget);
}

void TopToolbar::initMenu()
{

    m_mainMenu = new DMenu(this);
    m_mainMenu->setFixedWidth(120);
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
//    DMenu *themMenu = new DMenu(tr("Theme"));
//    QAction *themeLightAc = themMenu->addAction(tr("Light theme"));
//    QAction *themeDarkAc = themMenu->addAction(tr("Dark theme"));
//    QAction *themeSystemAc = themMenu->addAction(tr("System theme"));
//    m_mainMenu->addMenu(themMenu);

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
        case::EGraphicUserType::PictureType:
            toolType = EDrawToolMode::importPicture;
            break;
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

    m_scaleComboBox->setEditText(strScale);
//    m_scaleComboBox->setEditable(false);
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

void TopToolbar::initConnection()
{
    //colorPanel.
    connect(m_colorPanel, &ColorPanel::updateHeight, this, [ = ] {m_colorARect->setContent(m_colorPanel);});
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
    connect(m_colorPanel, &ColorPanel::signalColorChanged, m_drawTextWidget, &TextWidget::updateTextWidget);
    connect(m_drawTextWidget, &TextWidget::signalTextAttributeChanged, this, &TopToolbar::signalAttributeChanged);
    //draw blur widget.
    connect(TempFile::instance(), &TempFile::saveDialogPopup, this, &TopToolbar::showSaveDialog);


}

