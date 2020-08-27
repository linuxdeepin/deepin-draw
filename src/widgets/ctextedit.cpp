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
#include "bzItems/cgraphicstextitem.h"
#include "drawshape/cdrawscene.h"
#include "cgraphicsitemselectedmgr.h"
#include "bzItems/cgraphicsmasicoitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "application.h"

#include <DMenu>
#include <DApplication>
#include <QAction>
#include <QDebug>
#include <QTextBlock>
#include <QTextEdit>
#include <malloc.h>
#include <QMimeData>
#include <QComboBox>

#include "service/cmanagerattributeservice.h"

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

    updateBgColorTo(QColor(255, 255, 255), true);
}

CTextEdit::~CTextEdit()
{
    m_pItem = nullptr;
}

void CTextEdit::slot_textChanged()
{
    QString html = this->toHtml();
    QString spanStyle = "<span style=\" font-family";
    // 文本删除完后重新写入文字需要重置属性,删除完后预览中文需要进行设置
    if (this->document()->toPlainText().isEmpty()) {
        QFont font;
        font = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont();
        quint8 weight = getFontWeigthByStyleName(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFontStyle());

        QTextCharFormat fmt;
        fmt.setFont(font);
        // 字体的字重需要通过这种方式设置才会生效
        fmt.setFontWeight(weight);
        QColor color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
        color.setAlpha(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColorAlpha());
        fmt.setForeground(color);
        this->blockSignals(true);
        this->setCurrentCharFormat(fmt);
        this->blockSignals(false);
    } else if (!html.contains(spanStyle)) {
        // [40550] 百度输入法输入的时候是一次提交所有，没有预览的效果，就不会触发文本为空的时候
        // 所以在这里需要添加单独的判断
        QFont font;
        font = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont();
        quint8 weight = getFontWeigthByStyleName(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFontStyle());

        QTextCharFormat fmt;
        fmt.setFont(font);
        // 字体的字重需要通过这种方式设置才会生效
        fmt.setFontWeight(weight);
        QColor color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
        color.setAlpha(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColorAlpha());
        fmt.setForeground(color);
        this->blockSignals(true);
        this->textCursor().setBlockCharFormat(fmt);
        this->blockSignals(false);
    }

    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    // 如果是两点的状态高度需要自适应
    if (!m_pItem->getManResizeFlag()) {
        QSizeF size = this->document()->size();
        QRectF rect = m_pItem->rect();
        rect.setHeight(size.height());
        rect.setWidth(size.width());
        m_pItem->setRect(rect);
    }

    if (nullptr != m_pItem->scene()) {
        auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
        //更新字图元
        curScene->updateBlurItem(m_pItem);
    }
}

void CTextEdit::cursorPositionChanged()
{
    // [0] 当删除所有文字后，格式会被重置为默认的属性，需要从缓存中重新更新格式
    if (this->document()->toPlainText().isEmpty()) {
        updatePropertyCache2Cursor();
    }
    // [1] 光标位置在最前面 或者 开始是\n字符 的时候需要更新当前光标处的属性
    else if (this->textCursor().selectionStart() == 0
             && (this->document()->toPlainText().startsWith("\n"))) {
        updateCurrentCursorProperty();
    } else {
        // [2] 检测选中文字的属性是否相等
        QTextCursor cursor = this->textCursor();
        checkTextProperty(cursor);
    }

    // [3] 刷新属性到顶部文字菜单控件中
    if (m_pItem->drawScene() != nullptr) {
        m_pItem->drawScene()->getItemsMgr()->updateAttributes();
    }
}

void CTextEdit::insertFromMimeData(const QMimeData *source)
{
    if (source && source->hasText()) {
        this->insertPlainText(source->text());
    }
}

void CTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    //屏蔽自带的菜单
    e->accept();
}

//void CTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
//{
//    qDebug() << "CTextEdit::mouseDoubleClickEvent+++++++++++++++++++++++++";
//    return QTextEdit::mouseDoubleClickEvent(e);
//}

//void CTextEdit::mousePressEvent(QMouseEvent *event)
//{
//    qDebug() << "CTextEdit::mousePressEvent------------------------------";
//    return QTextEdit::mousePressEvent(event);
//}

//void CTextEdit::mouseMoveEvent(QMouseEvent *event)
//{
////    qDebug() << "CTextEdit::mouseMoveEvent----";
//    return QTextEdit::mouseMoveEvent(event);
//}

void CTextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    m_e = *e;
    QTextEdit::inputMethodEvent(e);
}

