#include "bordercolorbutton.h"

#include <QPainter>
#include <QDebug>

#include "utils/baseutils.h"
#include "utils/configsettings.h"

const qreal COLOR_RADIUS = 4;

BorderColorButton::BorderColorButton(QWidget *parent)
    : QPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    setFixedSize(24, 24);
    setCheckable(true);
     m_color = QColor(ConfigSettings::instance()->value("common", "strokeColor").toString());
    update();

    connect(this, &QPushButton::clicked, this, &BorderColorButton::setCheckedStatus);
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            &BorderColorButton::updateConfigColor);
}

void BorderColorButton::updateConfigColor(const QString &group,
                                                                                   const QString &key)
{
    if (group == "common" && key == "strokeColor")
    {
        m_color = QColor(ConfigSettings::instance()->value(group, key).toString());
        update();
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

    if (m_isChecked || m_isHover)
    {
        painter.setBrush(QBrush(QColor(0, 0, 0, 13)));
        painter.drawRoundedRect(rect(), 4, 4);
    } else
    {
        painter.setBrush(QBrush(Qt::transparent));
    }

    QPen pen;
    pen.setWidth(2);
    pen.setColor(m_color);
    painter.setPen(pen);
    painter.drawEllipse(this->rect().center(), 8, 8);

    if (m_isChecked)
    {
        painter.setBrush(QBrush(QColor(0, 0, 0, 13)));
        painter.drawEllipse(this->rect().center(), 8, 8);
    }
}

void BorderColorButton::setColor(QColor color)
{
    m_color = color;
    update();
}

void BorderColorButton::setColorIndex(int index)
{
    m_color = colorIndexOf(index);
    update();
}

void BorderColorButton::setCheckedStatus(bool checked)
{
    m_isChecked = checked;
    update();
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
