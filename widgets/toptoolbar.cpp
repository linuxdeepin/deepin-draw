#include "toptoolbar.h"

#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QMenu>
#include <QDebug>

#include <DApplication>

#include "utils/configsettings.h"
#include "utils/global.h"
#include "widgets/pushbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/toolbutton.h"

#include "textfontlabel.h"

DWIDGET_USE_NAMESPACE

TopToolbar::TopToolbar(QWidget* parent)
: QFrame(parent),
  m_shapesWidgetExist(false)
{
    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(24, 25);
    logoLabel->setObjectName("LogoLabel");

    QButtonGroup* shapesBtnGroup = new QButtonGroup(this);
    shapesBtnGroup->setExclusive(true);

    ToolButton* artBoardBtn = new ToolButton(this);
    artBoardBtn->setObjectName("ArtBoardBtn");
    shapesBtnGroup->addButton(artBoardBtn);

    ToolButton* picBtn = new ToolButton(this);
    picBtn->setObjectName("PictureBtn");
    shapesBtnGroup->addButton(picBtn);

    ToolButton* rectBtn = new ToolButton(this);
    rectBtn->setObjectName("RectBtn");
    shapesBtnGroup->addButton(rectBtn);

    ToolButton* ovalBtn = new ToolButton(this);
    ovalBtn->setObjectName("OvalBtn");
    shapesBtnGroup->addButton(ovalBtn);

    ToolButton* lineBtn = new ToolButton(this);
    lineBtn->setObjectName("LineBtn");
    shapesBtnGroup->addButton(lineBtn);

    ToolButton* textBtn = new ToolButton(this);
    textBtn->setObjectName("TextBtn");
    shapesBtnGroup->addButton(textBtn);

    ToolButton* blurBtn = new ToolButton(this);
    blurBtn->setObjectName("BlurBtn");
    shapesBtnGroup->addButton(blurBtn);

    initStackWidget();

    ToolButton* exportBtn = new ToolButton(this);
    exportBtn->setObjectName("ExportBtn");

    initMenu();

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(10);
    mLayout->addWidget(logoLabel);
    mLayout->addSpacing(15);
    mLayout->addWidget(artBoardBtn);
    mLayout->addSpacing(20);
    mLayout->addWidget(picBtn);
    mLayout->addWidget(rectBtn);
    mLayout->addWidget(ovalBtn);
    mLayout->addWidget(lineBtn);
    mLayout->addWidget(textBtn);
    mLayout->addWidget(blurBtn);
    mLayout->addStretch();
    mLayout->addWidget(m_stackWidget);
    mLayout->addStretch();
    mLayout->addWidget(exportBtn);
    mLayout->addSpacing(30);
    setLayout(mLayout);

    connect(picBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::Cut);
    });
    connect(rectBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::FillShape);
        drawShapes("rectangle");
    });
    connect(ovalBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::FillShape);
        drawShapes("oval");
    });
    connect(lineBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::DrawLine);
        int styleIndex = ConfigSettings::instance()->value("line", "style").toInt();
        switch (styleIndex) {
        case 0: drawShapes("straightLine"); break;
        case 1: drawShapes("arbitraryCurve"); break;
        default: drawShapes("arrow"); break;
        }
    });
    connect(textBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::DrawText);
        drawShapes("text");
    });
    connect(blurBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::DrawBlur);
        drawShapes("blur");
    });
    connect(artBoardBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::AdjustSize);
    });
}

void TopToolbar::importImage()
{
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle(tr("Import Image"));
    dialog->setAcceptMode(QFileDialog::AcceptOpen);


    m_path = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                          QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                          tr("Image Files (*.png *.jpg *.bmp, *svg)"));

    emit openImage(m_path);
}