void CTextEdit::focusOutEvent(QFocusEvent *e)
{
    QString &pre = const_cast<QString &>(m_e.preeditString());
    if (!pre.isEmpty()) {
        QTextEdit::focusOutEvent(e);
        m_e.setCommitString(pre);
        pre.clear();
        inputMethodEvent(&m_e);
    }

//    qDebug() << "new focus object = " << dApp->focusObject() << "is same = "
//             << (dApp->focusObject() == this)
//             << "parent = " << this->parent()
//             << "active widget = " << dApp->activePopupWidget();

    //字体下拉菜单的属性修改(如字体族,字体style)导致的焦点丢失不应该响应
    if (dApp->focusObject() == this || dApp->activePopupWidget() != nullptr) {
        qDebug() << "return dApp->focusObject() = " << dApp->focusObject() << "dApp->activePopupWidget() = " << dApp->activePopupWidget();
        return;
    }

    //焦点移动到了改变字体大小的combox上(准确点其实应该判断那个控件的指针),要隐藏光标,大小修改完成后再显示(参见字体修改后的makeEditabel)
    if (qobject_cast<QComboBox *>(dApp->focusObject()) != nullptr) {
        this->setReadOnly(true);  //隐藏光标
        return;
    }

    qDebug() << "CTextEdit::focusOutEvent ----- ";

    QTextEdit::focusOutEvent(e);

    if (m_pItem && m_pItem->drawScene()) {
        // 需要全选所有文字便于外面单击图元的时候修改的是整体的属性
        this->selectAll();
        hide();
        m_pItem->drawScene()->notSelectItem(m_pItem);
    }

    m_editing = false;
}

void CTextEdit::focusInEvent(QFocusEvent *e)
{
    m_editing = true;
    QTextEdit::focusInEvent(e);
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

    // [1] 剔除错误的字符格式并保存数据  得到的数据：
    // <span style=\" font-family:'Bitstream Charter'; font-size:14pt; font-weight:0; color:#000000;\">输入文本
    // <span style=\" font-family:'Bitstream Charter'; font-size:14pt; font-weight:0; color:rgba(0,0,0,0.341176);\">输入文本

    // [2] 28155 解决输入中文后删除所有再次输入文字属性错误
    if (!list.size()) {
        QTextCharFormat fmt;
        fmt.setFontFamily(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family());
        fmt.setFontPointSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSize());
        fmt.setFontWeight(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().weight());
        QColor color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
        color.setAlpha(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColorAlpha());
        fmt.setForeground(color);
        this->setCurrentCharFormat(fmt);
        QSizeF size = this->document()->size();
        QRectF rect = m_pItem->rect();
        // 如果是两点的状态高度需要自适应
        if (!m_pItem->getManResizeFlag()) {
            QFontMetrics fm(fmt.font());
            rect.setHeight(fm.height() * 1.4);
        }
        rect.setWidth(size.width());
        return;
    }

    m_allTextInfo.clear();
    for (int i = 0; i < list.size(); i++) {
        // [2] 将文本的格式和实际的字符串分割为三部分【字体属性】【颜色】【文字】，避免解析格式的时候影响文本切割出错
        int len = list[i].indexOf(">");
        QString formatString = list[i].left(len);
        QString textString = list[i].mid(len + 1);

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
//        qDebug() << "formatString: " << formatString; // Bitstream Charter; 14; 0; #000000; / Bitstream Charter; 14; 0; rgba(0,0,0,0.482353);
        QStringList temp = formatString.split(";");
//        qDebug() << "temp: " << temp;
        // [3] 单个字单个字进行保存其对应的属性，方便后面进行属性比较（换行符号不作处理）
        if (temp.size() == 5) {
            for (int i = 0; i < textString.length(); i++) {
                QMap<ProperType, QVariant> data;
                data.insert(FontFamily, temp[0].trimmed());
                data.insert(PointSize, temp[1].trimmed());
                data.insert(FontStyle, temp[2].trimmed());

                // 颜色需要单独进行特殊处理
                temp[3] = temp[3].trimmed();
                if (temp[3].contains("rgba")) {
                    QString colorStr = temp[3];
                    colorStr = colorStr.replace("rgba", "");
                    colorStr = colorStr.replace("(", "");
                    colorStr = colorStr.replace(")", "");
                    QStringList rgba = colorStr.split(",");
                    QColor color;
                    color.setRed(rgba[0].toInt());
                    color.setGreen(rgba[1].toInt());
                    color.setBlue(rgba[2].toInt());
                    color.setAlpha(static_cast<int>(rgba[3].toFloat() * 255));
                    data.insert(FontColor, color);
                    data.insert(ColorAlpha, color.alpha());
                } else {
                    QColor color(temp[3].trimmed());
                    data.insert(FontColor, color);
                    data.insert(ColorAlpha, color.alpha());
                }

                data.insert(Text, textString.at(i));
                m_allTextInfo.append(data);
            }
//            qDebug() << "m_allTextInfo: " << m_allTextInfo;
        }
    }
}

