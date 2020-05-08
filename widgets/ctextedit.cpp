/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "ctextedit.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsmasicoitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <DMenu>
#include <DApplication>
#include <QAction>
#include <QDebug>
#include <QTextBlock>
#include <QTextEdit>

CTextEdit::CTextEdit(CGraphicsTextItem *item, QWidget *parent)
    : QTextEdit(parent)
    , m_pItem(item)
    , m_widthF(0)
{
    //初始化字体
    connect(this, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    this->setLineWrapMode(NoWrap);
    this->setFrameStyle(NoFrame);
}

CTextEdit::~CTextEdit()
{
    m_pItem = nullptr;
}

void CTextEdit::slot_textChanged()
{
    if (this->document()->isEmpty()) {
        this->setFontFamily(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family());
        this->setFontPointSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSize());
        this->setFontWeight(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().weight());
        this->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());
    }

    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    rect.setHeight(qMax(size.height(), rect.height()));
    rect.setWidth(size.width());

    //判断是否出界
//    QPointF bottomRight = rect.bottomRight();
//    QPointF bottomRightInScene = m_pItem->mapToScene(bottomRight);
//    if (m_pItem->scene() != nullptr && !m_pItem->scene()->sceneRect().contains(bottomRightInScene)) {
//        this->setLineWrapMode(WidgetWidth);
//        this->document()->setTextWidth(m_widthF);
//        size = this->document()->size();
//        rect.setHeight(size.height());
//        rect.setWidth(size.width());
//    }

    if (m_pItem != nullptr) {
        m_pItem->setRect(rect);
    }

    m_widthF = rect.width();

    cursorPositionChanged();

//    if (nullptr != m_pItem->scene()) {
//        auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
//        //更新字图元
//        curScene->updateBlurItem(m_pItem);
//    }
}

void CTextEdit::cursorPositionChanged()
{
    // 当删除所有文字后，格式会被重置为默认的属性，需要重新设置格式
    if (this->document()->isEmpty()) {
        this->setFontFamily(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family());
        this->setFontPointSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSize());
        this->setFontWeight(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().weight());
        this->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());
    } else {
        // 只要鼠标点击后，此函数就会被调用一次
        QTextCursor cursor = this->textCursor();
        // [0] 检测选中文字的属性是否相等
        checkTextProperty(cursor);

        if (nullptr != m_pItem->scene()) {
            auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
            curScene->updateBlurItem(m_pItem);
        }
        this->setFocus();
    }
}

void CTextEdit::solveHtml(QString &html)
{
    // [0] 截取html文件中的文本格式
    QStringList list;
    QString temp;
    for (int i = 0; i < html.length(); i++) {
        // 不需要考虑数组越界，返回的html文本是正确的
        if (html.at(i) == '<' && html.at(i + 1) == 's' && html.at(i + 2) == 'p' && html.at(i + 3) == 'a' && html.at(i + 4) == 'n') {
            temp.clear();
        }
        if (html.at(i) == '<' && html.at(i + 1) == '/' && html.at(i + 2) == 's' && html.at(i + 3) == 'p'
                && html.at(i + 4) == 'a' && html.at(i + 5) == 'n') {
            list.append(temp);
        }
        temp += html.at(i);
    }

    // [1] 剔除错误的字符格式并保存数据  得到的数据：<span style=\" font-family:'Unifont'; font-size:36pt; font-weight:456; color:#000000;\">文 本内容；
    m_allTextInfo.clear();
    for (int i = 0; i < list.size(); i++) {
        // [2] 将文本的格式和实际的字符串分割为两部分，避免解析格式的时候影响文本切割出错
        int len = list[i].indexOf("color:") + 16;
        QString formatString = list[i].left(len); // <span style=\" font-family:'Unifont'; font-size:36pt; font-weight:456; color:#000000;\">
        QString textString = list[i].mid(len);
        formatString = formatString.replace("<span style=", "");
        formatString = formatString.replace("\"", "");
        formatString = formatString.replace("'", "");
//        formatString = formatString.replace(" ", ""); // 不需要进行处理空格，否则family中间的空格会被取消导致文字字重属性刷新不全
        formatString = formatString.replace(">", "");
        // 默认的字重是不会在单个的字的描述中显示，因此需要手动加上一个默认属性
        if (!formatString.contains("font-weight:")) {
            formatString.insert(formatString.indexOf("color:"), "formatString:" + QString::number(
                                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().weight()) + ";");
        }
        formatString = formatString.replace("font-family:", "");
        formatString = formatString.replace("font-size:", "");
        formatString = formatString.replace("font-weight:", "");
        formatString = formatString.replace("color:", "");
        formatString = formatString.replace("pt", "");
//        qDebug() << "formatString: " << formatString;
        QStringList temp = formatString.split(";");

        // [3] 单个字单个字进行保存其对应的属性，方便后面进行属性比较（换行符号不作处理）
        if (temp.size() == 5) {
            for (int i = 0; i < textString.length(); i++) {
                QMap<ProperType, QVariant> data;
                data.insert(FontFamily, temp[0].trimmed());
                data.insert(PointSize, temp[1].trimmed());
                data.insert(FontStyle, temp[2].trimmed());
                data.insert(FontColor, temp[3].trimmed());
                QColor color(temp[3].trimmed());
                data.insert(ColorAlpha, color.alpha());
                data.insert(Text, textString.at(i));
                m_allTextInfo.append(data);
            }
//            qDebug() << "m_allTextInfo: " << m_allTextInfo;
        }
    }
}

