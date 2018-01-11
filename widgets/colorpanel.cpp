#include "colorpanel.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QDebug>

#include "utils/global.h"
#include "utils/baseutils.h"
#include "utils/configsettings.h"
#include "colorlabel.h"
#include "colorslider.h"
#include "pickcolorwidget.h"

const int ORIGIN_HEIGHT = 200;
const int EXPAND_HEIGHT = 416;
const int RADIUS = 3;
const int BORDER_WIDTH = 2;
const QSize COLOR_BORDER_SIZE = QSize(20, 20);
const QSize COLOR_BUTTN = QSize(14, 14);

ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QPushButton(parent)
    , m_disable(false)
{
    m_color = color;
    setFixedSize(COLOR_BORDER_SIZE);
    setCheckable(true);

    connect(this, &ColorButton::clicked, this, [=]{
        if (m_disable)
        {
            qDebug() << "ColorButton currentColor transparent.";
            emit colorButtonClicked(Qt::transparent);
        } else
        {
            qDebug() << "ColorButton currentColor:" << m_color.name();
            emit colorButtonClicked(m_color);
        }
    });
}

void ColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(m_color);

    if (m_color == QColor(Qt::transparent))
    {
        painter.drawPixmap(QRect(3, 3, this->width() - 6, this->height() - 6),
            QPixmap(":/theme/light/images/draw/color_disable_active.png"));
        if (isChecked())
        {
            painter.setBrush(QBrush());
            QPen borderPen;
            borderPen.setWidth(BORDER_WIDTH);
            borderPen.setColor("#01bdff");
            painter.setPen(borderPen);
            painter.drawRoundedRect(QRect(1, 1, this->width() - 2,
                                          this->height() - 2), RADIUS, RADIUS);
        }
        return;
    }

    painter.setBrush(QBrush(m_color));

    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0, 0, 0, 26));
    painter.setPen(pen);
    painter.drawRoundedRect(QRect(3, 3, this->width() - 6,
                                  this->height() - 6), RADIUS, RADIUS);

    if (isChecked())
    {
        painter.setBrush(QBrush());
        QPen borderPen;
        borderPen.setWidth(BORDER_WIDTH);
        borderPen.setColor("#01bdff");
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(1, 1, this->width() - 2,
                                                    this->height() - 2), RADIUS, RADIUS);
    }
}

void ColorButton::setDisableColor(bool disable)
{
    m_disable = disable;
}

ColorButton::~ColorButton()
{
}

ColorPanel::ColorPanel(QWidget *parent)
    : QWidget(parent)
     , m_expand(false)
    , m_drawstatus(DrawStatus::Fill)
{
    DRAW_THEME_INIT_WIDGET("ColorPanel");
    if (!m_expand)
        setFixedSize(232, ORIGIN_HEIGHT);
    else
        setFixedSize(232, EXPAND_HEIGHT);

    m_colList = specifiedColorList();

    QButtonGroup* colorsButtonGroup = new QButtonGroup(this);
    colorsButtonGroup->setExclusive(true);

    QGridLayout* gLayout = new QGridLayout;
    gLayout->setVerticalSpacing(3);
    gLayout->setHorizontalSpacing(3);

    for(int i = 0; i < m_colList.length(); i++)
    {
        ColorButton* cb = new ColorButton(m_colList[i], this);
        if (i == 0)
            cb->setDisableColor(true);
        m_cButtonList.append(cb);
        gLayout->addWidget(cb, i/10, i%10);
        colorsButtonGroup->addButton(cb);
        qDebug() << "~~~" << i/10 << i%10;
        connect(cb, &ColorButton::colorButtonClicked, this, &ColorPanel::setConfigColor);
    }

    m_sliderLabel = new SliderLabel(tr("Alpha"), m_drawstatus,
                                    m_widgetStatus, this);
    m_sliderLabel->setFixedHeight(25);
    connect(m_sliderLabel, &SliderLabel::alphaChanged, this, [=](int value){
        if (m_widgetStatus != MiddleWidgetStatus::DrawText)
        {
            if (m_drawstatus == DrawStatus::Stroke)
            {
                ConfigSettings::instance()->setValue("common", "strokeColor_alpha", value);
            } else {
                ConfigSettings::instance()->setValue("common", "fillColor_alpha", value);
            }
        } else {
            ConfigSettings::instance()->setValue("text", "fillColor_alpha", value);
        }
    });
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            [=](const QString &group, const QString &key){
        Q_UNUSED(group);
        if (key == "fillColor_alpha" || key == "strokeColor_alpha")
        {
            m_sliderLabel->updateDrawStatus(m_drawstatus, m_widgetStatus);
        }
    });

    m_editLabel = new EditLabel(this);
    m_editLabel->setTitle(tr("Color"));
    m_editLabel->setEditWidth(150);

    m_colorfulBtn = new PushButton(this);
    m_colorfulBtn->setObjectName("ColorFulButton");

    QHBoxLayout* colorLayout = new QHBoxLayout;
    colorLayout->setMargin(0);
    colorLayout->setSpacing(0);
    colorLayout->addStretch();
    colorLayout->addWidget(m_editLabel);
    colorLayout->addSpacing(3);
    colorLayout->addWidget(m_colorfulBtn);


    PickColorWidget* pickColWidget = new PickColorWidget(this);
    connect(this, &ColorPanel::resetColorButtons, this, [=]{
        colorsButtonGroup->setExclusive(false);
        foreach (ColorButton* cb, m_cButtonList) {
            cb->setChecked(false);
        }
        colorsButtonGroup->setExclusive(true);
    });
    connect(pickColWidget, &PickColorWidget::pickedColor, this,
            [=](QColor color)
    {
        Q_UNUSED(color);
        emit resetColorButtons();
    });

    connect(pickColWidget, &PickColorWidget::pickedColor, this,
        &ColorPanel::setConfigColor);

    connect(m_editLabel, &EditLabel::editTextChanged,  this, [=](QString text){
        if (QColor(text).isValid())
        {
            pickColWidget->setRgbValue(QColor(text));
        }
    });

    QVBoxLayout* mLayout = new QVBoxLayout(this);
    mLayout->setContentsMargins(4, 4, 4, 4);
    mLayout->setSpacing(0);
    mLayout->addLayout(gLayout);
    mLayout->addSpacing(5);
    mLayout->addWidget(m_sliderLabel);
    mLayout->addSpacing(2);
    mLayout->addLayout(colorLayout);
    mLayout->addSpacing(5);
    mLayout->addWidget(pickColWidget);
    setLayout(mLayout);

    if (!m_expand)
        pickColWidget->hide();

    connect(m_colorfulBtn, &PushButton::clicked, this, [=]{
        if (m_expand)
        {
            pickColWidget->hide();
            setFixedHeight(ORIGIN_HEIGHT);
            updateGeometry();
        } else
        {
            pickColWidget->show();
            setFixedHeight(EXPAND_HEIGHT);
            updateGeometry();
        }
        emit updateHeight();

        m_expand = !m_expand;
    });
}

