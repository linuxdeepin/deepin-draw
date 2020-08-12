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
//#include "cgraphicsitemhighlight.h"
#include "cgraphicsitem.h"
#include "widgets/ctextedit.h"
#include "cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cgraphicsitemselectedmgr.h"
#include "application.h"

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
    : CGraphicsRectItem()
    , m_pTextEdit(nullptr)
    , m_pProxy(nullptr)
    , m_bManResize(false)
{
    initTextEditWidget();
}

CGraphicsTextItem::CGraphicsTextItem(const SGraphicsTextUnitData &data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem(data.rect, head, parent)
    , m_pTextEdit(nullptr)
    , m_pProxy(nullptr)
    , m_bManResize(false)
{
    initTextEditWidget();
    m_Font = data.font;
    m_bManResize = data.manResizeFlag;
    m_pTextEdit->setHtml(data.content);
    m_pTextEdit->hide();
    QRectF rect(data.rect.topLeft, data.rect.bottomRight);
    setRect(rect);
    m_pTextEdit->document()->clearUndoRedoStacks();
}

CGraphicsTextItem::~CGraphicsTextItem()
{
    if (m_pTextEdit != nullptr) {
        m_pTextEdit->deleteLater();
        m_pTextEdit = nullptr;
    }
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

    updateHandleVisible();

    //全选会更改一次字体 所以字体获取要在这之前
    QTextCursor textCursor = m_pTextEdit->textCursor();
    textCursor.select(QTextCursor::Document);
    m_pTextEdit->setTextCursor(textCursor);

    m_pTextEdit->show();
    m_pTextEdit->document()->clearUndoRedoStacks();
}

void CGraphicsTextItem::initHandle()
{
    CGraphicsRectItem::initHandle();
    updateHandleVisible();
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

QColor CGraphicsTextItem::getSelectedTextColor()
{
    return m_pTextEdit->getSelectedTextColor();
}

int CGraphicsTextItem::getSelectedFontSize()
{
    return m_pTextEdit->getSelectedFontSize();
}

QString CGraphicsTextItem::getSelectedFontFamily()
{
    return m_pTextEdit->getSelectedFontFamily();
}

QString CGraphicsTextItem::getSelectedFontStyle()
{
    return m_pTextEdit->getSelectedFontStyle();
}

int CGraphicsTextItem::getSelectedFontWeight()
{
    return m_pTextEdit->getSelectedFontWeight();
}

int CGraphicsTextItem::getSelectedTextColorAlpha()
{
    return m_pTextEdit->getSelectedTextColorAlpha();
}

void CGraphicsTextItem::makeEditabel()
{
    if (getMutiSelect())
        return;

    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode() == selection ||
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode() == text) {
        m_pTextEdit->show();
        QTextCursor textCursor = m_pTextEdit->textCursor();
        textCursor.select(QTextCursor::Document);
        m_pTextEdit->setTextCursor(textCursor);
    }

    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->updateBlurItem(this);
    }
    m_pTextEdit->setFocus();
}

void CGraphicsTextItem::updateHandleVisible()
{
    //bool visble = getManResizeFlag();
    //qDebug() << "CGraphicsTextItem visble = " << visble << "size = " << m_handles.size();

    //    if (drawScene() != nullptr) {
    //        CGraphicsItemSelectedMgr *pMgr = drawScene()->getItemsMgr();
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::LeftTop);
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::Top);
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::RightTop);
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::RightBottom);
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::Bottom);
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::LeftBottom);
    //        pMgr->setHandleVisible(visble, CSizeHandleRect::Rotation);
    //    }
}

bool CGraphicsTextItem::isGrabToolEvent()
{
    return isEditable();
}

