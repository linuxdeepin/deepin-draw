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
#include <QApplication>

#include <DApplication>

#include "utils/configsettings.h"
#include "utils/global.h"
#include "utils/imageutils.h"
#include "utils/tempfile.h"

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
    logoLabel->setFixedSize(24, 24);
    logoLabel->setObjectName("LogoLabel");

    PushButton* artBoardBtn = new PushButton(this);
    artBoardBtn->setObjectName("ArtBoardBtn");
    artBoardBtn->setToolTip(tr("Dimension"));
    m_actionPushButtons.append(artBoardBtn);

    PushButton* picBtn = new PushButton(this);
    picBtn->setObjectName("PictureBtn");
    picBtn->setToolTip(tr("Import"));
    m_actionPushButtons.append(picBtn);

    m_rectBtn = new PushButton(this);
    m_rectBtn->setObjectName("RectBtn");
    m_rectBtn->setToolTip(tr("Rectangle"));
    m_actionPushButtons.append(m_rectBtn);

    m_ovalBtn = new PushButton(this);
    m_ovalBtn->setObjectName("OvalBtn");
    m_ovalBtn->setToolTip(tr("Ellipse"));
    m_actionPushButtons.append(m_ovalBtn);

    m_lineBtn = new PushButton(this);
    m_lineBtn->setObjectName("LineBtn");
    m_lineBtn->setToolTip(tr("Pencil"));
    m_actionPushButtons.append(m_lineBtn);

    m_textBtn = new PushButton(this);
    m_textBtn->setObjectName("TextBtn");
    m_textBtn->setToolTip(tr("Text"));
    m_actionPushButtons.append(m_textBtn);

    m_blurBtn = new PushButton(this);
    m_blurBtn->setObjectName("BlurBtn");
    m_blurBtn->setToolTip(tr("Blur"));
    m_actionPushButtons.append(m_blurBtn);

    m_selectBtn = new PushButton(this);
    m_selectBtn->setObjectName("SelectedBtn");
    m_selectBtn->setToolTip(tr("Select"));
    m_actionPushButtons.append(m_selectBtn);

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
    m_layout->addWidget(m_rectBtn);
    m_layout->addWidget(m_ovalBtn);
    m_layout->addWidget(m_lineBtn);
    m_layout->addWidget(m_textBtn);
    m_layout->addWidget(m_blurBtn);
    m_layout->addWidget(m_selectBtn);
    m_layout->addWidget(m_stackWidget, 0, Qt::AlignCenter);
    m_layout->addWidget(exportBtn);
    m_layout->addSpacing(20);
    this->setLayout(m_layout);

    foreach(PushButton* button, m_actionPushButtons)
    {
        connect(button, &PushButton::clicked, this, [=]{
            if (m_cutWidget->cuttingStatus())
            {
                m_cutWidget->cutImageBtnReset();
            }
        });
    }
    connect(picBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        picBtn->setChecked(true);
        importImage();
    });
    connect(this, &TopToolbar::resetPicBtn, this, [=]{
        picBtn->setChecked(false);
    });
    connect(this, &TopToolbar::importPicBtnClicked, picBtn, &PushButton::clicked);
    connect(this, &TopToolbar::drawShapeChanged, this, [=](QString shape){
        if (shape == "image" && !picBtn->isChecked())
        {
            picBtn->setChecked(true);
        }
    });
    connect(m_rectBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        m_rectBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::FillShape);
        drawShapes("rectangle");
    });
    connect(m_ovalBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        m_ovalBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::FillShape);
        drawShapes("oval");
    });
    connect(m_lineBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        m_lineBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::DrawLine);
        drawShapes("line");
    });
    connect(m_textBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        m_textBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::DrawText);
        drawShapes("text");
    });
    connect(m_blurBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        m_blurBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::DrawBlur);
        drawShapes("blur");
    });
    connect(this, &TopToolbar::updateSelectedBtn, this, [=](bool checked){
        if (checked) {
            emit m_selectBtn->clicked();
        }
    });
    connect(m_selectBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        m_selectBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::Empty);
        emit fillShapeSelectedActive(m_selectBtn->getChecked());

        drawShapes("selected");
    });
    connect(artBoardBtn, &PushButton::clicked, this, [=]{
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
        artBoardBtn->setChecked(true);
        setMiddleStackWidget(MiddleWidgetStatus::AdjustSize);
        drawShapes("adjustSize");
    });
    connect(exportBtn, &PushButton::clicked, this, &TopToolbar::generateSaveImage);
    connect(TempFile::instance(), &TempFile::saveDialogPopup, this, &TopToolbar::showSaveDialog);
}

