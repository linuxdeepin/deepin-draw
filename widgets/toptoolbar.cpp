#include "toptoolbar.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QDebug>

#include "utils/global.h"
#include "utils/baseutils.h"
#include "widgets/pushbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"

TopToolbar::TopToolbar(QWidget* parent)
: QFrame(parent), m_shapesWidgetExist(false) {
    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(24, 25);
    logoLabel->setObjectName("LogoLabel");

    ToolButton* artBoardBtn = new ToolButton(this);
    artBoardBtn->setObjectName("ArtBoardBtn");
    ToolButton* importBtn = new ToolButton(this);
    importBtn->setObjectName("ImportBtn");
    importBtn->setCheckable(false);
    connect(importBtn, &ToolButton::clicked, this, &TopToolbar::importImage);

    ToolButton* rectBtn = new ToolButton(this);
    rectBtn->setObjectName("RectBtn");
    connect(rectBtn, &ToolButton::clicked, this,  [=]{
        drawShapes("rectangle");
    });

    ToolButton* ovalBtn = new ToolButton(this);
    ovalBtn->setObjectName("OvalBtn");
    connect(ovalBtn, &ToolButton::clicked, this, [=]{
        drawShapes("oval");
    });

    ToolButton* lineBtn = new ToolButton(this);
    lineBtn->setObjectName("LineBtn");
    connect(lineBtn, &ToolButton::clicked, this, [=]{
        drawShapes("line");
    });

    ToolButton* textBtn = new ToolButton(this);
    textBtn->setObjectName("TextBtn");
    connect(textBtn, &ToolButton::clicked, this, [=]{
        drawShapes("text");
    });

    ToolButton* blurBtn = new ToolButton(this);
    blurBtn->setObjectName("BlurBtn");

    initStackWidget();

    ToolButton* exportBtn = new ToolButton(this);
    exportBtn->setObjectName("ExportBtn");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(10);
    mLayout->addWidget(logoLabel);
    mLayout->addSpacing(15);
    mLayout->addWidget(artBoardBtn);
    mLayout->addSpacing(20);
    mLayout->addWidget(importBtn);
    mLayout->addWidget(rectBtn);
    mLayout->addWidget(ovalBtn);
    mLayout->addWidget(lineBtn);
    mLayout->addWidget(textBtn);
    mLayout->addWidget(blurBtn);
    mLayout->addStretch(1);
    mLayout->addWidget(m_stackWidget);
    mLayout->addStretch(1);
    mLayout->addWidget(exportBtn);
    mLayout->addSpacing(30);
    setLayout(mLayout);

}

void TopToolbar::importImage() {
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle(tr("Import Image"));
    dialog->setAcceptMode(QFileDialog::AcceptOpen);


    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"),
             QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
             tr("Image Files (*.png *.jpg *.bmp)"));

    emit openImage(fileName);
}