void CGraphicsTextItem::slot_textmenu(QPoint)
{
    m_menu->move(cursor().pos());
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
    updateShape();
    if (drawScene() != nullptr && isSelected())
        drawScene()->setHighlightHelper(mapToScene(getHighLightPath()));
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

QString CGraphicsTextItem::getTextFontStyle()
{
    return m_Font.styleName();
}

void CGraphicsTextItem::setTextFontStyle(const QString &style)
{
    /* 注意：5.11.3版本中 QTextCharFormat 不支持 setFontStyleName 接口
     * 只有在5.13之后才支持，同时无法直接设置font的样式然后修改字体字重
     * 后续Qt版本升级后可以查看相关文档使用 setFontStyleName 接口
    */
    //    QFont::Thin    0   QFont::ExtraLight 12  QFont::Light 25
    //    QFont::Normal  50  QFont::Medium     57  QFont::DemiBold 63
    //    QFont::Bold    75  QFont::ExtraBold  81  QFont::Black 87
    quint8 weight = 0;
    if (style == "Thin") {
        weight = 0;
    } else if (style == "ExtraLight") {
        weight = 12;
    } else if (style == "Light") {
        weight = 25;
    } else if (style == "Normal" || style == "Regular") {
        weight = 50;
    } else if (style == "Medium") {
        weight = 57;
    } else if (style == "DemiBold") {
        weight = 63;
    } else if (style == "Bold") {
        weight = 75;
    } else if (style == "ExtraBold") {
        weight = 81;
    } else if (style == "Black") {
        weight = 87;
    }

    QTextCharFormat fmt;
    fmt.setFontWeight(weight);
    mergeFormatOnWordOrSelection(fmt);
    m_Font.setStyleName(style);// 缓存自身最新的字体样式
}

void CGraphicsTextItem::setFontSize(qreal size)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(size);
    mergeFormatOnWordOrSelection(fmt);
    m_Font.setPointSizeF(size);
    m_pTextEdit->setFocus();
}

qreal CGraphicsTextItem::getFontSize()
{
    return m_Font.pointSizeF();
}

void CGraphicsTextItem::setFontFamily(const QString &family, bool setFoucs)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(family);
    mergeFormatOnWordOrSelection(fmt, setFoucs);
    m_Font.setFamily(family);
}

QString CGraphicsTextItem::getFontFamily()
{
    return m_Font.family();
}

void CGraphicsTextItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
    CGraphicsRectItem::resizeTo(dir, point, false, false);
    setManResizeFlag(true);
    updateWidget();
    m_pTextEdit->resizeDocument();
}

CGraphicsItem *CGraphicsTextItem::duplicateCreatItem()
{
    return new CGraphicsTextItem;
}

void CGraphicsTextItem::duplicate(CGraphicsItem *item)
{
    static_cast<CGraphicsTextItem *>(item)->setManResizeFlag(this->getManResizeFlag());
    static_cast<CGraphicsTextItem *>(item)->getCGraphicsProxyWidget()->hide();
    static_cast<CGraphicsTextItem *>(item)->setFontFamily(this->getFontFamily());
    static_cast<CGraphicsTextItem *>(item)->setTextFontStyle(this->getTextFontStyle());
    static_cast<CGraphicsTextItem *>(item)->setFontSize(this->getFontSize());
    static_cast<CGraphicsTextItem *>(item)->setTextColor(this->getTextColor());
    static_cast<CGraphicsTextItem *>(item)->getTextEdit()->setDocument(
        this->getTextEdit()->document()->clone(static_cast<CGraphicsTextItem *>(item)->getTextEdit()));
    CGraphicsRectItem::duplicate(item);
}

void CGraphicsTextItem::loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo)
{
    Q_UNUSED(allInfo)
    SGraphicsTextUnitData *pTextData = data.data.pText;

    if (pTextData != nullptr) {
        loadGraphicsRectUnit(pTextData->rect);
        m_Font = pTextData->font;
        m_bManResize = pTextData->manResizeFlag;

//        if (allInfo)
        m_pTextEdit->setHtml(pTextData->content);

        m_pTextEdit->hide();
        m_color = pTextData->color;
        QRectF rect(pTextData->rect.topLeft, pTextData->rect.bottomRight);
        setRect(rect);
    }
    loadHeadData(data.head);
}

void CGraphicsTextItem::setTextColor(const QColor &col)
{
    //qDebug() << "Content: " << col;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    m_color = col;
}

QColor CGraphicsTextItem::getTextColor()
{
    return m_color;
}

void CGraphicsTextItem::setTextColorAlpha(const int &alpha)
{
    QTextCharFormat fmt;
    m_color.setAlpha(alpha);
    fmt.setForeground(m_color);
    mergeFormatOnWordOrSelection(fmt);
}

int CGraphicsTextItem::getTextColorAlpha()
{
    return m_color.alpha();
}

