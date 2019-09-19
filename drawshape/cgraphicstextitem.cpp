#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include <QTextEdit>
#include <QPainter>
#include <QTextBlock>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QFont>

CGraphicsTextItem::CGraphicsTextItem()
    : CGraphicsRectItem ()
    , m_pTextEdit(nullptr)
    , m_pProxy(nullptr)
{
    m_pTextEdit = new QTextEdit(QObject::tr("请输入文字"));
    m_pTextEdit->setMinimumSize(QSize());
//    connect(m_pTextEdit, &QTextEdit::currentCharFormatChanged,
//            this, &CGraphicsTextItem2::currentCharFormatChanged);
    QTextCursor textCursor = m_pTextEdit->textCursor();
    textCursor.select(QTextCursor::Document);
    m_pTextEdit->setTextCursor(textCursor);

    m_pTextEdit->setWindowFlags(Qt::FramelessWindowHint);
    m_pTextEdit->setFrameShape(QTextEdit::NoFrame);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pProxy = new CGraphicsProxyWidget(this);
    m_pProxy->setWidget(m_pTextEdit);
    m_pProxy->setMinimumSize(0, 0);
    m_pProxy->setZValue(this->zValue() - 0.1);
}

CGraphicsTextItem::~CGraphicsTextItem()
{

}

QTextEdit *CGraphicsTextItem::getTextEdit() const
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
    m_pProxy->resize(rect.width(), rect.height());
    CGraphicsRectItem::setRect(rect);
    updateWidget();
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
    m_pTextEdit->resize(int(geom.width()), int(geom.height()));
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

//void CGraphicsTextItem::setFontSize(qreal size)
//{
//    QTextCharFormat fmt;
//    fmt.setFontPointSize(size);
//    mergeFormatOnWordOrSelection(fmt);

//    m_size = size;
//}

void CGraphicsTextItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    CGraphicsRectItem::resizeTo(dir, point, bShiftPress, bAltPress);
    updateWidget();
}

CGraphicsItem *CGraphicsTextItem::duplicate() const
{
    CGraphicsTextItem *item = new CGraphicsTextItem();
    item->setRect(this->rect());
    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
    item->getTextEdit()->setDocument(this->getTextEdit()->document());
    item->getTextEdit()->hide();
    return item;
}

void CGraphicsTextItem::setTextColor(const QColor &col)
{
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);

    m_color = col;
}

void CGraphicsTextItem::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = m_pTextEdit->textCursor();
    //if (!cursor.hasSelection())
    //cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    m_pTextEdit->mergeCurrentCharFormat(format);
}

void CGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    drawDocument(painter, m_pTextEdit->document(), rect());
    if (this->isSelected()) {
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(224, 224, 224));
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->rect());
    }
}

void CGraphicsTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    m_pProxy->show();
    m_pProxy->setFocus();
    QTextCursor textCursor = m_pTextEdit->textCursor();
    textCursor.select(QTextCursor::Document);
    m_pTextEdit->setTextCursor(textCursor);
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
    if (r.isValid())
        painter->setClipRect(r, Qt::IntersectClip);

    QSizeF size = doc->size();
    QTextFrameFormat fmt = doc->rootFrame()->frameFormat();
    size.setWidth(size.width() - fmt.leftMargin() - fmt.rightMargin());
    for (QTextBlock bl = doc->begin(); bl != doc->end(); bl = bl.next()) {
        // 因为每个QTextDocument 中包含一个QTextLayout
        drawTextLayout(painter, bl, size, brush);
    }
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
            outPos.setX(outPos.x() + offset);
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
            drawText(painter, outPos, textOnLine, chf, brush);
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
void CGraphicsTextItem::drawText(QPainter *painter, QPointF &p, QString &text, const QTextCharFormat &fmt,  const QBrush &brush)
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
        painter->drawText(p + rect().topLeft(), text);
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
    CDrawParamSigleton::GetInstance()->setTextFont(format.font().family());
    CDrawParamSigleton::GetInstance()->setTextSize(format.font().pointSize());
    CDrawParamSigleton::GetInstance()->setTextColor(format.foreground().color());

    //提示更改 TODO

}
