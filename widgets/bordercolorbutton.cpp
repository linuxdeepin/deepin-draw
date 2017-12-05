#include "bordercolorbutton.h"

#include <QPainter>
#include <QDebug>

#include "utils/baseutils.h"
#include "utils/configsettings.h"

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

BorderColorButton::BorderColorButton(QWidget *parent)
    : QPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    setFixedSize(24, 24);
    setCheckable(false);
     m_color = QColor(ConfigSettings::instance()->value("common", "strokeColor").toString());
     int alpha = ConfigSettings::instance()->value("common", "strokeColor_alpha").toInt();
     if (alpha == 0)
         m_color = QColor(Qt::transparent);

    update();

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            &BorderColorButton::updateConfigColor);
}

void BorderColorButton::updateConfigColor(const QString &group,
                                                                                   const QString &key)
{
    if (group == "common" && key == "strokeColor" || key == "strokeColor_alpha")
    {
        m_color = QColor(ConfigSettings::instance()->value(group,  "strokeColor").toString());
        int alpha = ConfigSettings::instance()->value("common", "strokeColor_alpha").toInt();
        if (alpha == 0)
            m_color = QColor(Qt::transparent);
        update();
    }
}

BorderColorButton::~BorderColorButton()
{
}

void BorderColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing
                           |QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);

    QColor drawColor = m_color;

    if (m_isChecked || m_isHover)
    {
        painter.setBrush(QBrush(QColor(0, 0, 0, 25)));
        painter.drawRoundedRect(rect(), 4, 4);
    } else if (m_isChecked)
    {
        drawColor = QColor(m_color.red(), m_color.green(), m_color.black(), 25);
    } else
    {
        painter.setBrush(Qt::transparent);
        painter.drawRoundedRect(rect(), 4, 4);
    }

    QPen pen;
    pen.setWidth(2);
    pen.setColor(drawColor);
    painter.setPen(pen);
    painter.setBrush(Qt::transparent);
    painter.drawEllipse(CENTER_POINT, BTN_RADIUS, BTN_RADIUS);

    QPen borderPen;
    borderPen.setWidth(1);
    borderPen.setColor(QColor(0, 0, 0, 15));
    painter.setPen(borderPen);
    painter.drawEllipse(CENTER_POINT, BTN_RADIUS + 1, BTN_RADIUS + 1);
    if (m_isChecked)
    {
        painter.setBrush(QColor(0, 0, 0, 35));
        painter.drawEllipse(CENTER_POINT, BTN_RADIUS - 1, BTN_RADIUS - 1);
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

void BorderColorButton::resetChecked()
{
    m_isChecked = false;
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

void BorderColorButton::mousePressEvent(QMouseEvent* )
{
    m_isChecked = !m_isChecked;
    btnCheckStateChanged(m_isChecked);

    update();
}
