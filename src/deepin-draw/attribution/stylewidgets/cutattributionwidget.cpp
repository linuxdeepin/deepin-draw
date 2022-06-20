#include "cutattributionwidget.h"
#include "cspinbox.h"
#include "hboxlayoutwidget.h"
#include <QLabel>
#include <QSpinBox>
#include "drawboard.h"
#include "drawboardtoolmgr.h"
#include "ccuttool.h"
#include <QButtonGroup>

CutAttributionWidget::CutAttributionWidget(DrawBoard *drawBoard, QWidget *parent): AttributeWgt(ECutToolAttri, parent)
    , m_drawBoard(drawBoard)
{
    initUi();
    initConnect();
}

void CutAttributionWidget::initUi()
{
    m_cutCutSize.setWidth(1920);
    m_cutCutSize.setHeight(1080);
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QHBoxLayout *m_scaleStyle = new QHBoxLayout(this);
    QLabel *title = new QLabel(tr("scale"));
    QLabel *scale_w = new QLabel();
    scale_w->setText("W");
    w_spinbox = new CSpinBox();
    w_spinbox->setEnabledEmbedStyle(true);
    w_spinbox->setSpinRange(10, 10000);
    w_spinbox->setMinimumWidth(90);
    w_spinbox->setValue(1920);

    QLabel *scale_h = new QLabel();
    scale_h->setText("H");
    h_spinbox = new CSpinBox();
    h_spinbox->setEnabledEmbedStyle(true);
    h_spinbox->setSpinRange(10, 10000);
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
    button_group = new QButtonGroup(this);
    for (int i = 0; i < CUTTYPE; ++i) {
        QToolButton *button_type = new QToolButton(this);
        button_type->setIconSize(QSize(30, 30));
        button_type->setIcon(QIcon::fromTheme(cuticon[i]));
        button_type->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button_type->setText(cuttype_name[i]);
        button_type->setCheckable(true);
        m_buttonList.push_back(button_type);
        button_group->addButton(button_type);
        m_gridLayout->addWidget(button_type, i / MAXROWNUM, i % MAXROWNUM);
    }

    //确认/取消按钮
    m_confirmbutton = new QToolButton(this);
    m_cancelbutton = new QToolButton(this);
    QHBoxLayout *m_buttonlayout = new QHBoxLayout(this);

    m_cancelbutton->setText(tr("cancel"));
    m_confirmbutton->setText(tr("confirm"));
    m_confirmbutton->setFixedSize(QSize(110, 38));
    m_cancelbutton->setFixedSize(QSize(110, 38));

    m_buttonlayout->addWidget(m_cancelbutton);
    m_buttonlayout->addWidget(m_confirmbutton);

    mainlayout->addLayout(m_scaleStyle);
    mainlayout->addLayout(m_gridLayout);
    mainlayout->addStretch();
    mainlayout->addLayout(m_buttonlayout);
    setLayout(mainlayout);
}

void CutAttributionWidget::initConnect()
{
    auto tool_manager = m_drawBoard->toolManager();
    connect(m_drawBoard, &DrawBoard::cutSizeChange, this, [ = ](QSizeF rect, bool setattr) {
        setCutSize(rect.toSize(), setattr);
    });

    for (int i = 0; i < m_buttonList.size(); ++i) {
        connect(m_buttonList[i], &DToolButton::clicked, this, [ = ]() {
            CCutTool *current_tool =  dynamic_cast<CCutTool *>(tool_manager->tool(tool_manager->currentTool()));
            cutstyle = i;
            QSizeF newSize = current_tool->changeCutType(cutstyle, m_drawBoard->currentPage()->scene());;
            if (cutstyle != ECutType::cut_original) {
                qreal rd = Radio[cutstyle];
                newSize.setWidth(qRound(newSize.height() * rd));
            }
            setCutSize(newSize.toSize(), true);
        });
    }
    connect(button_group, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [ = ](QAbstractButton * button, bool ischeckd) {
        cutstyle =  m_buttonList.indexOf(dynamic_cast<DToolButton *>(button));
    });

    connect(h_spinbox, &CSpinBox::valueChanged, this, [ = ]() {
        QSize newSize = m_cutCutSize;
        newSize.setHeight(h_spinbox->value());
        cutstyle = cut_free;
        setCutSize(newSize, true);
        h_spinbox->setFocus();
        //切换模式为自由
        m_buttonList[cut_free]->toggle();
    });

    connect(w_spinbox, &CSpinBox::valueChanged, this, [ = ]() {
        QSize newSize = m_cutCutSize;
        newSize.setWidth(w_spinbox->value());
        cutstyle = cut_free;
        setCutSize(newSize, true);
        w_spinbox->setFocus();
        //切换模式为自由
        m_buttonList[cut_free]->toggle();
    });
    connect(m_confirmbutton, &DToolButton::clicked, this, [ = ]() {
        CCutTool *current_tool =  dynamic_cast<CCutTool *>(tool_manager->tool(tool_manager->currentTool()));
        current_tool->doFinished(true, true);

    });
    connect(m_cancelbutton, &DToolButton::clicked, this, [ = ]() {
        CCutTool *current_tool =  dynamic_cast<CCutTool *>(tool_manager->tool(tool_manager->currentTool()));
        current_tool->doFinished(false, true);
    });

}

void CutAttributionWidget::setCutSize(const QSize &sz, bool setattr)
{
    if (sz != m_cutCutSize) {
        m_cutCutSize = sz;
        w_spinbox->setSpinValue(sz.width());
        h_spinbox->setSpinValue(sz.height());
        if (setattr) {
            QList<QVariant> vars;
            vars << cutstyle << sz;
            m_drawBoard->setDrawAttribution(ECutToolAttri, vars);
        }
    }
}

QSize CutAttributionWidget::getCutSzie()
{
    return QSize(w_spinbox->value(), h_spinbox->value());
}