void CTextEdit::checkTextProperty(const QTextCursor &cursor)
{
    // [0] 获取鼠标选中的文本段
    QTextBlock block = cursor.block();

    // [1] 判断是否有效
//    if (block.isValid()) {

    // [2] 判断是否只是点击进去未选中文字
//    QTextBlock::iterator it = block.begin();
//    if (it == block.end()) {
//        return;
//    }
    // [3] 解析当前所有的文本属性信息
    QString html = block.document()->toHtml();
    solveHtml(html);

    // [4] 找到选中文本段的索引
    QString allString = this->document()->toPlainText();
    int selected_start_index = cursor.selectionStart();
    int temp_index = selected_start_index;
    for (int i = 0; i < allString.length(); i++) {
        if (i > selected_start_index) {
            break;
        }

        if (allString.at(i) == '\n') {
            temp_index--;
        }
    }
    selected_start_index = temp_index;

    // [5] 获取鼠标选择的文本并且剔除段落换行符号
    QString selectedString = cursor.selectedText();
    if (selectedString.isEmpty()) { // 如果为空则返回当前光标出的属性
        QTextCharFormat fmt = this->currentCharFormat();
        m_selectedColor = fmt.foreground().color();
        m_selectedSize = fmt.font().pointSize();
        m_selectedFamily = fmt.fontFamily();
        m_selectedFontWeight = fmt.fontWeight();
        m_selectedColorAlpha = m_selectedColor.alpha();
        return;
//        更新整体属性
//        selectedString = allString;
//        selected_start_index = 0;
    }
    QString temp_str;
    for (int i = 0; i < selectedString.length(); i++) {
        if (selectedString.at(i) == "\u2029") {//删除段落符号
            continue;
        }
        temp_str += selectedString.at(i);
    }
    selectedString = temp_str;

    // [6] 剔除换行符号
    selectedString = selectedString.replace("\n", "");

    // [7] 判断选中的文本属性是否相同
    m_selectedColor = QColor();
    m_selectedSize = -1;
    m_selectedFamily.clear();
    m_selectedFontStyle.clear();
    m_selectedFontWeight = -1;
    m_selectedColorAlpha = -1;

    for (int i = selected_start_index, j = 0; i < m_allTextInfo.size(); i++) {
        // 如果匹配到当前第一个字符
        if (selectedString.at(j) == m_allTextInfo.at(i).value(Text).toString()) {

            QColor color = QColor(m_allTextInfo.at(i).value(FontColor).value<QColor>());
            int pointSize = m_allTextInfo.at(i).value(PointSize).toInt();
            QString family = m_allTextInfo.at(i).value(FontFamily).toString();
            int fontWeight = m_allTextInfo.at(i).value(FontStyle).toInt() / 8; // 根据查找Qt资料发现此处获取的大小是Qt自带自重大小的8倍
            int alpha = m_allTextInfo.at(i).value(ColorAlpha).toInt();

            if (0 == j) {
                m_selectedColor = color;
                m_selectedSize = pointSize;
                m_selectedFamily = family;
                m_selectedFontWeight = fontWeight;
                m_selectedColorAlpha = alpha;
                j++;
                if (j >= selectedString.length()) {
                    break;
                }
                continue;
            }
            if (m_selectedColor.isValid() && m_selectedColor != color) {
                m_selectedColor = QColor();
            }

            if (m_selectedSize >= 0 && m_selectedSize != pointSize) {
                m_selectedSize = -1;
            }

            if (!m_selectedFamily.isEmpty() && m_selectedFamily != family) {
                m_selectedFamily.clear();
            }

            if (m_selectedFontWeight >= 0 && m_selectedFontWeight != fontWeight) {
                m_selectedFontWeight = -1;
            }

            if (m_selectedColorAlpha >= 0 && m_selectedColorAlpha != alpha) {
                m_selectedColorAlpha = -1;
            }

            // 当所有的都不相同时跳出循环
            if (!m_selectedSize && m_selectedColor.isValid() && m_selectedFamily.isEmpty()
                    && m_selectedFontStyle.isEmpty() && !m_selectedColorAlpha) {
                break;
            }

            j++;
            if (j >= selectedString.length()) {
                break;
            }
        }

    }
    qDebug() << "selected_start_index: " << selected_start_index;
    qDebug() << "      selectedString: " << selectedString;
    qDebug() << "     m_selectedColor: " << m_selectedColor;
    qDebug() << "      m_selectedSize: " << m_selectedSize;
    qDebug() << "    m_selectedFamily: " << m_selectedFamily;
    qDebug() << "m_selectedFontWeight: " << m_selectedFontWeight;
    qDebug() << "m_selectedColorAlpha: " << m_selectedColorAlpha;
}

