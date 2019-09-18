/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "textfontlabel.h"

//#include <QLineEdit>
#include <DPushButton>
#include <QHBoxLayout>

#include "fontsizelineedit.h"
#include "toolbutton.h"
#include "utils/global.h"
#include "utils/configsettings.h"

const int LINEEDIT_WIDTH = 64;
const int BUTTON_WIDTH = 25;
const int BUTTON_HEIGHT = 26;

TextFontLabel::TextFontLabel(DWidget *parent)
    : DLabel(parent),
      m_fontsize(12)
{
    //DRAW_THEME_INIT_WIDGET("TextFontLabel");

    this->setObjectName("TextFontLabel");
    this->setFixedSize(LINEEDIT_WIDTH +  BUTTON_WIDTH * 2,
                       BUTTON_HEIGHT);
    FontsizeLineEdit *fontEdit = new FontsizeLineEdit(this);
    fontEdit->setObjectName("FontsizeEdit");
    fontEdit->setFixedSize(LINEEDIT_WIDTH, BUTTON_HEIGHT - 2);
    m_fontsize = ConfigSettings::instance()->value("text", "fontsize").toInt();
    fontEdit->setText(QString("%1").arg(m_fontsize));

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this, [ = ](
    const QString & group, const QString & key) {
        if (group == "text" && key == "fontsize") {
            m_fontsize = ConfigSettings::instance()->value(group, key).toInt();
            fontEdit->setText(QString("%1").arg(m_fontsize));
        }
    });
    connect(fontEdit, &QLineEdit::editingFinished, this, [ = ] {
        int fontSize = fontEdit->text().toInt();
        fontSize = std::max(8, fontSize);
        fontEdit->setText(QString("%1").arg(fontSize));
        m_fontsize = fontSize;
        ConfigSettings::instance()->setValue("text", "fontsize", fontSize);
    });
    connect(fontEdit, &FontsizeLineEdit::addSize, this, [ = ] {
        int fontSize = fontEdit->text().toInt();
        fontSize = std::max(8, fontSize + 1);
        m_fontsize = fontSize;
        fontEdit->setText(QString("%1").arg(fontSize));
    });
    connect(fontEdit, &FontsizeLineEdit::reduceSize, this, [ = ] {
        int fontSize = fontEdit->text().toInt();
        fontSize = std::max(8, fontSize - 1);
        m_fontsize = fontSize;
        fontEdit->setText(QString("%1").arg(fontSize));
    });

    ToolButton *addBtn = new ToolButton(this);
    addBtn->setObjectName("AddFontsizeBtn");
    addBtn->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    addBtn->setAutoRepeat(true);
    ToolButton *reduceBtn = new ToolButton(this);
    reduceBtn->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    reduceBtn->setObjectName("ReduceFontsizeBtn");
    reduceBtn->setAutoRepeat(true);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(fontEdit, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addWidget(addBtn, 0, Qt::AlignCenter);
    hLayout->addSpacing(0);
    hLayout->addWidget(reduceBtn, 0, Qt::AlignCenter);
    setLayout(hLayout);

    connect(addBtn, &ToolButton::pressed, this, [ = ] {
        m_fontsize += 1;
        fontEdit->setText(QString("%1").arg(m_fontsize));
        ConfigSettings::instance()->setValue("text", "fontsize", m_fontsize);
    });

    connect(reduceBtn, &ToolButton::pressed, this, [ = ] {
        m_fontsize -= 1;
        m_fontsize = std::max(8, m_fontsize);
        fontEdit->setText(QString("%1").arg(m_fontsize));
        ConfigSettings::instance()->setValue("text", "fontsize", m_fontsize);
    });
}

TextFontLabel::~TextFontLabel()
{
}
