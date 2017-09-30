#include "toptoolbar.h"
#include "../application.h"

#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenu>
#include <QDebug>

#include <DApplication>

#include "utils/configsettings.h"
#include "utils/global.h"
#include "utils/imageutils.h"

#include "widgets/pushbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/dialog/drawdialog.h"
#include "widgets/dialog/savedialog.h"
#include "widgets/textfontlabel.h"

#include "controller/importer.h"

DWIDGET_USE_NAMESPACE

TopToolbar::TopToolbar(QWidget* parent)
: QFrame(parent)
{
    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");

    QLabel* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(24, 25);
    logoLabel->setObjectName("LogoLabel");

    QList<PushButton*> actionPushButtons;

    PushButton* artBoardBtn = new PushButton(this);
    artBoardBtn->setObjectName("ArtBoardBtn");
    artBoardBtn->setToolTip(tr("Dimension"));
    actionPushButtons.append(artBoardBtn);

    PushButton* picBtn = new PushButton(this);
    picBtn->setObjectName("PictureBtn");
    picBtn->setToolTip(tr("Import"));
    actionPushButtons.append(picBtn);

    PushButton* rectBtn = new PushButton(this);
    rectBtn->setObjectName("RectBtn");
    rectBtn->setToolTip(tr("Rectangle"));
    actionPushButtons.append(rectBtn);

    PushButton* ovalBtn = new PushButton(this);
    ovalBtn->setObjectName("OvalBtn");
    ovalBtn->setToolTip(tr("Ellipse"));
    actionPushButtons.append(ovalBtn);

    PushButton* lineBtn = new PushButton(this);
    lineBtn->setObjectName("LineBtn");
    lineBtn->setToolTip(tr("Pencil"));
    actionPushButtons.append(lineBtn);

    PushButton* textBtn = new PushButton(this);
    textBtn->setObjectName("TextBtn");
    textBtn->setToolTip(tr("Text"));
    actionPushButtons.append(textBtn);

    PushButton* blurBtn = new PushButton(this);
    blurBtn->setObjectName("BlurBtn");
    blurBtn->setToolTip(tr("Blur"));
    actionPushButtons.append(blurBtn);

    PushButton* selectBtn = new PushButton(this);
    selectBtn->setObjectName("SelectedBtn");
    selectBtn->setToolTip(tr("Selected"));
    actionPushButtons.append(selectBtn);

    initStackWidget();

    PushButton* exportBtn = new PushButton(this);
    exportBtn->setObjectName("ExportBtn");
    exportBtn->setToolTip(tr("Save"));
    exportBtn->setCheckable(false);

    initMenu();

    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(10);
    m_layout->addWidget(logoLabel);
    m_layout->addSpacing(15);
    m_layout->addWidget(artBoardBtn);
    m_layout->addSpacing(20);
    m_layout->addWidget(picBtn);
    m_layout->addWidget(rectBtn);
    m_layout->addWidget(ovalBtn);
    m_layout->addWidget(lineBtn);
    m_layout->addWidget(textBtn);
    m_layout->addWidget(blurBtn);
    m_layout->addWidget(selectBtn);
    m_layout->addWidget(m_stackWidget, 0, Qt::AlignCenter);
    m_layout->addWidget(exportBtn);
    m_layout->addSpacing(20);
    this->setLayout(m_layout);

    connect(picBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        picBtn->setChecked(true);
        importImage();
    });

    connect(this, &TopToolbar::drawShapeChanged,
            this, [=](QString shape){
        if (shape == "image" && !picBtn->isChecked())
        {
            picBtn->setChecked(true);
        }
    });

    connect(rectBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        rectBtn->setChecked(true);
        setMiddleStackWidget(Status::FillShape);
        drawShapes("rectangle");
    });

    connect(ovalBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        ovalBtn->setChecked(true);
        setMiddleStackWidget(Status::FillShape);
        drawShapes("oval");
    });

    connect(lineBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        lineBtn->setChecked(true);
        setMiddleStackWidget(Status::DrawLine);
        int styleIndex = ConfigSettings::instance()->value("line", "style").toInt();
        switch (styleIndex) {
        case 0: drawShapes("straightLine"); break;
        case 1: drawShapes("arbitraryCurve"); break;
        default: drawShapes("arrow"); break;
        }
    });

    connect(textBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        textBtn->setChecked(true);
        setMiddleStackWidget(Status::DrawText);
        drawShapes("text");
    });

    connect(blurBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        blurBtn->setChecked(true);
        setMiddleStackWidget(Status::DrawBlur);
        drawShapes("blur");
    });

    connect(selectBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        selectBtn->setChecked(true);
        setMiddleStackWidget(Status::Empty);
        emit fillShapeSelectedActive(selectBtn->getChecked());

        drawShapes("selected");
    });

    connect(artBoardBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, actionPushButtons)
        {
            button->setChecked(false);
        }
        artBoardBtn->setChecked(true);
        setMiddleStackWidget(Status::AdjustSize);
    });

    connect(exportBtn, &PushButton::clicked, this, &TopToolbar::showSaveDialog);
}