void CTextEdit::checkTextProperty()
{
    checkTextProperty(this->textCursor());
}

void CTextEdit::setVisible(bool visible)
{

    QTextEdit::setVisible(visible);
    if (!visible) {
        QTextCursor cursor = this->textCursor();
        cursor.select(QTextCursor::Document);
        this->setTextCursor(cursor);
        if (m_pItem != nullptr && nullptr != m_pItem->scene()) {
            auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
            curScene->updateBlurItem(m_pItem);
        }
    }
}

void CTextEdit::setLastDocumentWidth(qreal width)
{
    m_widthF = width;
}

void CTextEdit::resizeDocument()
{
    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    //rect.setHeight(size.height());

    if (m_pItem != nullptr) {
        m_pItem->setRect(rect);
    }

    m_widthF = rect.width();
}

QColor CTextEdit::getSelectedTextColor()
{
    return m_selectedColor;
}

int CTextEdit::getSelectedFontSize()
{
    return m_selectedSize;
}

QString CTextEdit::getSelectedFontFamily()
{
    return m_selectedFamily;
}

QString CTextEdit::getSelectedFontStyle()
{
    //    QFont::Thin(Regular)    0   QFont::ExtraLight 12  QFont::Light 25
    //    QFont::Normal  50  QFont::Medium     57  QFont::DemiBold 63
    //    QFont::Bold    75  QFont::ExtraBold  81  QFont::Black 87
    switch (m_selectedFontWeight) {
    case 0: {
        m_selectedFontStyle = QObject::tr("Regular");
        break;
    }
    case 12: {
        m_selectedFontStyle = QObject::tr("ExtraLight");
        break;
    }
    case 25: {
        m_selectedFontStyle = QObject::tr("Light");
        break;
    }
    case 50: {
        m_selectedFontStyle = QObject::tr("Normal");
        break;
    }
    case 57: {
        m_selectedFontStyle = QObject::tr("Medium");
        break;
    }
    case 63: {
        m_selectedFontStyle = QObject::tr("DemiBold");
        break;
    }
    case 75: {
        m_selectedFontStyle = QObject::tr("Bold");
        break;
    }
    case 81: {
        m_selectedFontStyle = QObject::tr("ExtraBold");
        break;
    }
    case 87: {
        m_selectedFontStyle = QObject::tr("Black");
        break;
    }
    }
    return m_selectedFontStyle;
}

int CTextEdit::getSelectedFontWeight()
{
    return m_selectedFontWeight;
}

int CTextEdit::getSelectedTextColorAlpha()
{
    return m_selectedColorAlpha;
}



