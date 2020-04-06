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
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cgraphicsitemhighlight.h"
#include "cgraphicsitem.h"
#include "widgets/ctextedit.h"
#include "cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <DApplication>

#include <QTextEdit>
#include <QPainter>
#include <QTextBlock>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QFont>
#include <QMenu>
#include <QDebug>
#include <QObject>
#include <QTextDocument>




CGraphicsTextItem::CGraphicsTextItem()
    : CGraphicsRectItem ()
    , m_pTextEdit(nullptr)
    , m_pProxy(nullptr)
    , m_bManResize(false)
{
    initTextEditWidget();
}

CGraphicsTextItem::CGraphicsTextItem(const SGraphicsTextUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem (data->rect, head, parent)
    , m_pTextEdit(nullptr)
    , m_pProxy(nullptr)
    , m_bManResize(false)
{
    initTextEditWidget();
    m_Font = data->font;
    m_bManResize = data->manResizeFlag;
    m_pTextEdit->setHtml(data->content);
    m_pTextEdit->hide();
    QRectF rect(data->rect.topLeft, data->rect.bottomRight);
    setRect(rect);
    m_pTextEdit->document()->clearUndoRedoStacks();
}

CGraphicsTextItem::~CGraphicsTextItem()
{

}

void CGraphicsTextItem::initTextEditWidget()
{
    m_pTextEdit = new CTextEdit(this);
    m_pTextEdit->setMinimumSize(QSize(1, 1));


    m_pTextEdit->setWindowFlags(Qt::FramelessWindowHint);
    m_pTextEdit->setFrameShape(QTextEdit::NoFrame);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pProxy = new CGraphicsProxyWidget(this);
    m_pProxy->setWidget(m_pTextEdit);
    m_pProxy->setMinimumSize(0, 0);
    m_pProxy->setZValue(this->zValue() - 0.1);
    m_pTextEdit->setFocus();

    enum EDirection { LeftTop, Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left, Rotation, InRect, None};

    this->setSizeHandleRectFlag(CSizeHandleRect::LeftTop, false);
    this->setSizeHandleRectFlag(CSizeHandleRect::Top, false);
    this->setSizeHandleRectFlag(CSizeHandleRect::RightTop, false);
    this->setSizeHandleRectFlag(CSizeHandleRect::RightBottom, false);
    this->setSizeHandleRectFlag(CSizeHandleRect::Bottom, false);
    this->setSizeHandleRectFlag(CSizeHandleRect::LeftBottom, false);
    this->setSizeHandleRectFlag(CSizeHandleRect::Rotation, false);

    QFont font = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont();
    m_Font = font;
    QColor color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();

    //全选会更改一次字体 所以字体获取要在这之前
    QTextCursor textCursor = m_pTextEdit->textCursor();
    textCursor.select(QTextCursor::Document);
    m_pTextEdit->setTextCursor(textCursor);

    //更新初始化的字体和颜色
    QTextCharFormat fmt;
    fmt.setFont(font);
    fmt.setForeground(color);
    this->mergeFormatOnWordOrSelection(fmt);
    this->currentCharFormatChanged(fmt);

    m_pTextEdit->show();
    m_pTextEdit->document()->clearUndoRedoStacks();
}

void CGraphicsTextItem::setLastDocumentWidth(qreal width)
{
    m_pTextEdit->setLastDocumentWidth(width);
}

QPainterPath CGraphicsTextItem::getHighLightPath()
{
    QPainterPath path;
    path.addRect(this->rect());
    return path;
}

bool CGraphicsTextItem::getAllTextColorIsEqual()
{
    //    // 必须要点击文本检验后才能获取是否所有颜色相同，否则返回所有文字相同
    //    bool temp = m_allColorIsEqual;
    //    m_allColorIsEqual = true;
    return m_allColorIsEqual;
}

void CGraphicsTextItem::slot_textmenu(QPoint)
{
    m_menu->move (cursor().pos());
    m_menu->show();
}


CTextEdit *CGraphicsTextItem::getTextEdit() const
{
    return m_pTextEdit;
}

int CGraphicsTextItem::type() const
{
    return TextType;
}

void CGraphicsTextItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    if (m_pProxy != nullptr) {
        m_pProxy->resize(rect.width(), rect.height());
    }
    CGraphicsRectItem::setRect(rect);
    updateWidget();

    if (static_cast<CDrawScene *>(scene()) && static_cast<CDrawScene *>(scene())->selectedItems().size() == 1) {
        CGraphicsItem *item = static_cast<CGraphicsItem *>(static_cast<CDrawScene *>(scene())->selectedItems().at(0));
        if (item && item->type() == TextType) {
            static_cast<CDrawScene *>(scene())->getItemHighLight()->setPos(QPoint(0, 0));
            static_cast<CDrawScene *>(scene())->getItemHighLight()->setPath(item->mapToScene(item->getHighLightPath()));
        }
    }
}

