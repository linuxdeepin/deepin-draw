#include "bigcolorbutton.h"

#include "utils/baseutils.h"
#include "utils/configsettings.h"

#include <QDebug>

const qreal COLOR_RADIUS = 8;

BigColorButton::BigColorButton(QWidget *parent)
    : QPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    setFixedSize(24, 24);
    setCheckable(true);

    m_color = QColor(ConfigSettings::instance()->value(
                        "common", "fillColor").toString());

    connect(this, &QPushButton::clicked, this, &BigColorButton::setCheckedStatus);
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged,
                  this, &BigColorButton::updateConfigColor);
}

void BigColorButton::updateConfigColor(const QString &group,
                                                                            const QString &key)
{
    if (group == "common" && key == "fillColor")
    {
        m_color = QColor(ConfigSettings::instance()->value(
                            group, key).toString());
        update();
    }
}

BigColorButton::~BigColorButton()
{
}

void BigColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::transparent);

    painter.setBrush(QBrush(QColor(0, 0, 0, 13)));
    if (m_isHover || m_isChecked)
    {
        painter.drawRoundedRect(rect(), 4, 4);
    }

    painter.setBrush(QBrush(QColor(m_color)));
    painter.drawEllipse(QPointF(12, 12), COLOR_RADIUS, COLOR_RADIUS);

    if (m_isChecked)
    {
        painter.setBrush(QColor(0, 0, 0, 13));
        painter.drawEllipse(QPointF(12, 12), COLOR_RADIUS, COLOR_RADIUS);
    }
}

void BigColorButton::setColor(QColor color)
{
    m_color = color;
    update();
}

void BigColorButton::setColorIndex(int index)
{
    m_color = colorIndexOf(index);
    update();
}

void BigColorButton::setCheckedStatus(bool checked)
{
    m_isChecked = checked;
    update();

    emit btnCheckStateChanged(m_isChecked);
}

void BigColorButton::enterEvent(QEvent *)
{
    if (!m_isHover)
    {
        m_isHover = true;
        update();
    }
}

void BigColorButton::leaveEvent(QEvent *)
{
    if (m_isHover)
    {
        m_isHover = false;
        update();
    }
}