void CGraphicsTextItem::mergeFormatOnWordOrSelection(const QTextCharFormat &format, bool setFoucs)
{
    // [0] 设置当前选中文本都最新格式
    QTextCursor cursor = m_pTextEdit->textCursor();
    cursor.mergeCharFormat(format);

    // [1] 设置 TextEdit 光标处最新的格式
    m_pTextEdit->mergeCurrentCharFormat(format);

    // [2] 设置焦点
    if (setFoucs) {
        if (drawScene() != nullptr) {
            drawScene()->drawView()->setFocus();
            drawScene()->setFocusItem(this);
            qDebug() << "setFoucs =====  " << setFoucs << dApp->focusWidget() << drawScene()->focusItem();
        }
        m_pTextEdit->setFocus();
    }
}

void CGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateHandlesGeometry();

    // judge selectool isValid
    if (!rect().isValid())
        return;

    beginCheckIns(painter);
    drawDocument(painter, m_pTextEdit->document(), this->rect());
    endCheckIns(painter);

    paintMutBoundingLine(painter, option);
}

void CGraphicsTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    makeEditabel();
}

bool CGraphicsTextItem::isPosPenetrable(const QPointF &posLocal)
{
    Q_UNUSED(posLocal)
    return false;
}

QVariant CGraphicsTextItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        if (value.toBool() == false)
            this->getTextEdit()->hide();

        updateHandleVisible();
    }

    return CGraphicsRectItem::itemChange(change, value);
}

void CGraphicsTextItem::drawDocument(QPainter *painter,
                                     const QTextDocument *doc,
                                     const QRectF &r,
                                     const QBrush &brush)
{
    Q_UNUSED(brush)
    if (doc->isEmpty())
        return;

    painter->save();
    // 按区域绘制
    if (r.isValid()) {
        painter->setClipRect(r, Qt::IntersectClip);
    }
    painter->translate(r.topLeft());
    QTextDocument *t_doc = const_cast<QTextDocument *>(doc);
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
    Q_UNUSED(chf);
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
            if (percentOfSpacing > 100.0) {
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
    Q_UNUSED(format)
    // 此处不再需要向缓存中写入数据了
//    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(format.font().family());
//    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextSize(format.font().pointSize());
//    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextColor(format.foreground().color());

//    //提示更改 TODO
//    if (this->scene() != nullptr) {
//        emit static_cast<CDrawScene *>(this->scene())->signalUpdateTextFont();
//    }
}

bool CGraphicsTextItem::getManResizeFlag() const
{
    return m_bManResize;
}

void CGraphicsTextItem::setManResizeFlag(bool flag)
{
    bool changed = (flag != m_bManResize);
    m_bManResize = flag;

    if (changed)
        updateHandleVisible();
}

CGraphicsUnit CGraphicsTextItem::getGraphicsUnit(bool all) const
{
    Q_UNUSED(all)
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
    unit.data.pText->manResizeFlag = this->getManResizeFlag();
//    unit.data.pText->content = all ? this->m_pTextEdit->toHtml() : "";
    unit.data.pText->content = this->m_pTextEdit->toHtml();
    unit.data.pText->color = m_color;

    return  unit;
}

CTextEdit *CGraphicsTextItem::getTextEdit()
{
    return m_pTextEdit;
}

bool CGraphicsTextItem::isEditable() const
{
    return !m_pTextEdit->isHidden();
    //return m_pTextEdit->getEditing();
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

void CGraphicsTextItem::setSelectTextBlockAlign(const Qt::Alignment &align)
{
    switch (align) {
    case Qt::AlignLeft : {
        m_pTextEdit->setAlignment(Qt::AlignLeft);
        break;
    }
    case Qt::AlignRight : {
        m_pTextEdit->setAlignment(Qt::AlignRight);
        break;
    }
    case Qt::AlignHCenter : {
        m_pTextEdit->setAlignment(Qt::AlignHCenter);
        break;
    }
    case Qt::AlignTop : {
        m_pTextEdit->setAlignment(Qt::AlignTop);
        break;
    }
    case Qt::AlignBottom : {
        m_pTextEdit->setAlignment(Qt::AlignBottom);
        break;
    }
    case Qt::AlignVCenter : {
        m_pTextEdit->setAlignment(Qt::AlignVCenter);
        break;
    }
    case Qt::AlignCenter : {
        m_pTextEdit->setAlignment(Qt::AlignCenter);
        break;
    }
    }
}

void CGraphicsTextItem::doUndo()
{
    m_pTextEdit->undo();
}

void CGraphicsTextItem::doRedo()
{
    m_pTextEdit->redo();
}

void CGraphicsTextItem::doDelete()
{
    m_pTextEdit->textCursor().deleteChar();
}

