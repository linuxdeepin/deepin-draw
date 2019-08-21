#include "textedit.h"

#include <QDebug>
#include <QPen>
#include <QScrollBar>
#include <QPixmap>
#include <QFontMetricsF>
#include <QApplication>

#include "utils/baseutils.h"
#include "utils/configsettings.h"

const QSize CURSOR_SIZE = QSize(5, 20);
const int TEXT_MARGIN = 10;

TextEdit::TextEdit(int index, QWidget *parent)
    : QPlainTextEdit(parent)
    , m_textColor(Qt::black)
    , m_fontsizeRation(1)
{
    m_index = index;
    m_startPos = mapToParent(QPoint(0, 0));
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setContextMenuPolicy(Qt::NoContextMenu);

    int defaultColorIndex = 3;
    QColor defaultColor = colorIndexOf(defaultColorIndex);
    setColor(defaultColor);

    QFont textFont;
    m_fontSize = 12;
    textFont.setPixelSize(m_fontSize);
    this->document()->setDefaultFont(textFont);

    QTextCursor cursor = textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);

    QFontMetricsF m_fontMetric = QFontMetricsF(this->document()->defaultFont());
    QSizeF originSize = QSizeF(m_fontMetric.boundingRect(
                                   "d").width()  + TEXT_MARGIN,  m_fontMetric.boundingRect(
                                   "d").height() + TEXT_MARGIN);
    this->resize(originSize.width(), originSize.height());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this->document(), &QTextDocument::contentsChange, this,  [=]{
        m_startPos = mapToParent(QPoint(0, 0));
        updateContentSize(this->toPlainText());
    });
}

const QColor TextEdit::getTextColor()
{
    return m_textColor;
}

int TextEdit::fontSize()
{
    return m_fontSize;
}

int TextEdit::getIndex()
{
    return m_index;
}

void TextEdit::setColor(QColor c)
{
    m_textColor = c;
    setStyleSheet(QString("TextEdit {background-color: transparent;"
        "color: rgba(%1, %2, %3, %4); border: none;}").arg(m_textColor.red()
        ).arg(m_textColor.green()).arg(m_textColor.blue()).arg(m_textColor.alpha()));
    this->updateGeometry();
}

void TextEdit::setFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font;
    font.setPixelSize(int(m_fontSize*m_fontsizeRation));
    this->document()->setDefaultFont(font);
    this->updateGeometry();

    updateContentSize(this->toPlainText());
}

void TextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    QPlainTextEdit::inputMethodEvent(e);

    QString virtualStr = this->toPlainText() + e->preeditString();
    updateContentSize(virtualStr);
}

void TextEdit::updateContentSize(QString content)
{
    QFontMetricsF fontMetric = QFontMetricsF(this->document()->defaultFont());
    QSizeF docSize =  fontMetric.size(0,  content);
    this->setMinimumSize(docSize.width() + TEXT_MARGIN,
                                          docSize.height() + TEXT_MARGIN);
    this->resize(docSize.width() + TEXT_MARGIN,
                        docSize.height() + TEXT_MARGIN);
    emit  repaintTextRect(this,  QRectF(this->x(), this->y(),
                                        docSize.width() + TEXT_MARGIN,
                                        docSize.height() + TEXT_MARGIN));

    qDebug() << "TextEdit updateContentSize:" << QRectF(this->x(), this->y(),
    docSize.width() + TEXT_MARGIN, docSize.height() + TEXT_MARGIN)
    << this->rect() << mapToParent(QPoint(0, 0));
}

void TextEdit::setFontsizeRation(qreal ration)
{
    m_fontsizeRation = ration;
    move(QPoint(int(m_startPos.x()*ration), int(m_startPos.y()*ration)));
    setFontSize(m_fontSize);
}

void TextEdit::movePos(QPointF movePos)
{
    m_startPos = movePos;
    this->move(QPoint(m_startPos.x(), m_startPos.y()));
}

void TextEdit::updateCursor()
{
//    setTextColor(Qt::green);
}

void TextEdit::setCursorVisible(bool visible)
{
    if (visible) {
        setCursorWidth(1);
    } else {
        setCursorWidth(0);
    }
}

void TextEdit::keepReadOnlyStatus()
{
}

void TextEdit::mousePressEvent(QMouseEvent *e)
{
    bool activeMove = ConfigSettings::instance()->value("tools", "activeMove").toBool();
    if (activeMove)
    {
        emit textEditSelected(getIndex());
        if (e->button() == Qt::RightButton)
        {
            emit showMenuInTextEdit();
            return;
        }

        if (!this->isReadOnly()) {
            QPlainTextEdit::mousePressEvent(e);
            return;
        }

        if (e->button() == Qt::LeftButton) {
            m_isPressed = true;
            m_pressPoint = QPointF(mapToGlobal(e->pos()));
        }
    }
//    QPlainTextEdit::mousePressEvent(e);
}

void TextEdit::mouseMoveEvent(QMouseEvent *e)
{
    bool activeMove = ConfigSettings::instance()->value("tools", "activeMove").toBool();
    if (activeMove)
    {
        emit hoveredOnTextEdit(m_index);
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        QPointF posOrigin = QPointF(mapToGlobal(e->pos()));
        QPointF movePos = QPointF(posOrigin.x(), posOrigin.y());

        if (m_isPressed && movePos != m_pressPoint) {
            this->move(this->x() + movePos.x() - m_pressPoint.x(),
                       this->y() + movePos.y() - m_pressPoint.y());

            m_startPos = mapToParent(QPoint(0, 0));
            emit  repaintTextRect(this,  QRectF(qreal(this->x()), qreal(this->y()),
                                                this->width(),  this->height()));
            m_pressPoint = movePos;
        }
    }

    QPlainTextEdit::mouseMoveEvent(e);
}

void TextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    bool activeMove = ConfigSettings::instance()->value("tools", "activeMove").toBool();
    if (activeMove)
    {
        m_isPressed = false;
        if (this->isReadOnly()) {
            setMouseTracking(false);
            return;
        }
    }

    QPlainTextEdit::mouseReleaseEvent(e);
}

void TextEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    bool activeMove = ConfigSettings::instance()->value("tools", "activeMove").toBool();
    if (activeMove)
    {
        this->setReadOnly(false);
        this->setCursorVisible(true);
        emit backToEditing();
        QPlainTextEdit::mouseDoubleClickEvent(e);
    }
}

void TextEdit::keyPressEvent(QKeyEvent* e)
{
    QPlainTextEdit::keyPressEvent(e);
    if (e->key() == Qt::Key_Escape && !this->isReadOnly()) {
        this->setReadOnly(true);
    }
}

void TextEdit::enterEvent(QEnterEvent* e)
{
    Q_UNUSED(e);
    bool activeMove = ConfigSettings::instance()->value("tools", "activeMove").toBool();
    if (activeMove)
    {
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
    }
}

TextEdit::~TextEdit() {}