void CTextEdit::updateCurrentCursorProperty()
{
    // note: 输入中文会显示到输入框中，但是获取的文本是空，要考虑这样的情况
    if (this->textCursor().selectionStart() == 0 && (this->document()->toPlainText().startsWith("\n") || this->document()->toPlainText().isEmpty())) {
        QTextCharFormat fmt;
        fmt.setFontFamily(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family());
        fmt.setFontPointSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSize());
        fmt.setFontWeight(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().weight());
        QColor color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
        color.setAlpha(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColorAlpha());
        fmt.setForeground(color);
        this->setCurrentCharFormat(fmt);
    }

    QTextCharFormat fmt = this->currentCharFormat();
    m_selectedColor = fmt.foreground().color();
    m_selectedSize = fmt.font().pointSize();
    m_selectedFamily = fmt.fontFamily();
    m_selectedFontWeight = fmt.fontWeight();
    m_selectedColorAlpha = m_selectedColor.alpha();
}

void CTextEdit::updatePropertyCache2Cursor()
{
    m_selectedColor = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
    m_selectedSize = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSize();
    m_selectedFamily = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family();
    m_selectedFontWeight = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().weight();
    m_selectedColorAlpha = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColorAlpha();
}

quint8 CTextEdit::getFontWeigthByStyleName(const QString &styleName)
{
    quint8 weight = 0;
    if (styleName == "Thin") {
        weight = 0;
    } else if (styleName == "ExtraLight") {
        weight = 12;
    } else if (styleName == "Light") {
        weight = 25;
    } else if (styleName == "Normal" || styleName == "Regular") {
        weight = 50;
    } else if (styleName == "Medium") {
        weight = 57;
    } else if (styleName == "DemiBold") {
        weight = 63;
    } else if (styleName == "Bold") {
        weight = 75;
    } else if (styleName == "ExtraBold") {
        weight = 81;
    } else if (styleName == "Black") {
        weight = 87;
    }
    return weight;
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

    // [7] 如果为空则返回当前光标出的属性
    if (selectedString.isEmpty()) {
        QTextCharFormat fmt = this->currentCharFormat();
        m_selectedColor = fmt.foreground().color();
        m_selectedSize = fmt.font().pointSize();
        m_selectedFamily = fmt.fontFamily();
        m_selectedFontWeight = fmt.fontWeight();
        m_selectedColorAlpha = m_selectedColor.alpha();
        return;
    }

    // [8] 判断选中的文本属性是否相同
    m_selectedColor = QColor();
    m_selectedSize = -1;
    m_selectedFamily.clear();
    m_selectedFontStyle.clear();
    m_selectedFontWeight = -1;
    m_selectedColorAlpha = -1;

    for (int i = selected_start_index, j = 0; i < m_allTextInfo.size() && i >= 0; i++) {
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
//    qDebug() << "selected_start_index: " << selected_start_index;
//    qDebug() << "      selectedString: " << selectedString;
//    qDebug() << "     m_selectedColor: " << m_selectedColor;
//    qDebug() << "      m_selectedSize: " << m_selectedSize;
//    qDebug() << "    m_selectedFamily: " << m_selectedFamily;
//    qDebug() << "m_selectedFontWeight: " << m_selectedFontWeight;
//    qDebug() << "m_selectedColorAlpha: " << m_selectedColorAlpha;
}

void CTextEdit::checkTextProperty()
{
    /*if (!this->textCursor().hasSelection() &&
            this->textCursor().selectionStart() == 0 &&
            this->textCursor().selectionEnd() == this->toPlainText().length() - 1) {
        this->selectAll();
        checkTextProperty(this->textCursor());
    } else */
    if (this->textCursor().hasSelection() && !this->toPlainText().startsWith("\n")) {
        checkTextProperty(this->textCursor());
    } else {
        updateCurrentCursorProperty();
    }
}

void CTextEdit::updateBgColorTo(const QColor c, bool laterDo)
{
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Base, c);
    if (laterDo) {
        QMetaObject::invokeMethod(this, [ = ]() {
            this->setPalette(palette);
        }, Qt::QueuedConnection);
    } else {
        this->setPalette(palette);
    }
}

bool CTextEdit::getEditing()
{
    return m_editing;
}

//void CTextEdit::setVisible(bool visible)
//{
//    QTextEdit::setVisible(visible);
//}

void CTextEdit::setLastDocumentWidth(qreal width)
{
    m_widthF = width;
}

void CTextEdit::resizeDocument()
{
    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QRectF rect = m_pItem->rect();

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
        // 为0的时候本应该是Thin，但是需要显示为 Regular
        m_selectedFontStyle = "Regular";
//        m_selectedFontStyle = QObject::tr("Thin");
        break;
    }
    case 12: {
        m_selectedFontStyle = "ExtraLight";
        break;
    }
    case 25: {
        m_selectedFontStyle = "Light";
        break;
    }
    case 50: {
        m_selectedFontStyle = "Regular";
        break;
    }
    case 57: {
        m_selectedFontStyle = "Medium";
        break;
    }
    case 63: {
        m_selectedFontStyle = "DemiBold";
        break;
    }
    case 75: {
        m_selectedFontStyle = "Bold";
        break;
    }
    case 81: {
        m_selectedFontStyle = "ExtraBold";
        break;
    }
    case 87: {
        m_selectedFontStyle = "Black";
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