void CGraphicsTextItem::setCGraphicsProxyWidget(CGraphicsProxyWidget *proxy)
{
    m_pProxy = proxy;
}

CGraphicsProxyWidget *CGraphicsTextItem::getCGraphicsProxyWidget() const
{
    return m_pProxy;
}

void CGraphicsTextItem::updateWidget()
{
    const QRectF &geom = this->rect();
    bool flag = m_pTextEdit->isVisible();
    m_pTextEdit->show();
    //m_pTextEdit->resize(int(geom.width()), int(geom.height()));
    m_pProxy->setPos(geom.x(), geom.y());
    if (!flag) {
        m_pTextEdit->hide();
    }
}

void CGraphicsTextItem::setFont(const QFont &font)
{
    //设置当前选中字块的字体和新增的字体
    QTextCharFormat fmt;
    fmt.setFont(font);
    mergeFormatOnWordOrSelection(fmt);

    m_Font = font;
}

QFont CGraphicsTextItem::getFont()
{
    return m_Font;
}

void CGraphicsTextItem::setFontSize(qreal size)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(size);
    mergeFormatOnWordOrSelection(fmt);

    m_Font.setPointSizeF(size);

//    //只有把焦点设成这个  才可以输入文字
//    if (this->scene() != nullptr) {
//        this->scene()->views()[0]->setFocus();
    //    }
}

qreal CGraphicsTextItem::getFontSize()
{
    return m_Font.pointSizeF();
}

void CGraphicsTextItem::setFontFamily(const QString &family)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(family);
    mergeFormatOnWordOrSelection(fmt);
    m_Font.setFamily(family);

    //只有把焦点设成这个  才可以输入文字
    if (this->scene() != nullptr) {
        this->scene()->views()[0]->setFocus();
    }
}

void CGraphicsTextItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    CGraphicsRectItem::resizeTo(dir, point, false, false);
    m_bManResize = true;
    updateWidget();
    m_pTextEdit->resizeDocument();

}

void CGraphicsTextItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);
    static_cast<CGraphicsTextItem *>(item)->setManResizeFlag(this->m_bManResize);
    static_cast<CGraphicsTextItem *>(item)->getTextEdit()->setDocument(this->getTextEdit()->document()->clone(static_cast<CGraphicsTextItem *>(item)->getTextEdit()));
    static_cast<CGraphicsTextItem *>(item)->getCGraphicsProxyWidget()->hide();
}

void CGraphicsTextItem::setTextColor(const QColor &col)
{
    qDebug() << col;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);

    m_color = col;

    //只有把焦点设成这个  才可以输入文字
//    if (this->scene() != nullptr) {
//        this->scene()->views()[0]->setFocus();
    //    }
}

QColor CGraphicsTextItem::getTextColor()
{
    return m_color;
}

void CGraphicsTextItem::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = m_pTextEdit->textCursor();
//    if (!cursor.hasSelection())
//        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    m_pTextEdit->mergeCurrentCharFormat(format);
    m_pTextEdit->setFocus();
}

void CGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    updateGeometry();

    drawDocument(painter, m_pTextEdit->document(), this->rect());
    if (this->getMutiSelect()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale());
//        if ( CManageViewSigleton::GetInstance()->getThemeType() == 1) {
//            pen.setColor(QColor(224, 224, 224));
//        } else {
//            pen.setColor(QColor(69, 69, 69));
//        }
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}

void CGraphicsTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode() == selection ||
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode() == text) {
        m_pTextEdit->show();
        //m_pProxy->setFocus();
        QTextCursor textCursor = m_pTextEdit->textCursor();
        textCursor.select(QTextCursor::Document);
        m_pTextEdit->setTextCursor(textCursor);
//    m_pTextEdit->cursorPositionChanged();
    }

    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->updateBlurItem(this);
    }
    m_pTextEdit->setFocus();
}

void CGraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    m_pTextEdit->selectAll();

    QTextCursor cur = m_pTextEdit->textCursor();

    m_allColorIsEqual = true;

    QTextBlock block = cur.block();
    if (block.isValid()) {
        QTextBlock::iterator it;

        for (it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;

            if (fragment.charFormat().foreground().color() != m_color) {
                m_allColorIsEqual = false;
                return;
            }
        }
    }
    CGraphicsRectItem::mousePressEvent(event);
}

void CGraphicsTextItem::drawDocument(QPainter *painter,
                                     const QTextDocument *doc,
                                     const QRectF &r,
                                     const QBrush &brush)
{
    if (doc->isEmpty())
        return;

    painter->save();
    // 按区域绘制
    if (r.isValid()) {
        painter->setClipRect(r, Qt::IntersectClip);
    }
    painter->translate(r.topLeft());
    QTextDocument *t_doc = (QTextDocument *)doc;
    t_doc->drawContents(painter, QRectF());
    painter->restore();
}

//第二步 绘制 Layout
void CGraphicsTextItem::drawTextLayout(QPainter *painter, const QTextBlock &block, const QSizeF &sizeToFill, const QBrush &brush)
{
    if (!block.isValid())
        return;
    QTextLayout *layout = block.layout();
    if (!layout)
        return;
    if (layout->lineCount() < 1)
        return;

    // 基线，每绘制一行则Y下移一行的位置，X不变
    QPointF baseLine = layout->position();
    baseLine.setY(baseLine.y() + layout->lineAt(0).ascent());

    // 输出位置，以基线Pos为基础，往右移动，每次换行，重置X位置
    QPointF outPos = baseLine;

    // QTextLayout 下一级是 QTextBlock..
    // QTextBlock 的下一级又是QTextFragment..
    QTextBlock::iterator it;
    for (it = block.begin(); !(it.atEnd()); ++it) {
        QTextFragment fragment = it.fragment();
        if (!fragment.isValid())
            continue;
        // 获取文本
        QString text = fragment.text();
        if (text.isEmpty())
            continue;
        // 获取文本格式
        QTextCharFormat chf = fragment.charFormat();
        int lineNoBegin = layout->lineForTextPosition(fragment.position() - block.position()).lineNumber();
        int lineNoEnd = layout->lineForTextPosition(fragment.position() + fragment.length() - 1 - block.position()).lineNumber();
        int j = fragment.position();
        for (int i = lineNoBegin; i <= lineNoEnd; i++) {
            QTextLine line = layout->lineAt(i);
            // 计算输出位置X
            qreal offset = alignPos(layout->textOption().alignment(), sizeToFill.width(), line.naturalTextWidth());
            //outPos.setX(outPos.x());
            outPos.setY(baseLine.y());
            // 获取属于本行的文本
            QString textOnLine;
            while (j < fragment.position() + fragment.length()) {
                int c = block.position();
                int d = line.textStart() + line.textLength();
                if (j < c + d)
                    textOnLine.append(text[j - fragment.position()]);
                else
                    break;
                j++;
            }
            // 绘制属于本行的Fragment中的文本
            drawText(painter, outPos, textOnLine, chf, brush, offset);
            // 有换行的话，要更新BaseLine的Y及outPos的X
            if (i < lineNoEnd) {
                // 更新基线位置Y
                baseLine.setY(baseLine.y() + line.height());
                outPos.setX(baseLine.x());
            }
        }
    }
}