void TopToolbar::initStackWidget() {
    m_stackWidget = new QStackedWidget(this);

    //colorPanel.
    m_colorPanel = new ColorPanel();
    m_strokeARect = new DArrowRectangle(DArrowRectangle::ArrowTop, this);
    m_strokeARect->setArrowX(25);
    m_strokeARect->setArrowWidth(30);
    m_strokeARect->setContent(m_colorPanel);
    m_strokeARect->setBackgroundColor(QColor(255, 255, 255, 0.5));

    //draw rectangle, and oval.
    m_fillShapeWidget = new QWidget(this);
    BigColorButton* fillColorBtn = new BigColorButton(this);
    QLabel* fillColLabel = new QLabel(this);
    fillColLabel->setText(tr("Fill"));
    connect(fillColorBtn, &BigColorButton::clicked, this, [=]{
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
    });
    BorderColorButton* fillShapeStrokeBtn = new BorderColorButton(this);
    fillShapeStrokeBtn->setObjectName("FillStrokeButton");
    connect(fillShapeStrokeBtn, &BorderColorButton::clicked, this, [=]{
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
    });
    QLabel* strokeLabel = new QLabel(this);
    strokeLabel->setText(tr("Stroke"));
    SeperatorLine* fillShapeSepLine = new SeperatorLine();
    QLabel* fillShapeLWLabel = new QLabel(this);
    fillShapeLWLabel->setObjectName("BorderLabel");
    fillShapeLWLabel->setText(tr("Width"));
    ToolButton* lineBtn0 = new ToolButton(this);
    lineBtn0->setObjectName("FinerLineBtn");
    ToolButton* lineBtn1 = new ToolButton(this);
    lineBtn1->setObjectName("FineLineBtn");
    ToolButton* lineBtn2 = new ToolButton(this);
    lineBtn2->setObjectName("MediumLineBtn");
    ToolButton* lineBtn3 = new ToolButton(this);
    lineBtn3->setObjectName("BoldLineBtn");
    QHBoxLayout* fillHLayout = new QHBoxLayout(this);
    fillHLayout->setMargin(0);
    fillHLayout->setSpacing(6);
    fillHLayout->addStretch();
    fillHLayout->addWidget(fillColorBtn);
    fillHLayout->addWidget(fillColLabel);
    fillHLayout->addWidget(fillShapeStrokeBtn);
    fillHLayout->addWidget(strokeLabel);
    fillHLayout->addWidget(fillShapeSepLine);
    fillHLayout->addWidget(fillShapeLWLabel);
    fillHLayout->addWidget(lineBtn0);
    fillHLayout->addWidget(lineBtn1);
    fillHLayout->addWidget(lineBtn2);
    fillHLayout->addWidget(lineBtn3);
    fillHLayout->addStretch();
    m_fillShapeWidget->setLayout(fillHLayout);
    m_stackWidget->addWidget(m_fillShapeWidget);

    //process image.
    m_cutWidget = new QWidget(this);
    PushButton* leftRotateBtn = new PushButton();
    leftRotateBtn->setObjectName("LeftRotate");
    leftRotateBtn->setText(tr("Rotate 90° CCW"));

    PushButton* rightRotateBtn = new PushButton(this);
    rightRotateBtn->setObjectName("RightRotate");
    rightRotateBtn->setText(tr("Rotate 90° CW"));

    PushButton* cutBtn = new PushButton(this);
    cutBtn->setObjectName("CutButton");
    cutBtn->setText(tr("Clip"));

    PushButton* horiFlipBtn = new PushButton(this);
    horiFlipBtn->setObjectName("FlipHorizontalBtn");
    horiFlipBtn->setText(tr("Flip horizontally"));

    PushButton* verticaFlipBtn = new PushButton(this);
    verticaFlipBtn->setObjectName("FlipVerticalBtn");
    verticaFlipBtn->setText("Flip vertically");

    QHBoxLayout* cutHbLayout = new QHBoxLayout(m_cutWidget);
    cutHbLayout->setMargin(0);
    cutHbLayout->setSpacing(0);
    cutHbLayout->addWidget(leftRotateBtn);
    cutHbLayout->addWidget(rightRotateBtn);
    cutHbLayout->addWidget(cutBtn);
    cutHbLayout->addWidget(horiFlipBtn);
    cutHbLayout->addWidget(verticaFlipBtn);
    m_cutWidget->setLayout(cutHbLayout);
    m_stackWidget->addWidget(m_cutWidget);

    //draw line.
    m_drawShapeWidget = new QWidget(this);
    QLabel* borderColLabel = new QLabel(this);
    borderColLabel->setObjectName("BorderStrokeLabel");
    borderColLabel->setText(tr("Stroke"));
    BorderColorButton* borderCButton = new BorderColorButton(this);
    connect(borderCButton, &BorderColorButton::clicked, this, [=]{
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
    });

    SeperatorLine* sep1Line = new SeperatorLine(this);
    QLabel* borderStyleLabel = new QLabel(this);
    borderStyleLabel->setObjectName("BorderStyleLabel");
    borderStyleLabel->setText(tr("Style"));

    ToolButton* sLineBtn = new ToolButton(this);
    sLineBtn->setObjectName("StraightLineBtn");
    ToolButton* arbiLineBtn = new ToolButton(this);
    arbiLineBtn->setObjectName("ArbiLineBtn");
    ToolButton* arrowBtn = new ToolButton(this);
    arrowBtn->setObjectName("ArrowBtn");
    SeperatorLine* sep2Line = new SeperatorLine(this);

    QLabel* borderLWLabel = new QLabel(this);
    borderLWLabel->setObjectName("BorderLWLabel");
    borderLWLabel->setText(tr("Width"));
    ToolButton* finerLineBtn = new ToolButton(this);
    finerLineBtn->setObjectName("FinerLineBtn");
    ToolButton* fineLineBtn = new ToolButton(this);
    fineLineBtn->setObjectName("FineLineBtn");
    ToolButton* mediumLineBtn = new ToolButton(this);
    mediumLineBtn->setObjectName("MediumLineBtn");
    ToolButton* boldLineBtn = new ToolButton(this);
    boldLineBtn->setObjectName("BoldLineBtn");

    QHBoxLayout* drawHbLayout = new QHBoxLayout(m_drawShapeWidget);
    drawHbLayout->setMargin(0);
    drawHbLayout->setSpacing(10);
    drawHbLayout->addWidget(borderColLabel);
    drawHbLayout->addWidget(borderCButton);
    drawHbLayout->addWidget(sep1Line, 0, Qt::AlignCenter);
    drawHbLayout->addWidget(borderStyleLabel);
    drawHbLayout->addWidget(sLineBtn);
    drawHbLayout->addWidget(arbiLineBtn);
    drawHbLayout->addWidget(arrowBtn);
    drawHbLayout->addWidget(sep2Line, 0, Qt::AlignCenter);
    drawHbLayout->addWidget(borderLWLabel);
    drawHbLayout->addWidget(finerLineBtn);
    drawHbLayout->addWidget(fineLineBtn);
    drawHbLayout->addWidget(mediumLineBtn);
    drawHbLayout->addWidget(boldLineBtn);
    m_drawShapeWidget->setLayout(drawHbLayout);
    m_stackWidget->addWidget(m_drawShapeWidget);

    //process  artboard's size.
    m_picWidget = new QWidget(this);
    QLabel* casLengthLabel = new QLabel(this);
    casLengthLabel->setObjectName("CasLengthLabel");
    QLineEdit* lengthLEdit = new QLineEdit(this);
    lengthLEdit->setObjectName("LengthLineEdit");
    lengthLEdit->setStyleSheet("background-color: red;");
    QLabel* casWidthLabel = new QLabel(this);
    casWidthLabel->setObjectName("CasWidthLabel");
    QLineEdit* widthLEdit = new QLineEdit(this);
    widthLEdit->setObjectName("WidthLineEdit");
    QLineEdit* cutTransAreaLEdit = new QLineEdit(this);
    cutTransAreaLEdit->setObjectName("CutAreaLineEdit");
    QHBoxLayout* picHbLayout = new QHBoxLayout(m_picWidget);
    picHbLayout->addWidget(casLengthLabel);
    picHbLayout->addWidget(lengthLEdit);
    picHbLayout->addWidget(casWidthLabel);
    picHbLayout->addWidget(widthLEdit);
    picHbLayout->addWidget(cutTransAreaLEdit);
    m_picWidget->setLayout(picHbLayout);
    m_stackWidget->addWidget(m_picWidget);

    //draw text.
    m_textWidget = new QWidget(this);
    ToolButton* colBtn = new ToolButton(this);
    QLabel* fontsizeLabel = new QLabel(this);
    QLineEdit* fontsizeEdit = new QLineEdit(this);
    fontsizeEdit->setObjectName("FontsizeEdit");
    ToolButton* addFontsizeBtn = new ToolButton(this);
    addFontsizeBtn->setObjectName("AddFontsizeButton");
    ToolButton* reduceFontsizeBtn = new ToolButton(this);
    reduceFontsizeBtn->setObjectName("ReduceFontsizeButton");
    QHBoxLayout* textHbLayout = new QHBoxLayout(m_textWidget);
    textHbLayout->addWidget(colBtn);
    textHbLayout->addWidget(fontsizeLabel);
    textHbLayout->addWidget(fontsizeEdit);
    textHbLayout->addWidget(addFontsizeBtn);
    textHbLayout->addWidget(reduceFontsizeBtn);
    m_textWidget->setLayout(textHbLayout);
    m_stackWidget->addWidget(m_textWidget);

    //draw blur widget.
    m_blurWidget = new QWidget(this);
    QLabel* penLabel = new QLabel(this);
    ToolButton* fineBtn = new ToolButton(this);
    QSlider* lineWidthSlider = new QSlider(Qt::Horizontal, this);
    ToolButton* boldBtn = new ToolButton(this);
    QHBoxLayout* blurHbLayout = new QHBoxLayout(m_blurWidget);
    blurHbLayout->addWidget(penLabel);
    blurHbLayout->addWidget(fineBtn);
    blurHbLayout->addWidget(lineWidthSlider);
    blurHbLayout->addWidget(boldBtn);
    m_blurWidget->setLayout(blurHbLayout);
    m_stackWidget->addWidget(m_blurWidget);

    m_stackWidget->setCurrentWidget(m_fillShapeWidget);
}

void TopToolbar::drawShapes(QString shape) {
    if (!m_shapesWidgetExist) {
        emit initShapeWidgetAction(shape, true);
        m_shapesWidgetExist = true;
    } else {
        emit initShapeWidgetAction(shape, false);
    }
}

bool TopToolbar::shapesWidgetExist() {
    return m_shapesWidgetExist;
}

void TopToolbar::resizeEvent(QResizeEvent *event) {
    this->updateGeometry();
}

TopToolbar::~TopToolbar() {
}
