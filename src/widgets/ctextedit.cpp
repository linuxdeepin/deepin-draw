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

#include "service/cmanagerattributeservice.h"

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
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()), Qt::QueuedConnection);

    connect(this, &CTextEdit::selectionChanged, this, &CTextEdit::onSelectionChanged, Qt::QueuedConnection);
    connect(this, &CTextEdit::currentCharFormatChanged, this, &CTextEdit::onCurrentCharFormatChanged, Qt::QueuedConnection);

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

QTextCharFormat CTextEdit::currentFormat(bool considerSelection)
{
    if (considerSelection && this->textCursor().hasSelection()) {
        updateSelectionFormat();
        return _selectionFmt;
    }
    return currentCharFormat();
}

void CTextEdit::setCurrentFormat(const QTextCharFormat &format, bool merge)
{
    if (textCursor().hasSelection()) {
        //同时设置默认的块的字体格式
        textCursor().mergeBlockCharFormat(format);
    }
    merge ? mergeCurrentCharFormat(format) : setCurrentCharFormat(format);
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
    fmt.setFontWeight(toWeight(style));
    setCurrentFormat(fmt, true);
}

void CTextEdit::onTextChanged()
{
    // 如果是两点的状态高度需要自适应
    if (m_pItem->isAutoAdjustSize()) {
        QSizeF size = this->document()->size();
        QRectF rect = m_pItem->rect();
        rect.setHeight(size.height());
        rect.setWidth(size.width());
        m_pItem->setRect(rect);
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
    if (!textCursor().hasSelection())
        updatePropertyWidget();
}

void CTextEdit::updatePropertyWidget()
{
    // 刷新属性
    if (m_pItem->drawScene() != nullptr) {
        m_pItem->drawScene()->selectGroup()->updateAttributes();
    }
}

void CTextEdit::insertFromMimeData(const QMimeData *source)
{
    if (source && source->hasText()) {
        this->insertPlainText(source->text());
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
    onTextChanged();
    return QTextEdit::keyPressEvent(event);
}

void CTextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    QTextEdit::inputMethodEvent(e);
}

void CTextEdit::focusOutEvent(QFocusEvent *e)
{
    QTextEdit::focusOutEvent(e);
}

void CTextEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTextEdit::mouseDoubleClickEvent(event);
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