void TopToolbar::initStackWidget()
{
    m_stackWidget = new QStackedWidget(this);

    m_emptyWidget = new QWidget(this);
    m_stackWidget->addWidget(m_emptyWidget);

    m_cutWidget = new CutWidget(this);
    m_stackWidget->addWidget(m_cutWidget);

    //colorPanel.
    m_colorPanel = new ColorPanel();
    m_colorARect = new DArrowRectangle(DArrowRectangle::ArrowTop, this);
    m_colorARect->setArrowX(25);
    m_colorARect->setArrowWidth(30);
    m_colorARect->setContent(m_colorPanel);
    m_colorARect->setBackgroundColor(QColor(255, 255, 255, 0.5));

    //fill rectangle, and oval.
    m_fillShapeWidget = new FillshapeWidget(this);
    m_stackWidget->addWidget(m_fillShapeWidget);
    connect(m_fillShapeWidget, &FillshapeWidget::showColorPanel,
                    this, &TopToolbar::showColorfulPanel);

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

    //draw blur widget.
    m_drawBlurWidget = new BlurWidget(this);
    m_stackWidget->addWidget(m_drawBlurWidget);

    //process  artboard's size.
    m_adjustsizeWidget = new AdjustsizeWidget(this);
    m_stackWidget->addWidget(m_adjustsizeWidget);

    m_stackWidget->setCurrentWidget(m_emptyWidget);
}

void TopToolbar::initMenu()
{
    m_mainMenu = new QMenu(this);
    QAction* importAc = m_mainMenu->addAction(tr("Import"));
//    QAction* importFScannerAc = m_mainMenu->addAction(tr("Import from scanner"));
    QAction* saveAc = m_mainMenu->addAction(tr("Save"));
    QAction* saveAsAc = m_mainMenu->addAction(tr("Save as"));
    QAction* printAc = m_mainMenu->addAction(tr("Print"));
    QAction* themeAc = m_mainMenu->addAction(tr("Dark theme"));
    QAction* helpAc = m_mainMenu->addAction(tr("Help"));

//    Q_UNUSED(importFScannerAc);
    Q_UNUSED(saveAc);
    Q_UNUSED(saveAsAc);
    Q_UNUSED(printAc);
    Q_UNUSED(themeAc);
    Q_UNUSED(helpAc);
   qApp->setProductIcon(QPixmap(":/theme/common/images/deepin-draw.png"));
   qApp->setApplicationDescription(tr("Deepin Draw is a lightweight drawing tool."
                " You can freely draw on the layer or simplely edit images. "
                "Deepin Draw is released under GPL v3."));
   connect(importAc, &QAction::triggered, this, &TopToolbar::importImage);
}

void TopToolbar::setMiddleStackWidget(Status status)
{
    switch (status)
    {
    case Empty: m_stackWidget->setCurrentWidget(m_emptyWidget); break;
    case Cut: m_stackWidget->setCurrentWidget(m_cutWidget); break;
    case DrawLine: m_stackWidget->setCurrentWidget(m_drawLineWidget); break;
    case FillShape: m_stackWidget->setCurrentWidget(m_fillShapeWidget); break;
    case DrawText: m_stackWidget->setCurrentWidget(m_drawTextWidget); break;
    case DrawBlur: m_stackWidget->setCurrentWidget(m_drawBlurWidget); break;
    case AdjustSize: m_stackWidget->setCurrentWidget(m_adjustsizeWidget); break;
    default: break;
    }
}

void TopToolbar::setDrawStatus(DrawStatus drawstatus)
{
    m_drawStatus = drawstatus;
}

void TopToolbar::showColorfulPanel(DrawStatus drawstatus, QPoint pos)
{
    m_drawStatus = drawstatus;
    m_colorPanel->setDrawStatus(m_drawStatus);
    m_colorPanel->updateColorButtonStatus();

    if (m_colorARect->isHidden())
        m_colorARect->show(pos.x(), pos.y() + 8);
    else
        m_colorARect->hide();
}

void TopToolbar::drawShapes(QString shape)
{
    if (!m_shapesWidgetExist) {
        emit initShapeWidgetAction(shape);
        m_shapesWidgetExist = true;
    } else {
        emit initShapeWidgetAction(shape);
    }
}

bool TopToolbar::shapesWidgetExist()
{
    return m_shapesWidgetExist;
}

QMenu* TopToolbar::mainMenu()
{
    return m_mainMenu;
}

void TopToolbar::resizeEvent(QResizeEvent *event)
{
    this->updateGeometry();
    Q_UNUSED(event);
}

TopToolbar::~TopToolbar()
{
}
