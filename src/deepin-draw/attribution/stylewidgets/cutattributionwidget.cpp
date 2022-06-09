#include "cutattributionwidget.h"
#include "cspinbox.h"
#include "hboxlayoutwidget.h"
#include <QLabel>
#include <QSpinBox>
#include "drawboard.h"
#include "drawboardtoolmgr.h"
#include "ccuttool.h"
#define CUTTYPE 6
#define MAXROWNUM 3
CutAttributionWidget::CutAttributionWidget(DrawBoard *drawBoard, QWidget *parent): AttributeWgt(ECutToolAttri, parent)
    , m_drawBoard(drawBoard)
{
    initUi();
    initConnect();
}

void CutAttributionWidget::initUi()
{
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QHBoxLayout *m_scaleStyle = new QHBoxLayout(this);
    QLabel *title = new QLabel(tr("scale"));
    QLabel *scale_w = new QLabel();
    scale_w->setText("W");
    w_spinbox = new CSpinBox();
    w_spinbox->setEnabledEmbedStyle(true);
    w_spinbox->setSpinRange(0, 10000);
    w_spinbox->setMinimumWidth(90);
    w_spinbox->setValue(1920);

    QLabel *scale_h = new QLabel();
    scale_h->setText("H");
    h_spinbox = new CSpinBox();
    h_spinbox->setEnabledEmbedStyle(true);
    h_spinbox->setSpinRange(0, 10000);
    h_spinbox->setMinimumWidth(90);
    h_spinbox->setValue(1080);

    mainlayout->addWidget(title);
    m_scaleStyle->addWidget(scale_w);
    m_scaleStyle->addWidget(w_spinbox);
    m_scaleStyle->addWidget(scale_h);
    m_scaleStyle->addWidget(h_spinbox);

    QGridLayout *m_gridLayout = new QGridLayout();
//    m_gridLayout->setAlignment(Qt::AlignLeft);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setMargin(10);
    m_gridLayout->setVerticalSpacing(10);
    m_gridLayout->setHorizontalSpacing(10);
    QStringList cuttype_name = {tr("original"), tr("free"), "1:1", "2:3", "8:5", "16:9"};
    QStringList cuticon = {"original", "free", "1.1", "2.3", "8.5", "16.9"};
    for (int i = 0; i < CUTTYPE; ++i) {
        QToolButton *button_type = new QToolButton(this);
        button_type->setIconSize(QSize(30, 30));
        button_type->setIcon(QIcon::fromTheme(cuticon[i]));
        button_type->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button_type->setText(cuttype_name[i]);
        m_buttonList.push_back(button_type);
        m_gridLayout->addWidget(button_type, i / MAXROWNUM, i % MAXROWNUM);
    }

    mainlayout->addLayout(m_scaleStyle);
    mainlayout->addLayout(m_gridLayout);
    setLayout(mainlayout);
}

void CutAttributionWidget::initConnect()
{
    auto tool_manager = m_drawBoard->toolManager();
    for (int i = 0; i < m_buttonList.size(); ++i) {
        connect(m_buttonList[i], &DToolButton::clicked, this, [ = ]() {
            CCutTool *current_tool =  dynamic_cast<CCutTool *>(tool_manager->tool(tool_manager->currentTool()));
            auto scene = m_drawBoard->currentPage()->scene();
            current_tool->changeCutType(i, scene);
        });
    }

    connect(h_spinbox, &CSpinBox::valueChanged, this, [ = ]() {
        CCutTool *current_tool =  dynamic_cast<CCutTool *>(tool_manager->tool(tool_manager->currentTool()));
        auto scene = m_drawBoard->currentPage()->scene();
        current_tool->changeCutSize(scene, getCutSzie());
    });

    connect(w_spinbox, &CSpinBox::valueChanged, this, [ = ]() {
        CCutTool *current_tool =  dynamic_cast<CCutTool *>(tool_manager->tool(tool_manager->currentTool()));
        auto scene = m_drawBoard->currentPage()->scene();
        current_tool->changeCutSize(scene, getCutSzie());
    });

}

QSize CutAttributionWidget::getCutSzie()
{
    return QSize(w_spinbox->value(), h_spinbox->value());
}
