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
#include "mainwindow.h"

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

    // 设置文本默认的字体格式,设置默认的字体类型为思源宋黑体，没有该字体则选择系统第一个默认字体
    QFontDatabase fontbase;
    QString sourceHumFont = QObject::tr("Source Han Sans CN");
    if (!fontbase.families().contains(sourceHumFont)) {
        sourceHumFont = fontbase.families().first();
    }
    QFont font;
    font.setFamily(sourceHumFont);
    font.setStyleName("Regular");
    font.setPointSize(14);
    this->document()->setDefaultFont(font);
}

CTextEdit::~CTextEdit()
{
    m_pItem = nullptr;
}

void CTextEdit::slot_textChanged()
{
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
    // [0] 文本被全部删除的时候首先响应位置改变信号
    if (this->document()->toPlainText().isEmpty()) {
        this->blockSignals(true);
        this->setCurrentCharFormat(getCacheCharFormat());
        this->blockSignals(false);
    }

    // [1] 第一个段落无任何文字的时候重置属性（解决50649）
    if (this->document()->firstBlock().text().isEmpty()) {
        this->textCursor().movePosition(QTextCursor::Start);
        this->blockSignals(true);
        this->setCurrentCharFormat(getCacheCharFormat());
        this->blockSignals(false);
    }

    // [2] 初始化默认属性
    m_selectedColor = QColor();
    m_selectedSize = -1;
    m_selectedFamily.clear();
    m_selectedFontweight = -1;
    m_selectedColorAlpha = -1;

    int start_index = textCursor().selectionStart();
    int end_index = textCursor().selectionEnd();

    // 没有选中任何的文本
    if (start_index == end_index) {
        updateCurrentCursorProperty();
    } else {
        QTextBlock startblock = this->document()->firstBlock();
        QTextBlock endblock = this->document()->findBlock(end_index);

        int pos = -1;
        bool findedStart = false;

        while (startblock.isValid()) {
            pos++;
            if (pos == start_index) {
                findedStart = true;
            }
            if (pos == end_index) {
                break;
            }

            // 当前行是回车行
            if (startblock.text().isEmpty() && findedStart) {
                // 空行处的属性不应该参与到实际文字中进行属性比较
            } else {
                for (QTextBlock::iterator it = startblock.begin(); !it.atEnd(); it++) {
                    QTextFragment fragment = it.fragment();
                    QTextCharFormat charFmt = fragment.charFormat();

                    for (int i = 0; i < fragment.text().length(); i++) {
                        if (pos == start_index) {
                            findedStart = true;
                            m_selectedColor = charFmt.foreground().color();
                            m_selectedColorAlpha = charFmt.foreground().color().alpha();
                            m_selectedFamily = charFmt.fontFamily();
                            m_selectedFontweight = charFmt.fontWeight();
                            m_selectedSize = charFmt.fontPointSize();
                        }
                        if (pos == end_index) {
                            break;
                        }
                        pos++;
                    }

                    // 比较属性，有可能选择的文字只是颜色不同，但是其它属性相同
                    if (m_selectedColor.isValid() && m_selectedColor != charFmt.foreground().color()) {
                        m_selectedColor = QColor();
                    }
                    if (m_selectedSize >= 0 && m_selectedSize != charFmt.fontPointSize()) {
                        m_selectedSize = -1;
                    }
                    if (!m_selectedFamily.isEmpty() && m_selectedFamily != charFmt.fontFamily()) {
                        m_selectedFamily.clear();
                    }
                    if (m_selectedFontweight >= 0 && m_selectedFontweight != charFmt.fontWeight()) {
                        m_selectedFontweight = -1;
                    }
                    if (m_selectedColorAlpha >= 0 && m_selectedColorAlpha != charFmt.foreground().color().alpha()) {
                        m_selectedColorAlpha = -1;
                    }
                    // 当所有的都不相同时跳出循环
                    if (!m_selectedSize && m_selectedColor.isValid() && m_selectedFamily.isEmpty()
                            && !m_selectedFontweight && !m_selectedColorAlpha) {
                        // 所有属性都不相等，直接让pos == end_index，便于跳出循环
                        pos = end_index;
                        break;
                    }

                    if (pos == end_index) {
                        break;
                    }
                }
            }
            if (pos == end_index) {
                break;
            }
            if (startblock == endblock) {
                break;
            }
            startblock = startblock.next();
        }
    }

//    qDebug() << "     m_selectedColor: " << m_selectedColor;
//    qDebug() << "      m_selectedSize: " << m_selectedSize;
//    qDebug() << "    m_selectedFamily: " << m_selectedFamily;
//    qDebug() << "m_selectedFontweight: " << m_selectedFontweight;
//    qDebug() << "m_selectedColorAlpha: " << m_selectedColorAlpha;

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

void CTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::CTRL) {
        if (event->key() == Qt::Key_Y) {
            this->redo();
            event->accept();
            return;
        }
    }
    if ((event->modifiers() & Qt::CTRL) && (event->modifiers()&Qt::SHIFT) && event->key() == Qt::Key_Z) {
        event->accept();
        return;
    }
    return QTextEdit::keyPressEvent(event);
}

void CTextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    m_e = *e;
    QTextEdit::inputMethodEvent(e);
}

void CTextEdit::focusOutEvent(QFocusEvent *e)
{
    QString &pre = const_cast<QString &>(m_e.preeditString());

//    qDebug() << "m_e.preeditString():" << m_e.preeditString();
//    qDebug() << "m_e.commitString():" << m_e.commitString();


    if (!pre.isEmpty()) {
        //保证隐藏输入框
        dApp->topMainWindow()->setFocus();
    }

    qDebug() << "new focus object = " << dApp->focusObject() << "is same = "
             << (dApp->focusObject() == this)
             << "parent = " << this->parent()
             << "active widget = " << dApp->activePopupWidget();

    //字体下拉菜单的属性修改(如字体族,字体style)导致的焦点丢失不应该响应
    if (dApp->focusObject() == this || dApp->activePopupWidget() != nullptr) {
        qDebug() << "return dApp->focusObject() = " << dApp->focusObject() << "dApp->activePopupWidget() = " << dApp->activePopupWidget();
        //m_pItem->makeEditabel(false);

        //保证按键响应到textedit控件的底层(从而才能将key事件传递给textedit)
        if (m_pItem->curView() != nullptr) {
            m_pItem->curView()->setFocus();
        }

        //保证自身的焦点
        setFocus();
        return;
    }

    //焦点移动到了改变字体大小的combox上(准确点其实应该判断那个控件的指针),要隐藏光标,大小修改完成后再显示(参见字体修改后的makeEditabel)
    if (qobject_cast<QComboBox *>(dApp->focusObject()) != nullptr) {
        this->setTextInteractionFlags(this->textInteractionFlags() & (~Qt::TextEditable));
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
}

void CTextEdit::updateCurrentCursorProperty()
{
    QTextCharFormat fmt = this->currentCharFormat();
    m_selectedColor = fmt.foreground().color();
    m_selectedSize = fmt.font().pointSize();
    m_selectedFamily = fmt.fontFamily();
    m_selectedColorAlpha = m_selectedColor.alpha();
    m_selectedFontweight = fmt.fontWeight();
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

QString CTextEdit::getFontStyleByWeight(const int &weight)
{
    switch (weight) {
    case 0: return "Thin";
    case 12: return "ExtraLight";
    case 25: return "Light";
    case 50: return "Regular";
    case 57: return "Medium";
    case 63: return "DemiBold";
    case 75: return "Bold";
    case 81: return "ExtraBold";
    case 87: return "Black";
    default: return "";
    }
}

QTextCharFormat CTextEdit::getCacheCharFormat()
{
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
    return fmt;
}

void CTextEdit::checkTextProperty()
{
    cursorPositionChanged();
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
    return getFontStyleByWeight(m_selectedFontweight);
}

int CTextEdit::getSelectedTextColorAlpha()
{
    return m_selectedColorAlpha;
}
