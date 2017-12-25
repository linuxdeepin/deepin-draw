#include "bigcolorbutton.h"

#include "utils/baseutils.h"
#include "utils/configsettings.h"

#include <QDebug>

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

BigColorButton::BigColorButton(const QString &group, QWidget *parent)
    : QPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    setFixedSize(24, 24);
    setCheckable(false);
    m_group = group;

    if (group == "common")
    {
        bool transColBtnChecked = ConfigSettings::instance()->value(
                    "common", "fillColor_transparent").toBool();
        if (transColBtnChecked)
        {
            m_color = QColor(Qt::transparent);
        } else {
            m_color = QColor(ConfigSettings::instance()->value(
                        "common", "fillColor").toString());
        }
    } else {
        bool transColBtnChecked = ConfigSettings::instance()->value(
                    "text", "fillColor_transparent").toBool();
        if (transColBtnChecked)
        {
            m_color = QColor(Qt::transparent);
        } else {
            m_color = QColor(ConfigSettings::instance()->value(
                        "text", "fillColor").toString());
        }
    }

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged,
                  this, &BigColorButton::updateConfigColor);
}

void BigColorButton::updateConfigColor(const QString &group,
                                                                            const QString &key)
{
    if (group == m_group && (key == "fillColor" || key == "fillColor_alpha"
                             || key == "fillColor_transparent"))
    {
        qDebug() << "updateConfigColor......" << key;
        m_color = QColor(ConfigSettings::instance()->value(
                            m_group, "fillColor").toString());

        bool transColBtnChecked = ConfigSettings::instance()->value(m_group,
                                                                    "fillColor_transparent").toBool();
        if (transColBtnChecked)
            m_color = QColor(Qt::transparent);
        else {
            m_color = QColor(ConfigSettings::instance()->value(
                       m_group, "fillColor").toString());
        }

        update();
    }
}

BigColorButton::~BigColorButton()
{
}

void BigColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing
                           |QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);

    painter.setBrush(QBrush(QColor(0, 0, 0, 13)));
    QColor drawColor = m_color;

    if (m_isHover || m_isChecked)
    {
        painter.setBrush(QBrush(QColor(0, 0, 0, 25)));
        painter.drawRoundedRect(rect(), 4, 4);
    } else if (m_isChecked)
    {
        drawColor = QColor(m_color.red(), m_color.green(), m_color.blue(), 25);
    }

    painter.setBrush(drawColor);
    painter.drawEllipse(CENTER_POINT,  BTN_RADIUS, BTN_RADIUS);

    QPen borderPen;
    borderPen.setWidth(1);
    borderPen.setColor(QColor(0, 0, 0, 15));
    painter.setPen(borderPen);
    if (m_isChecked)
    {
        painter.setBrush(QColor(0, 0, 0, 55));
    } else {
        painter.setBrush(Qt::transparent);
    }
    painter.drawEllipse(CENTER_POINT, BTN_RADIUS + 1, BTN_RADIUS+1);
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

void BigColorButton::mousePressEvent(QMouseEvent* )
{
    m_isChecked = !m_isChecked;
    update();

    btnCheckStateChanged(m_isChecked);
}

void BigColorButton::resetChecked()
{
    m_isChecked = false;
    update();
}
