#include "bordercolorbutton.h"

#include <QPainter>
#include <QDebug>

const qreal COLOR_RADIUS = 4;

BorderColorButton::BorderColorButton(QWidget *parent)
    : QPushButton(parent),
      m_color(QColor(Qt::red)),
      m_isHover(false),
      m_isChecked(false)
{
    setFixedSize(24, 24);
    setCheckable(true);

    connect(this, &QPushButton::clicked, this,
            &BorderColorButton::setCheckedStatus);
}

void BorderColorButton::updateConfigColor(const QString &shape,
                                                                                   const QString &key, int index)
{
    if (shape == "common" && key == "color_index")
    {
//        setColor(colorIndexOf(index));
    }
}

BorderColorButton::~BorderColorButton()
{
}

void BorderColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::transparent);

    if (m_isHover || m_isChecked)
    {
        painter.setBrush(QBrush(QColor(0, 0, 0, 20.4)));
        painter.drawRoundedRect(rect(), 4, 4);
    } else {
        painter.setBrush(QBrush(Qt::transparent));
    }

    QPen pen;
    pen.setWidth(2);
    pen.setColor(m_color);
    painter.setPen(pen);
    painter.drawEllipse(this->rect().center(), 8, 8);
}

void BorderColorButton::setColor(QColor color)
{
    m_color = color;
    update();
}

void BorderColorButton::setColorIndex()
{
//    int colorNum = ConfigSettings::instance()->value("common", "color_index").toInt();
//    m_color = colorIndexOf(colorNum);
    update();
}

void BorderColorButton::setCheckedStatus(bool checked)
{
    if (checked)
    {
        m_isChecked = true;
        update();
    }

}

void BorderColorButton::enterEvent(QEvent *)
{
    if (!m_isHover)
    {
        m_isHover = true;
        update();
    }
}

void BorderColorButton::leaveEvent(QEvent *)
{
    if (m_isHover)
    {
        m_isHover = false;
        update();
    }
}
