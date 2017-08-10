#include "textfontlabel.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

#include "toolbutton.h"
#include "utils/global.h"

TextFontLabel::TextFontLabel(QWidget *parent)
    : QLabel(parent),
      m_fontsize(12)
{
    DRAW_THEME_INIT_WIDGET("TextFontLabel");

    this->setObjectName("TextFontLabel");
    this->setFixedHeight(26);
    QLineEdit* fontEdit = new QLineEdit(this);
    fontEdit->setObjectName("FontEdit");
    fontEdit->setFixedSize(190, 24);
    fontEdit->setObjectName("FontsizeEdit");
    fontEdit->setText(QString("%1").arg(m_fontsize));

    ToolButton* addBtn = new ToolButton(this);
    addBtn->setObjectName("AddFontsizeBtn");
    ToolButton* reduceBtn = new ToolButton(this);
    reduceBtn->setObjectName("ReduceFontsizeBtn");

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addStretch();
    hLayout->addWidget(fontEdit, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addWidget(addBtn, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addWidget(reduceBtn, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addStretch();
    setLayout(hLayout);

    connect(addBtn, &ToolButton::clicked, this, [=]{
        m_fontsize += 1;
        emit textFontsizeChanged(m_fontsize);
    });

    connect(reduceBtn, &ToolButton::clicked, this, [=]{
        m_fontsize -=1;
        m_fontsize = std::max(8, m_fontsize);
        emit textFontsizeChanged(m_fontsize);
    });
    connect(this, &TextFontLabel::textFontsizeChanged, this, [=]{
        fontEdit->setText(QString("%1").arg(m_fontsize));
    });
}

TextFontLabel::~TextFontLabel()
{

}