void TopToolbar::importImage()
{
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle(tr("Import Image"));
    dialog->setAcceptMode(QFileDialog::AcceptOpen);

    m_paths = QFileDialog::getOpenFileNames(this, tr("Open Images"),
                                            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                            ("Files(*.*);;"
                                             "Files (*.bmp *.bmp24);;"
                                             "Files(*.ico);;Files(*.jpg *.jpe *.jpeg *.jpeg24);;"
                                             "Files( *.jng );;Files(*.pcd *.pcx);; "
                                             "Files(*.png);;"
                                             "Files(*.tga *.tif *.tiff *.tiff24);;"
                                             "Files(*.psd);; Files(*.xpm);;"
                                             "Files(*.dds);;Files(*.gif);;Files(*.sgi *.j2k *jp2);;"
                                             "Files(*.pct );;"
                                             "Files(*.webp *.wdp);;"
                                             "Files(*.cr2 *.pef *.arw );;"
                                             "Files(*.nef *.icb *.dng);;"
                                             "Files(*.vda *.vst *.raf *.orf);;"
                                             "Files(*.svg);;"
                                             "Files(*.ptif *.mef *.mrw *.xbm)"));

    using namespace utils::image;
    if (m_paths.length() > 0)
    {
        emit Importer::instance()->importedFiles(m_paths);
    }
}

void TopToolbar::importImageDir()
{
    qDebug() << "import image dir";

    drawShapes("image");
    setMiddleStackWidget(Status::Cut);
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle(tr("Import Image"));
    dialog->setAcceptMode(QFileDialog::AcceptOpen);

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qDebug() << "dir"  << dir;


    Importer::instance()->appendDir(dir);
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
    connect(m_cutWidget, &CutWidget::cutImage, this, [=]{
        drawShapes("cutImage");
    });
    connect(m_cutWidget, &CutWidget::mirroredImage, this, &TopToolbar::mirroredImage);

    //colorPanel.
    m_colorPanel = new ColorPanel();
    m_colorARect = new DArrowRectangle(DArrowRectangle::ArrowTop);
    m_colorARect->setWindowFlags(Qt::Popup);
//    m_colorARect->setFocusPolicy(Qt::StrongFocus);
    m_colorARect->setArrowX(25);
    m_colorARect->setArrowWidth(30);
    m_colorARect->setContent(m_colorPanel);
    m_colorARect->setBackgroundColor(QColor(255, 255, 255, 0.5));

    connect(m_colorPanel, &ColorPanel::updateHeight, this, [=]{
        m_colorARect->setContent(m_colorPanel);
    });

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

    connect(this, &TopToolbar::adjustArtBoardSize, m_adjustsizeWidget,
            &AdjustsizeWidget::updateCanvasSize);

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

    Q_UNUSED(printAc);
    Q_UNUSED(themeAc);
    Q_UNUSED(helpAc);
   dApp->setProductIcon(QPixmap(":/theme/common/images/deepin-draw-96.png"));
   dApp->setApplicationDescription(tr("Deepin Draw is a lightweight drawing tool."
                " You can freely draw on the layer or simplely edit images. "));
   dApp->setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin-draw");

   connect(importAc, &QAction::triggered, this, &TopToolbar::importImage);
   connect(dApp, &Application::popupConfirmDialog, this, &TopToolbar::showDrawDialog);
    connect(saveAc, &QAction::triggered, this, &TopToolbar::showSaveDialog);
    connect(saveAsAc, &QAction::triggered, this, &TopToolbar::showSaveDialog);
}

void TopToolbar::showDrawDialog()
{
    DrawDialog* dd = new DrawDialog;
    dd->showInCenter(window());
}

void TopToolbar::showSaveDialog()
{
    SaveDialog* sd = new SaveDialog;
    QDateTime currentDate;
    QString currentTime =  currentDate.currentDateTime().
             toString("yyyyMMddHHmmss");
    QString imageName = QString("DeepinDraw_%1.png").arg(currentTime);

    emit sd->imageNameChanged(imageName);
    sd->showInCenter(window());


    connect(sd, &SaveDialog::saveToPath, this, &TopToolbar::saveImageAction);
}

void TopToolbar::updateMiddleWidget(QString type)
{
    if (type == "image")
    {
        setMiddleStackWidget(Status::Cut);
    } else if (type == "rectangle" || type == "oval")
    {
        setMiddleStackWidget(Status::FillShape);
    } else if (type == "arrow" || type == "straightLine" || type == "arbitraryCurve")
    {
        setMiddleStackWidget(Status::DrawLine);
    } else if (type == "blur")
    {
        setMiddleStackWidget(Status::DrawBlur);
    } else if (type == "text")
    {
        setMiddleStackWidget(Status::DrawText);
    } else
    {
        qDebug() << "updateMiddleWidget type:" << type;
    }
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
    emit drawShapeChanged(shape);
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
