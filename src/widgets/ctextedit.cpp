/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "application.h"
#include "mainwindow.h"

#include <QGraphicsProxyWidget>
#include <DMenu>
#include <DApplication>
#include <QAction>
#include <QDebug>
#include <QTextBlock>
#include <QTextEdit>
#include <malloc.h>
#include <QMimeData>
#include <QComboBox>
#include <QScrollBar>
#include <QTimer>

static QTextCharFormat::Property senseProerties[] = {QTextCharFormat::ForegroundBrush,
                                                     QTextCharFormat::FontFamily,
                                                     QTextCharFormat::FontPointSize,
                                                     QTextCharFormat::FontWeight
                                                    };
static int senseProertiesCount = sizeof(senseProerties) / sizeof(QTextCharFormat::ForegroundBrush);

CTextEdit::CTextEdit(CGraphicsTextItem *item, QWidget *parent)
    : QTextEdit(parent)
    , m_pItem(item)
{
    //初始化字体
    connect(this, &CTextEdit::textChanged, this, &CTextEdit::onTextChanged);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged())/*, Qt::QueuedConnection*/);

    connect(this, &CTextEdit::selectionChanged, this, &CTextEdit::onSelectionChanged/*, Qt::QueuedConnection*/);
    connect(this, &CTextEdit::currentCharFormatChanged, this, &CTextEdit::onCurrentCharFormatChanged/*, Qt::QueuedConnection*/);

    this->setLineWrapMode(NoWrap);
    this->setFrameStyle(NoFrame);

    this->setLineWidth(0);

    updateBgColorTo(QColor(255, 255, 255), true);
}

CTextEdit::~CTextEdit()
{
    m_pItem = nullptr;
}

QTextCharFormat CTextEdit::currentFormat(bool considerSelection)
{
    if (considerSelection) {
        //如果考虑选中情况,那么当不处于编辑状态时,需要先行选中然后再获取字符样式
        if (!this->textCursor().hasSelection()) {
            if (m_pItem != nullptr && m_pItem->textState() == CGraphicsTextItem::EReadOnly)
                this->selectAll();
        }
        if (this->textCursor().hasSelection()) {
            updateSelectionFormat();
            return _selectionFmt;
        }
    }
    return currentCharFormat();
}

void CTextEdit::setCurrentFormat(const QTextCharFormat &format, bool merge)
{
    //如果修改到了0位置的格式，那么要改变默认格式
    static bool block = false;
    if (block)
        return;

    merge ? mergeCurrentCharFormat(format) : setCurrentCharFormat(format);
    if (!merge) {
        this->textCursor().joinPreviousEditBlock();
        block = true;
        bool setDefault = false;
        if (textCursor().hasSelection()) {
            auto selectionStart = textCursor().selectionStart();
            auto selectionEnd = textCursor().selectionEnd();
            if (0 == selectionStart || selectionEnd == 0) {
                setDefault = true;
            }
        } else {
            if (textCursor().position() == 0) {
                setDefault = true;
            }
        }
        if (setDefault) {
            setDefaultFormat(currentCharFormat());
        }
        block = false;
        this->textCursor().endEditBlock();
    }
}

QColor CTextEdit::currentColor()
{
    auto fmt = currentFormat();
    if (!fmt.hasProperty(QTextCharFormat::ForegroundBrush)) {
        return QColor();
    }
    return fmt.foreground().color();
}

void CTextEdit::setCurrentColor(const QColor &color)
{
    QTextCharFormat fmt;
    fmt.setForeground(color);
    setCurrentFormat(fmt, true);
}

QFont CTextEdit::currentFont(bool considerSelection)
{
    return currentFormat(considerSelection).font();
}

void CTextEdit::setCurrentFont(const QFont &ft)
{
    QTextCharFormat fmt;
    fmt.setFont(ft);
    setCurrentFormat(fmt, true);
}

int CTextEdit::currentFontSize()
{
    return qRound(currentFormat().fontPointSize());
}

void CTextEdit::setCurrentFontSize(const int sz)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(sz);
    setCurrentFormat(fmt, true);
}

QString CTextEdit::currentFontFamily()
{
    return currentFormat().fontFamily();
}