void TopToolbar::importImage()
{
    using namespace utils::image;
    QString selfilter = tr("All images ") + ("(%1)");
    selfilter = selfilter.arg(utils::image::supportedSuffixList().join(" "));
    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setOption(QFileDialog::HideNameFilterDetails);
    dialog.setNameFilter(selfilter);
    dialog.selectNameFilter(selfilter);

    if (dialog.exec() == QFileDialog::Accepted)
    {
        m_paths = dialog.selectedFiles();
        qDebug() << "load image succeed!";
    } else {
        m_paths = QStringList();
        qDebug() << "load image failed!";
    }

    qDebug() << "Load images:" << m_paths;
    if (m_paths.length() > 0)
    {
        emit Importer::instance()->importedFiles(m_paths);
    }
    emit resetPicBtn();
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
    connect(this, &TopToolbar::cutImageFinished, m_cutWidget, &CutWidget::cutImageBtnReset);

    //colorPanel.
    m_colorPanel = new ColorPanel(this);
    qApp->setProperty("_d_isDxcb",false);
    m_colorARect = new ArrowRectangle(DArrowRectangle::ArrowTop, this->parentWidget());
    qApp->setProperty("_d_isDxcb",true);
    m_colorARect->setWindowFlags(Qt::Widget);
    m_colorARect->setAttribute(Qt::WA_TranslucentBackground,false);
    m_colorARect->setArrowX(25);
    m_colorARect->setArrowWidth(30);
    m_colorARect->setContent(m_colorPanel);
   m_colorARect->hide();
   connect(m_colorPanel, &ColorPanel::updateHeight, this, [=]{
       m_colorARect->setContent(m_colorPanel);
   });

    //fill rectangle, and oval.
    m_fillShapeWidget = new FillshapeWidget(this);
    m_stackWidget->addWidget(m_fillShapeWidget);
    connect(m_fillShapeWidget, &FillshapeWidget::showColorPanel,
                    this, &TopToolbar::showColorfulPanel);
    connect(m_colorARect, &ArrowRectangle::hideWindow, m_fillShapeWidget,
            &FillshapeWidget::resetColorBtns);

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
    m_mainMenu = new QMenu(this);
    QAction* importAc = m_mainMenu->addAction(tr("Import"));
    m_mainMenu->addSeparator();
    QAction* saveAc = m_mainMenu->addAction(tr("Save"));
    QAction* printAc = m_mainMenu->addAction(tr("Print"));
    m_mainMenu->addSeparator();
    QAction* themeAc = m_mainMenu->addAction(tr("Dark theme"));
    m_mainMenu->addSeparator();
    QAction* helpAc = m_mainMenu->addAction(tr("Help"));

    Q_UNUSED(themeAc);
    Q_UNUSED(helpAc);
    dApp->setProductIcon(QIcon(QPixmap(":/theme/common/images/deepin-draw-96.svg")));
    dApp->setApplicationDescription(tr("Deepin Draw is a lightweight drawing tool."
                                       " You can freely draw on the layer or simplely edit images. "));
    dApp->setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin-draw/");

    connect(importAc, &QAction::triggered, this, &TopToolbar::importPicBtnClicked);
//    connect(dApp, &Application::popupConfirmDialog, this, &TopToolbar::showDrawDialog);
    connect(saveAc, &QAction::triggered, this, &TopToolbar::generateSaveImage);
    connect(printAc, &QAction::triggered, this, &TopToolbar::printImage);
}

//void TopToolbar::showDrawDialog()
//{
//    DrawDialog*  dd = new DrawDialog(this);
//    dd->showInCenter(window());