// 第三步 绘制文字
void CGraphicsTextItem::drawText(QPainter *painter, QPointF &p, QString &text, const QTextCharFormat &fmt,  const QBrush &brush, qreal offset)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    QFont f = fmt.font();
    f.resolve(QFont::AllPropertiesResolved);

    // 获得第个文字的大小。。
    QFontMetricsF fm(f);
    QRectF textBoundingRect = fm.boundingRect(text);
    // draw text
    if (needDrawText(fmt) || brush.style() != Qt::NoBrush) {
        painter->setFont(f);
        QPen pen;
        if (brush.style() == Qt::NoBrush) {
            QBrush t = fmt.foreground();
            if (t.style() == Qt::NoBrush)
                pen.setBrush(QBrush(QColor(0, 0, 0)));
            else if (t.style() != Qt::SolidPattern)
                pen = QPen(t, 0);
            else {
                QColor c = t.color();
                QColor cc = c.toRgb();
                pen.setBrush(QBrush(cc));
            }
        } else {
            if (brush.style() != Qt::SolidPattern)
                pen = QPen(brush, 0);
            else {
                pen.setBrush(brush);
            }
        }
        painter->setPen(pen);
        // draw text.
        painter->drawText((p + QPointF(offset, 0)) + rect().topLeft(), text);
    }

    p += QPointF(textBoundingRect.width(), 0);
}
qreal CGraphicsTextItem::alignPos(Qt::Alignment a, const qreal &width, const qreal &textWidth)
{
    if (a & Qt::AlignRight)
        return  width - textWidth;
    else if (!(a & Qt::AlignLeft))
        return (width - textWidth) / 2;
    return 0;
}
bool CGraphicsTextItem::needDrawText(const QTextCharFormat &chf)
{
    return true;
}

void CGraphicsTextItem::clearLetterSpacing(QTextDocument *doc, int *blockNum)
{
    bool bAllBlock = !blockNum ? true : false;
    QVector<QTextBlock> blocks;
    if (bAllBlock) {
        for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next())
            blocks.push_back(block);
    } else {
        blocks.push_back(doc->findBlockByNumber(*blockNum));
    }
    // 清空所有文字的字间距
    for (int idx = 0; idx < blocks.size(); idx++) {
        QTextCursor cursor(blocks[idx]);
        for (int i = 0; i < blocks[idx].layout()->lineCount(); i++) {
            cursor.select(QTextCursor::LineUnderCursor);
            QTextCharFormat fmt;
            fmt.setFontLetterSpacing(100);
            cursor.mergeCharFormat(fmt);
            cursor.movePosition(QTextCursor::Down);
        }
    }
}
// 将QTextDocument中的指定Block置为发散对其方式，DocWidth为限定宽度,注意本函数会破坏原有的字符间距
void CGraphicsTextItem::adjustAlignJustify(QTextDocument *doc, qreal DocWidth, int *blockNum)
{
    // 清空文字的字间距
    clearLetterSpacing(doc, blockNum);
    // 获取要发散对其的Block
    bool bAllBlock = !blockNum ? true : false;
    QVector<QTextBlock> blocks;
    if (bAllBlock) {
        for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next())
            blocks.push_back(block);
    } else {
        blocks.push_back(doc->findBlockByNumber(*blockNum));
    }
    // 获取Layout的Margin
    QTextFrameFormat frame = doc->rootFrame()->frameFormat();
    // 发散对齐,计算字间距
    for (int idx = 0; idx < blocks.size(); idx++) {
        QTextCursor cursor(blocks[idx]);
        for (int i = 0; i < blocks[idx].layout()->lineCount(); i++) {
            QTextLine line = blocks[idx].layout()->lineAt(i);
            // 计算最后一个字的宽度
            cursor.setPosition(blocks[idx].position() + line.textStart() + line.textLength() - 1);
            cursor.setPosition(cursor.position() + 1, QTextCursor::KeepAnchor);
            QString text = cursor.selectedText();
            if (text.isEmpty()) {
                cursor.movePosition(QTextCursor::Down);
                continue;
            }
            QFontMetricsF m(cursor.charFormat().font());
            qreal lastCharWidth = m.width(text[0]);
            //  计算字符间距
            qreal widthOfLayout = DocWidth - (frame.leftMargin() + frame.rightMargin()) - lastCharWidth;
            qreal widthOfText = line.naturalTextRect().width() - lastCharWidth;
            qreal percentOfSpacing = widthOfLayout / widthOfText * 100;
            if (percentOfSpacing > 100.0f) {
                // 选择第一个字到最后一个字的前一个字，设置这些字的字间距
                cursor.setPosition(blocks[idx].position());
                int pos = blocks[idx].position() + line.textStart() + line.textLength() - 1;
                cursor.setPosition(pos,  QTextCursor::KeepAnchor);
                QTextCharFormat fmt;
                fmt.setFontLetterSpacing(percentOfSpacing);
                cursor.mergeCharFormat(fmt);
            }
            cursor.movePosition(QTextCursor::Down);
        }
    }
}