void CTextEdit::setCurrentFontFamily(const QString &family)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(family);
    setCurrentFormat(fmt, true);
}

QString CTextEdit::currentFontStyle()
{
    auto fmt = currentFormat();
    int w = fmt.hasProperty(QTextCharFormat::FontWeight) ? fmt.intProperty(QTextCharFormat::FontWeight) : -1;
    return toStyle(w);
}

void CTextEdit::setCurrentFontStyle(const QString &style)
{
    QTextCharFormat fmt;
    int wt = toWeight(style);
    fmt.setFontWeight(wt);
    setCurrentFormat(fmt, true);
}

void CTextEdit::onTextChanged()
{
    if (m_pItem == nullptr)
        return;

    // 如果是两点的状态高度需要自适应
    if (m_pItem->isAutoAdjustSize()) {
        QSizeF size = this->document()->size();
        QRectF rect = m_pItem->rect();
        rect.setHeight(size.height());
        rect.setWidth(size.width());
        m_pItem->setRect(rect);
    } else {
        //自动换行时,如果当前的文字字体太大那么输入的文字可能显示不到,我们就设置
        int ftHeight = QFontMetrics(this->currentFont(false)).lineSpacing();
        if (this->height() < ftHeight) {
            auto curRect = m_pItem->rect();
            curRect.setHeight(ftHeight);
            QMetaObject::invokeMethod(this, [ = ]() {
                m_pItem->setRect(curRect);
            }, Qt::QueuedConnection);
        }
    }

}

void CTextEdit::onCursorPositionChanged()
{
    if (!this->textCursor().hasSelection())
        updatePropertyWidget();
}

void CTextEdit::onSelectionChanged()
{
    static int s_preSelectionBeginPos = -1;
    static int s_preSelectionEndPos = -1;

    int newBegin = textCursor().selectionStart();
    int newEnd = textCursor().selectionEnd();
    if (s_preSelectionBeginPos !=  newBegin ||
            s_preSelectionEndPos != newEnd) {
        s_preSelectionBeginPos = newBegin;
        s_preSelectionEndPos = newEnd;
        updatePropertyWidget();
    }
}

void CTextEdit::onCurrentCharFormatChanged(const QTextCharFormat &format)
{
    Q_UNUSED(format)
    //if (!textCursor().hasSelection())  //使用控件自身刷新
    updatePropertyWidget();
}

void CTextEdit::updatePropertyWidget()
{
    if (_updateTimer == nullptr) {
        _updateTimer = new QTimer(this);
        _updateTimer->setSingleShot(true);
        connect(_updateTimer, &QTimer::timeout, this, [ = ]() {
            // 刷新属性
            if (m_pItem->drawScene() != nullptr) {
                m_pItem->drawScene()->pageContext()->update();
            }
        });
    }
    _updateTimer->start(70);
}

void CTextEdit::applyDefaultToFirstFormat()
{
    //BUG,if the text be empty, then the format will unexpected be clear,so there do that to
    setDefaultFormat(firstPosFormat());
}

void CTextEdit::insertFromMimeData(const QMimeData *source)
{
    if (source && source->hasText()) {
        this->insertPlainText(source->text());
        return;
    }
    QTextEdit::insertFromMimeData(source);
}

void CTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    //屏蔽自带的菜单
    e->accept();
}

void CTextEdit::keyPressEvent(QKeyEvent *event)
{
    //1.重载实现自定义的撤销还原快捷键
    if (event->modifiers() == Qt::CTRL) {
        if (event->key() == Qt::Key_Y) {
            this->redo();
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Z) {
            this->undo();
            event->accept();
            return;
        }
    }

    //2.屏蔽原来默认的重做快捷键
    if ((event->modifiers() & Qt::CTRL) && (event->modifiers()&Qt::SHIFT) && event->key() == Qt::Key_Z) {
        event->accept();
        return;
    }
    return QTextEdit::keyPressEvent(event);
}

void CTextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    if (!e->commitString().isEmpty()) {
        QTextEdit::inputMethodEvent(e);
//        //自动换行时,如果当前的文字字体太大那么输入的文字可能显示不到,我们就设置
//        if (m_pItem != nullptr && !m_pItem->isAutoAdjustSize()) {

//            //行高
//            int ftHeight = QFontMetrics(this->currentFont(false)).lineSpacing();

//            if (this->height() < ftHeight) {
//                auto curRect = m_pItem->rect();
//                curRect.setHeight(ftHeight);
//                QMetaObject::invokeMethod(this, [ = ]() {
//                    //int move = - ftHeight / 2;
//                    //verticalScrollBar()->setValue(verticalScrollBar()->value() + move);
//                    m_pItem->setRect(curRect);
//                }, Qt::QueuedConnection);
//            }
//        }
    }
}

void CTextEdit::focusOutEvent(QFocusEvent *e)
{
    QTextEdit::focusOutEvent(e);
}

void CTextEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTextEdit::mouseDoubleClickEvent(event);
}

void CTextEdit::mousePressEvent(QMouseEvent *event)
{
    QTextEdit::mousePressEvent(event);
    if (m_pItem != nullptr) {
        m_pItem->toFocusEiditor();
    }
}

QVector<QTextLayout::FormatRange> CTextEdit::getCharFormats(int posBegin, int posEnd)
{
    QVector<QTextLayout::FormatRange> fmts;
    int beginPos = posBegin;
    int endPos = posEnd;
    auto beginBlk = document()->findBlock(beginPos);
    auto endBlk   = document()->findBlock(endPos).next();
    for (auto itBk = beginBlk; itBk != endBlk; itBk = itBk.next()) {
        auto fmtVector = itBk.textFormats();
        for (auto fmt : fmtVector) {
            int spanBeginPosInDoc = itBk.position() + fmt.start;
            int spanEndPosInDoc  = spanBeginPosInDoc + fmt.length - 1;

            if (spanEndPosInDoc < beginPos || spanBeginPosInDoc > endPos) {
                continue;
            }
            //qDebug() << "s b = " << beginPos << "e b = " << endPos << "span b = " << spanBeginPosInDoc << "span e = " << spanEndPosInDoc;
            int newStart = qMax(spanBeginPosInDoc, beginPos);
            int newEnd   = qMin(spanEndPosInDoc, endPos);
            fmt.start  = newStart;
            fmt.length = newEnd - newStart + 1;

            fmts.append(fmt);
        }
    }
    //qDebug() << "fmts count ======= " << fmts.count();
    return fmts;
}

void CTextEdit::updateSelectionFormat()
{
    QTextCursor c = textCursor();
    int beginPos = qMin(c.selectionStart(), c.selectionEnd());
    int endPos = qMax(c.selectionStart(), c.selectionEnd()) - 1;  //光标的首位(pos == 0)位于第一个字符之前,所以其结尾位置需要-1以和字符在doc中的索引保持同步
    auto fmts = getCharFormats(beginPos, endPos);

    //格式数据是否有冲突
    QTextCharFormat fmt = fmts.first().format;
    for (int i = 1; i < fmts.count(); ++i) {
        QTextCharFormat ft = fmts.at(i).format;
        for (int j = 0; j < senseProertiesCount; ++j) {
            QTextCharFormat::Property property = senseProerties[j];
            if (fmt.hasProperty(property)) {
                if (fmt.property(property) != ft.property(property)) {
                    fmt.clearProperty(property);
                }
            }
        }
    }
    _selectionFmt = fmt;
}

int CTextEdit::toWeight(const QString &styleName)
{
    int weight = 0;
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

QString CTextEdit::toStyle(const int &weight)
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

QTextCharFormat CTextEdit::firstPosFormat() const
{
    if (!document()->isEmpty()) {
        return document()->firstBlock().textFormats().first().format;
    }
    return currentCharFormat();
}

void CTextEdit::setDefaultFormat(const QTextCharFormat &format)
{
    _defaultFormat = format;
    QSignalBlocker bloker(this);
    QTextCharFormat fmt = _defaultFormat;
    setCurrentFormat(fmt);
    textCursor().mergeBlockCharFormat(fmt);
    this->setAlignment(this->alignment());
    textCursor().setPosition(0);
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