//    connect(dd, &DrawDialog::saveDrawImage, this, [=]{
//        TempFile::instance()->setSaveFinishedExit(true);
//        emit generateSaveImage();
//    });
//    dd->exec();
//}

void TopToolbar::showSaveDialog()
{
    SaveDialog* sd = new SaveDialog(TempFile::instance()->savedImage(), this);
    sd->showInCenter(window());
}

void TopToolbar::updateMiddleWidget(QString type)
{
    if (type == "image")
    {
        emit updateSelectedBtn(true);
        setMiddleStackWidget(MiddleWidgetStatus::Cut);
        m_cutWidget->cutImageBtnReset();
    } else if (type == "rectangle" || type == "oval")
    {
        setMiddleStackWidget(MiddleWidgetStatus::FillShape);
    } else if (type == "arrow" || type == "straightLine" || type == "arbitraryCurve")
    {
        setMiddleStackWidget(MiddleWidgetStatus::DrawLine);
    } else if (type == "blur")
    {
        setMiddleStackWidget(MiddleWidgetStatus::DrawBlur);
    } else if (type == "text")
    {
        setMiddleStackWidget(MiddleWidgetStatus::DrawText);
    } else if (type == "adjustSize")
    {
        setMiddleStackWidget(MiddleWidgetStatus::AdjustSize);
    }
}

void TopToolbar::setMiddleStackWidget(MiddleWidgetStatus status)
{
    m_middleWidgetStatus = status;
    m_colorPanel->setMiddleWidgetStatus(status);

    switch (status)
    {
    case Empty: m_stackWidget->setCurrentWidget(m_emptyWidget); break;
    case Cut: m_stackWidget->setCurrentWidget(m_cutWidget); break;
    case DrawLine: m_stackWidget->setCurrentWidget(m_drawLineWidget); break;
    case FillShape: m_stackWidget->setCurrentWidget(m_fillShapeWidget); break;
    case DrawText:   emit m_drawTextWidget->updateColorBtn();
        m_stackWidget->setCurrentWidget(m_drawTextWidget); break;
    case DrawBlur: m_stackWidget->setCurrentWidget(m_drawBlurWidget); break;
    case AdjustSize: m_stackWidget->setCurrentWidget(m_adjustsizeWidget); break;
    default: break;
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
    if (visible)
    {
        QPoint startPos = QPoint(0, 0);
        m_colorARect->show( cursor().pos().x() - mapToGlobal(startPos).x(),
                            cursor().pos().y() + 25 - mapToGlobal(startPos).y());
    }
    else
        m_colorARect->hide();
}

void TopToolbar::updateCurrentShape(QString shape)
{
    if (shape == "rectangle")
    {
        emit m_rectBtn->clicked();
    } else if (shape == "oval")
    {
        emit m_ovalBtn->clicked();
    } else if (shape == "straightLine")
    {
        emit m_lineBtn->clicked();
    } else if (shape == "text")
    {
        emit m_textBtn->clicked();
    } else if (shape == "blur")
    {
        emit m_blurBtn->clicked();
    } else if (shape == "selected")
    {
        emit m_selectBtn->clicked();
    } else {
        foreach(PushButton* button, m_actionPushButtons)
        {
            button->setChecked(false);
        }
    }
}

void TopToolbar::updateColorPanelVisible(QPoint pos)
{
    QRect colorPanelGeom = m_colorARect->geometry();
    if (!colorPanelGeom.contains(pos))
    {
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

QMenu* TopToolbar::mainMenu()
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
    if (e->key() == Qt::Key_Shift)
    {
        GlobalShortcut::instance()->setShiftScStatus(true);
    } else if (e->key() == Qt::Key_Alt)
    {
        GlobalShortcut::instance()->setAltScStatus(true);
    }
}

void TopToolbar::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift)
    {
        GlobalShortcut::instance()->setShiftScStatus(false);
    } else if (e->key() == Qt::Key_Alt)
    {
        GlobalShortcut::instance()->setAltScStatus(false);
    }
}

void TopToolbar::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "toptoolbar mousePressEvent:" << m_colorARect->hasFocus();

//    if (!m_colorARect->hasFocus())
//        m_colorARect->hide();
}

TopToolbar::~TopToolbar()
{
}
