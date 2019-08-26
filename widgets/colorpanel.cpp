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

const int ORGIN_WIDTH = 250;
const int PANEL_WIDTH = 222;
const int ORIGIN_HEIGHT = 213;
const int EXPAND_HEIGHT = 430;
const int RADIUS = 3;
const int BORDER_WIDTH = 2;
const QSize COLOR_BORDER_SIZE = QSize(20, 20);
const QSize COLOR_BUTTN = QSize(14, 14);
const QSize SLIDER_SIZE = QSize(178, 22);
const QSize BTN_SIZE = QSize(24, 24);


ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QPushButton(parent)
    , m_disable(false)
{
    m_color = color;
    setFixedSize(COLOR_BORDER_SIZE);
    setCheckable(true);

    connect(this, &ColorButton::clicked, this, [ = ] {
        setChecked(true);
        if (m_disable)
        {
            emit colorButtonClicked(Qt::transparent);
        } else
        {
            emit colorButtonClicked(m_color);
        }
    });
}

void ColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(m_color);

    if (m_color == QColor(Qt::transparent)) {
        painter.drawPixmap(QRect(3, 3, this->width() - 6, this->height() - 6),
                           QPixmap(":/theme/light/images/draw/color_disable_active.png"));
        if (isChecked()) {
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

    if (isChecked()) {
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
    QWidget *colorBtnWidget = new QWidget(this);
    colorBtnWidget->setFixedSize(ORGIN_WIDTH, ORIGIN_HEIGHT);

    if (!m_expand)
        setFixedSize(ORGIN_WIDTH, ORIGIN_HEIGHT);
    else
        setFixedSize(ORGIN_WIDTH, EXPAND_HEIGHT);

    m_colList = specifiedColorList();

    QButtonGroup *colorsButtonGroup = new QButtonGroup(this);
    colorsButtonGroup->setExclusive(true);

    QGridLayout *gLayout = new QGridLayout;
    gLayout->setVerticalSpacing(3);
    gLayout->setHorizontalSpacing(3);

    for (int i = 0; i < m_colList.length(); i++) {
        ColorButton *cb = new ColorButton(m_colList[i], this);
        if (i == 0)
            cb->setDisableColor(true);
        m_cButtonList.append(cb);
        gLayout->addWidget(cb, i / 10, i % 10);
        colorsButtonGroup->addButton(cb);
//        qDebug() << "~~~" << i/10 << i%10;
        connect(cb, &ColorButton::colorButtonClicked, this, &ColorPanel::setConfigColor);
    }

    m_sliderLabel = new SliderLabel(tr("Alpha"), m_drawstatus, m_widgetStatus, this);
    connect(m_sliderLabel, &SliderLabel::alphaChanged, this, [ = ](int value) {
//        if (m_widgetStatus != EDrawToolMode::DrawText) {
//            if (m_drawstatus == DrawStatus::Stroke) {
//                ConfigSettings::instance()->setValue("common", "strokeColor_alpha", value);
//            } else {
//                ConfigSettings::instance()->setValue("common", "fillColor_alpha", value);
//            }
//        } else {
//            ConfigSettings::instance()->setValue("text", "fillColor_alpha", value);
//        }
    });
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
    [ = ](const QString & group, const QString & key) {
        Q_UNUSED(group);
        if (key == "fillColor_alpha" || key == "strokeColor_alpha") {
            m_sliderLabel->updateDrawStatus(m_drawstatus, m_widgetStatus);
        }
    });

    QWidget *colorValueWidget = new QWidget;
    colorValueWidget->setFixedWidth(PANEL_WIDTH);
    QLabel *colLabel = new QLabel(colorValueWidget);
    colLabel->setObjectName("ColorLabel");
    colLabel->setFixedWidth(48);
    colLabel->setText(tr("Color"));

    m_colLineEdit = new QLineEdit(colorValueWidget);
    m_colLineEdit->setObjectName("ColorLineEdit");
    m_colLineEdit->setFixedSize(145, 24);
    m_colorfulBtn = new PushButton(colorValueWidget);
    m_colorfulBtn->setObjectName("ColorFulButton");
    m_colorfulBtn->setFixedSize(BTN_SIZE);

    QHBoxLayout *colorLayout = new QHBoxLayout(colorValueWidget);
    colorLayout->setMargin(0);
    colorLayout->setSpacing(0);
    colorLayout->addWidget(colLabel);
    colorLayout->addWidget(m_colLineEdit);
    colorLayout->addSpacing(21);
    colorLayout->addWidget(m_colorfulBtn);

    PickColorWidget *pickColWidget = new PickColorWidget(this);
    connect(this, &ColorPanel::resetColorButtons, this, [ = ] {
        colorsButtonGroup->setExclusive(false);
        foreach (ColorButton *cb, m_cButtonList)
        {
            cb->setChecked(false);
        }
        colorsButtonGroup->setExclusive(true);
    });
    connect(pickColWidget, &PickColorWidget::pickedColor, this,
    [ = ](QColor color) {
        Q_UNUSED(color);
        emit resetColorButtons();
    });

    connect(pickColWidget, &PickColorWidget::pickedColor, this,
            &ColorPanel::setConfigColor);

    connect(m_colLineEdit, &QLineEdit::textChanged,  this, [ = ](QString text) {
        if (QColor(text).isValid()) {
            pickColWidget->setRgbValue(QColor(text));
        }
    });

    QVBoxLayout *vLayout = new QVBoxLayout(colorBtnWidget);
    vLayout->setContentsMargins(4, 4, 7, 4);
    vLayout->setSpacing(0);
    vLayout->addSpacing(7);
    vLayout->addLayout(gLayout);
    vLayout->addSpacing(9);
    vLayout->addWidget(m_sliderLabel, 0, Qt::AlignCenter);
    vLayout->addSpacing(14);
    vLayout->addWidget(colorValueWidget, 0, Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(colorBtnWidget);
    layout->addWidget(pickColWidget, 0, Qt::AlignCenter);

    if (!m_expand)
        pickColWidget->hide();

    connect(m_colorfulBtn, &PushButton::clicked, this, [ = ] {
        if (m_expand)
        {
            pickColWidget->hide();
            pickColWidget->setPickedColor(false);
            setFixedHeight(ORIGIN_HEIGHT);
            updateGeometry();
        } else
        {
            pickColWidget->show();
            pickColWidget->setPickedColor(true);
            setFixedHeight(EXPAND_HEIGHT);
            updateGeometry();
        }
        emit updateHeight();

        m_expand = !m_expand;
    });
}

void ColorPanel::setDrawStatus(DrawStatus status)
{
    m_drawstatus = status;

    QString colorName;
    if (m_drawstatus == DrawStatus::Stroke) {
        colorName = ConfigSettings::instance()->value("common",
                                                      "strokeColor").toString();
    } else {
        colorName = ConfigSettings::instance()->value("common",
                                                      "fillColor").toString();
    }
    m_colLineEdit->setText(colorName);
    m_sliderLabel->updateDrawStatus(m_drawstatus, m_widgetStatus);
}

void ColorPanel::setConfigColor(QColor color)
{
    m_colLineEdit->setText(color.name());
    m_sliderLabel->setAlpha(color.alpha());
}


void ColorPanel::setMiddleWidgetStatus(EDrawToolMode status)
{
    m_widgetStatus = status;
}

void ColorPanel::updateColorButtonStatus()
{
    ///////////////?????????????
//    if (m_widgetStatus != EDrawToolMode::DrawText) {
//        if (m_drawstatus == DrawStatus::Stroke) {
//            updateColorBtnByWidget("common", "strokeColor");
//        } else {
//            updateColorBtnByWidget("common", "fillColor");
//        }
//    } else {
//        updateColorBtnByWidget("text", "fillColor");
//    }
}

void ColorPanel::updateColorBtnByWidget(const QString &group, const QString &key)
{
    bool transColorBtnChecked = ConfigSettings::instance()->value(
                                    group, QString("%1_transparent").arg(key)).toBool();
    if (transColorBtnChecked) {
        m_cButtonList[0]->setChecked(true);
    } else {
        const QString colorName = ConfigSettings::instance()->value(group, QString("%1").arg(key)).toString();
        if (m_colList.contains(colorName)) {
            m_cButtonList[m_colList.indexOf(colorName)]->setChecked(true);
        } else {
            resetColorButtons();
        }
    }
}

ColorPanel::~ColorPanel()
{
}
