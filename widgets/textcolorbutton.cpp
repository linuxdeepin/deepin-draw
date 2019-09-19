#include "textcolorbutton.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"

#include <QDebug>

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

TextColorButton::TextColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    setFixedSize(24, 24);
    setCheckable(false);

    m_color = CDrawParamSigleton::GetInstance()->getTextColor();
}

void TextColorButton::updateConfigColor()
{
    QColor configColor = CDrawParamSigleton::GetInstance()->getTextColor();
    if (m_color == configColor) {
        return;
    }
    m_color = configColor;
    update();
}

TextColorButton::~TextColorButton()
{
}

void TextColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing
                           | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);

    painter.setBrush(QBrush(QColor(0, 0, 0, 13)));
    QColor drawColor = m_color;

    if (m_isHover || m_isChecked) {
        painter.setBrush(QBrush(QColor(0, 0, 0, 25)));
        painter.drawRoundedRect(rect(), 4, 4);
    } else if (m_isChecked) {
        drawColor = QColor(m_color.red(), m_color.green(), m_color.blue(), 25);
    }

    painter.setBrush(drawColor);
    painter.drawEllipse(CENTER_POINT,  BTN_RADIUS, BTN_RADIUS);

    QPen borderPen;
    borderPen.setWidth(1);
    borderPen.setColor(QColor(0, 0, 0, 15));
    painter.setPen(borderPen);
    if (m_isChecked) {
        painter.setBrush(QColor(0, 0, 0, 55));
    } else {
        painter.setBrush(Qt::transparent);
    }
    painter.drawEllipse(CENTER_POINT, BTN_RADIUS + 1, BTN_RADIUS + 1);
}

void TextColorButton::setColor(QColor color)
{
    m_color = color;
    update();
}

void TextColorButton::setColorIndex(int index)
{
    m_color = colorIndexOf(index);
    update();
}

void TextColorButton::enterEvent(QEvent *)
{
    if (!m_isHover) {
        m_isHover = true;
        update();
    }
}

void TextColorButton::leaveEvent(QEvent *)
{
    if (m_isHover) {
        m_isHover = false;
        update();
    }
}

void TextColorButton::mousePressEvent(QMouseEvent * )
{
    m_isChecked = !m_isChecked;
    update();

    btnCheckStateChanged(m_isChecked);
}

void TextColorButton::resetChecked()
{
    m_isChecked = false;
    update();
}
