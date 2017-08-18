#include "textfontlabel.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

#include "toolbutton.h"
#include "utils/global.h"
#include "utils/configsettings.h"

const int LINEEDIT_WIDTH = 190;
const int BUTTON_ICON = 24;

TextFontLabel::TextFontLabel(QWidget *parent)
    : QLabel(parent),
      m_fontsize(12)
{
    DRAW_THEME_INIT_WIDGET("TextFontLabel");

    this->setObjectName("TextFontLabel");
    this->setFixedSize(LINEEDIT_WIDTH+BUTTON_ICON*2,
                                      BUTTON_ICON + 2);
    QLineEdit* fontEdit = new QLineEdit(this);
    fontEdit->setObjectName("FontEdit");
    fontEdit->setFixedSize(LINEEDIT_WIDTH, 24);
    fontEdit->setObjectName("FontsizeEdit");
    m_fontsize = ConfigSettings::instance()->value("text", "fontsize").toInt();
    fontEdit->setText(QString("%1").arg(m_fontsize));

    ToolButton* addBtn = new ToolButton(this);
    addBtn->setObjectName("AddFontsizeBtn");
    ToolButton* reduceBtn = new ToolButton(this);
    reduceBtn->setObjectName("ReduceFontsizeBtn");

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(fontEdit, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addWidget(addBtn, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addWidget(reduceBtn, 0, Qt::AlignCenter);
    setLayout(hLayout);

    connect(addBtn, &ToolButton::clicked, this, [=]{
        m_fontsize += 1;
        fontEdit->setText(QString("%1").arg(m_fontsize));
        ConfigSettings::instance()->setValue("text", "fontsize", m_fontsize);
    });

    connect(reduceBtn, &ToolButton::clicked, this, [=]{
        m_fontsize -= 1;
        m_fontsize = std::max(8, m_fontsize);
        fontEdit->setText(QString("%1").arg(m_fontsize));
        ConfigSettings::instance()->setValue("text", "fontsize", m_fontsize);
    });
}

TextFontLabel::~TextFontLabel()
{
}