void CGraphicsTextItem::currentCharFormatChanged(const QTextCharFormat &format)
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(format.font().family());
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextSize(format.font().pointSize());
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextColor(format.foreground().color());

    //提示更改 TODO
    if (this->scene() != nullptr) {
        emit static_cast<CDrawScene *>(this->scene())->signalUpdateTextFont();
    }

}

bool CGraphicsTextItem::getManResizeFlag() const
{
    return m_bManResize;
}

void CGraphicsTextItem::setManResizeFlag(bool flag)
{
    m_bManResize = flag;
}

CGraphicsUnit CGraphicsTextItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsTextUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pText = new SGraphicsTextUnitData();
    unit.data.pText->rect.topLeft = this->rect().topLeft();
    unit.data.pText->rect.bottomRight = this->rect().bottomRight();
    unit.data.pText->font = this->m_Font;
    unit.data.pText->manResizeFlag = this->m_bManResize;
    unit.data.pText->content = this->m_pTextEdit->toHtml();

    return  unit;
}

CTextEdit *CGraphicsTextItem::getTextEdit()
{
    return m_pTextEdit;
}

bool CGraphicsTextItem::isEditable() const
{
    return !m_pTextEdit->isHidden();
}

void CGraphicsTextItem::doCut()
{
    m_pTextEdit->cut();
}

void CGraphicsTextItem::doCopy()
{
    m_pTextEdit->copy();
}

void CGraphicsTextItem::doPaste()
{
    m_pTextEdit->paste();
}

void CGraphicsTextItem::doSelectAll()
{
    m_pTextEdit->selectAll();
}

void CGraphicsTextItem::doTopAlignment()
{
    m_pTextEdit->setAlignment(Qt::AlignJustify);
}

void CGraphicsTextItem::doRightAlignment()
{
    m_pTextEdit->setAlignment(Qt::AlignRight);
}

void CGraphicsTextItem::doLeftAlignment()
{
    m_pTextEdit->setAlignment(Qt::AlignLeft);
}

void CGraphicsTextItem::doCenterAlignment()
{
    m_pTextEdit->setAlignment(Qt::AlignCenter);
}

void CGraphicsTextItem::doUndo()
{
    m_pTextEdit->undo();
}

void CGraphicsTextItem::doRedo()
{
    m_pTextEdit->redo();
}

