#include "toptoolbar.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>

#include "utils/global.h"
#include "utils/baseutils.h"
#include "widgets/seperatorline.h"

#include <DTitlebar>

DWIDGET_USE_NAMESPACE

TopToolbar::TopToolbar(QWidget* parent)
: QFrame(parent) {

    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(24, 25);
    logoLabel->setObjectName("LogoLabel");

    ToolButton* artBoardBtn = new ToolButton(this);
    artBoardBtn->setObjectName("ArtBoardBtn");
    ToolButton* importBtn = new ToolButton(this);
    importBtn->setObjectName("ImportBtn");

    ToolButton* rectBtn = new ToolButton(this);
    rectBtn->setObjectName("RectBtn");
    ToolButton* ovalBtn = new ToolButton(this);
    ovalBtn->setObjectName("OvalBtn");
    ToolButton* lineBtn = new ToolButton(this);
    lineBtn->setObjectName("LineBtn");
    ToolButton* textBtn = new ToolButton(this);
    textBtn->setObjectName("TextBtn");
    ToolButton* blurBtn = new ToolButton(this);
    blurBtn->setObjectName("BlurBtn");

    initStackWidget();

    ToolButton* exportBtn = new ToolButton(this);
    exportBtn->setObjectName("ExportBtn");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(10);
    mLayout->addWidget(logoLabel);
    mLayout->addSpacing(100);
    mLayout->addWidget(artBoardBtn);
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
    mLayout->addSpacing(100);
    setLayout(mLayout);
}

void TopToolbar::initStackWidget() {
    m_stackWidget = new QStackedWidget(this);

//    //cutWidget
    m_cutWidget = new QWidget(this);
    ToolButton* leftRotateBtn = new ToolButton(this);
    leftRotateBtn->setObjectName("LeftRotate");
    ToolButton* rightRotateBtn = new ToolButton(this);
    rightRotateBtn->setObjectName("RightRotate");
    ToolButton* cutBtn = new ToolButton(this);
    cutBtn->setObjectName("CutButton");
    ToolButton* horiFlipBtn = new ToolButton(this);
    horiFlipBtn->setObjectName("FlipHorizontalBtn");
    ToolButton* verticaFlipBtn = new ToolButton(this);
    verticaFlipBtn->setObjectName("FlipVerticalBtn");

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

    //drawShapeWidget
    m_drawShapeWidget = new QWidget(this);
    ToolButton* borderColBtn = new ToolButton(this);
    QLabel* borderColLabel = new QLabel(this);
    borderColLabel->setObjectName("BorderStrokeLabel");
    borderColLabel->setText(tr("Stroke"));

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
    drawHbLayout->addWidget(borderColBtn);
    drawHbLayout->addWidget(borderColLabel);
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

    //pictureWidget
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

    //textWidget
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

    //blurWidget
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

    m_stackWidget->setCurrentWidget(m_cutWidget);
}

void TopToolbar::resizeEvent(QResizeEvent *event) {
    this->updateGeometry();
}

TopToolbar::~TopToolbar() {}