void ColorPanel::setColor(QColor color)
{
    Q_UNUSED(color);
}

void ColorPanel::setDrawStatus(DrawStatus status)
{
    m_drawstatus = status;

    QString colorName;
    if (m_drawstatus == DrawStatus::Stroke)
    {
        colorName = ConfigSettings::instance()->value("common",
                                                      "strokeColor").toString();
    } else
    {
        colorName = ConfigSettings::instance()->value("common",
                                                      "fillColor").toString();
    }
    m_editLabel->setEditText(colorName);
    m_sliderLabel->updateDrawStatus(m_drawstatus, m_widgetStatus);
}

void ColorPanel::setConfigColor(QColor color)
{
    m_editLabel->setEditText(color.name());
    if (m_widgetStatus != MiddleWidgetStatus::DrawText)
    {
        if (m_drawstatus == DrawStatus::Stroke)
        {
            qDebug() << "setConfigColor" << color.name();
            updateConfigByWidget("common", "strokeColor", color);
        } else {
            updateConfigByWidget("common", "fillColor", color);
        }
    } else {
        updateConfigByWidget("text", "fillColor", color);
    }
}

void ColorPanel::updateConfigByWidget(const QString &group,
                                      const QString &key, QColor color)
{
    if (color ==  QColor(Qt::transparent))
    {
        ConfigSettings::instance()->setValue(group,
                                             QString("%1_transparent").arg(key), true);
    } else {
        ConfigSettings::instance()->setValue(group,
                                             QString("%1_transparent").arg(key), false);
        ConfigSettings::instance()->setValue(group, key,  color.name());
        qDebug() << "updateCofigByWidget:" << group << key << color.name();
        m_sliderLabel->setAlpha(100);
    }
}

void ColorPanel::updateColorAlpha(DrawStatus status,
                                  MiddleWidgetStatus widgetStatus)
{
    m_sliderLabel->updateDrawStatus(status, widgetStatus);
}

void ColorPanel::setMiddleWidgetStatus(MiddleWidgetStatus status)
{
    m_widgetStatus = status;
}

void ColorPanel::updateColorButtonStatus()
{
    if (m_widgetStatus != MiddleWidgetStatus::DrawText)
    {
        if (m_drawstatus == DrawStatus::Stroke)
        {
            updateColorBtnByWidget("common", "strokeColor");
        } else {
            updateColorBtnByWidget("common", "fillColor");
        }
    } else {
        updateColorBtnByWidget("text", "fillColor");
    }
}

void ColorPanel::updateColorBtnByWidget(const QString &group,
                                        const QString &key)
{
    bool transColorBtnChecked = ConfigSettings::instance()->value(
                group, QString("%1_transparent").arg(key)).toBool();
    if (transColorBtnChecked)
    {
        m_cButtonList[0]->setChecked(true);
    } else {
        const QString colorName = ConfigSettings::instance()->value(group,
                                                                    QString("%1").arg(key)).toString();
        if (m_colList.contains(colorName))
        {
            m_cButtonList[m_colList.indexOf(colorName)]->setChecked(true);
        } else {
            resetColorButtons();
        }
    }
}

ColorPanel::~ColorPanel()
{
}
