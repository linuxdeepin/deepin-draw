#include "colorpanel.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QDebug>

#include "utils/global.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"
#include "ccheckbutton.h"
#include "colorlabel.h"
#include "colorslider.h"
#include "pickcolorwidget.h"
#include "calphacontrolwidget.h"


const int ORGIN_WIDTH = 250;
const int PANEL_WIDTH = 230;
const int ORIGIN_HEIGHT = 213;
const int EXPAND_HEIGHT = 430;
const int RADIUS = 3;
const int BORDER_WIDTH = 2;
const QSize COLOR_BORDER_SIZE = QSize(20, 20);
//const QSize COLOR_BUTTN = QSize(14, 14);
//const QSize SLIDER_SIZE = QSize(178, 22);
const QSize BTN_SIZE = QSize(24, 24);


ColorButton::ColorButton(const QColor &color, DWidget *parent)
    : DPushButton(parent)
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

ColorPanel::ColorPanel(DWidget *parent)
    : DWidget(parent)
    , m_drawstatus(DrawStatus::Fill)
    , m_expand(false)
{
    initUI();
    initConnection();
}

ColorPanel::~ColorPanel()
{

}

////颜色按钮点击处理
void ColorPanel::setConfigColor(QColor color)
{
    /// 颜色名字
    m_colLineEdit->setText(color.name());
    /// 颜色Alpha值
    m_alphaControlWidget->updateAlphaControlWidget(color.alpha());

    ///写入参数
    if (m_drawstatus == Fill) {
        CDrawParamSigleton::GetInstance()->setFillColor(color);
    } else if (m_drawstatus == Stroke) {
        CDrawParamSigleton::GetInstance()->setLineColor(color);
    } else if (m_drawstatus == TextFill) {
        CDrawParamSigleton::GetInstance()->setTextColor(color);
    }

    emit signalColorChanged();
}

void ColorPanel::slotPickedColorChanged(QColor newColor)
{
    resetColorBtn();
    setConfigColor(newColor);
}

void ColorPanel::initUI()
{
    DWidget *colorBtnWidget = new DWidget(this);
    colorBtnWidget->setFixedSize(ORGIN_WIDTH, ORIGIN_HEIGHT);

    if (!m_expand)
        setFixedSize(ORGIN_WIDTH, ORIGIN_HEIGHT);
    else
        setFixedSize(ORGIN_WIDTH, EXPAND_HEIGHT);

    m_colList = specifiedColorList();

    m_colorsButtonGroup = new QButtonGroup(this);
    m_colorsButtonGroup->setExclusive(true);

    QGridLayout *gLayout = new QGridLayout;
    gLayout->setVerticalSpacing(3);
    gLayout->setHorizontalSpacing(3);

    for (int i = 0; i < m_colList.length(); i++) {
        ColorButton *cb = new ColorButton(m_colList[i], this);
        if (i == 0)
            cb->setDisableColor(true);
        m_cButtonList.append(cb);
        gLayout->addWidget(cb, i / 10, i % 10);
        m_colorsButtonGroup->addButton(cb);
    }

    m_alphaControlWidget = new CAlphaControlWidget(this);
    m_alphaControlWidget->setFixedHeight(24);
//    m_alphaControlWidget->setStyleSheet("background-color: rgb(255, 0, 0);");

    DWidget *colorValueWidget = new DWidget;
    colorValueWidget->setFixedWidth(PANEL_WIDTH);
    DLabel *colLabel = new DLabel(colorValueWidget);
    colLabel->setObjectName("ColorLabel");
    colLabel->setFixedWidth(48);
    colLabel->setText(tr("Color"));

    m_colLineEdit = new DLineEdit(colorValueWidget);
    m_colLineEdit->setObjectName("ColorLineEdit");
    m_colLineEdit->setFixedSize(145, 24);

    QMap<CCheckButton::EButtonSattus, QString> pictureMap;
    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/draw/color_more_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/draw/color_more_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/draw/color_more_active.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/draw/color_more_active.svg");
    m_colorfulBtn = new CCheckButton(pictureMap, colorValueWidget, false);
    m_colorfulBtn->setObjectName("ColorFulButton");
    m_colorfulBtn->setFixedSize(BTN_SIZE);

    QHBoxLayout *colorLayout = new QHBoxLayout(colorValueWidget);
//    colorValueWidget->setStyleSheet("background-color: rgb(0, 255, 0);");
    colorLayout->setMargin(0);
    colorLayout->setSpacing(0);
    colorLayout->addWidget(colLabel);
    colorLayout->addWidget(m_colLineEdit);
    colorLayout->addSpacing(10);
    colorLayout->addWidget(m_colorfulBtn);

    m_pickColWidget = new PickColorWidget(this);

    QVBoxLayout *vLayout = new QVBoxLayout(colorBtnWidget);
    vLayout->setContentsMargins(4, 4, 7, 4);
    vLayout->setSpacing(0);
    vLayout->addSpacing(7);
    vLayout->addLayout(gLayout);
    vLayout->addSpacing(9);
    vLayout->addWidget(m_alphaControlWidget, 0, Qt::AlignCenter);
    vLayout->addSpacing(9);
    vLayout->addWidget(colorValueWidget, 0, Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(colorBtnWidget);
    layout->addWidget(m_pickColWidget, 0, Qt::AlignCenter);

    if (!m_expand)
        m_pickColWidget->hide();
}

void ColorPanel::initConnection()
{
    ///颜色按钮
    foreach (ColorButton *cb, m_cButtonList) {
        connect(cb, &ColorButton::colorButtonClicked, this, &ColorPanel::setConfigColor);
    }

    ///调试板改变
    connect(m_pickColWidget, &PickColorWidget::pickedColor, this, &ColorPanel::slotPickedColorChanged);

    connect(m_colLineEdit, &DLineEdit::textChanged,  this, [ = ](QString text) {
        if (QColor(text).isValid()) {
            m_pickColWidget->setRgbValue(QColor(text));
        }
    });


    ///Alpha
    connect(m_alphaControlWidget, &CAlphaControlWidget::signalAlphaChanged, this, [ = ] (int alphaValue) {
        QColor tmpColor;
        if (m_drawstatus == Fill) {
            tmpColor = CDrawParamSigleton::GetInstance()->getFillColor();
            tmpColor.setAlpha(alphaValue);
            CDrawParamSigleton::GetInstance()->setFillColor(tmpColor);

        } else if (m_drawstatus == Stroke) {
            tmpColor = CDrawParamSigleton::GetInstance()->getLineColor();
            tmpColor.setAlpha(alphaValue);
            CDrawParamSigleton::GetInstance()->setLineColor(tmpColor);
        } else if (m_drawstatus == TextFill) {
            tmpColor = CDrawParamSigleton::GetInstance()->getTextColor();
            tmpColor.setAlpha(alphaValue);
            CDrawParamSigleton::GetInstance()->setTextColor(tmpColor);
        }
        emit signalColorChanged();
    });

    ///展开按钮
    connect(m_colorfulBtn, &CCheckButton::buttonClick, this, [ = ] {
        if (m_expand)
        {
            m_pickColWidget->hide();
            m_pickColWidget->setPickedColor(false);
            setFixedHeight(ORIGIN_HEIGHT);
            updateGeometry();
        } else
        {
            m_pickColWidget->show();
            m_pickColWidget->setPickedColor(true);
            setFixedHeight(EXPAND_HEIGHT);
            updateGeometry();
        }
        emit updateHeight();

        m_expand = !m_expand;
    });
}

void ColorPanel::resetColorBtn()
{
    m_colorsButtonGroup->setExclusive(false);
    foreach (ColorButton *cb, m_cButtonList) {
        cb->setChecked(false);
    }
    m_colorsButtonGroup->setExclusive(true);
}


void ColorPanel::updateColorPanel(DrawStatus status)
{
    m_drawstatus = status;

    QColor configColor;
    if (m_drawstatus == DrawStatus::Fill) {
        configColor = CDrawParamSigleton::GetInstance()->getFillColor();
    } else if (m_drawstatus == DrawStatus::Stroke) {
        configColor = CDrawParamSigleton::GetInstance()->getLineColor();
    } else if (m_drawstatus == DrawStatus::TextFill) {
        configColor = CDrawParamSigleton::GetInstance()->getTextColor();
    }

    ////更新颜色按钮
    if (m_colList.contains(configColor)) {
        m_cButtonList[m_colList.indexOf(configColor)]->setChecked(true);
    } else {
        resetColorBtn();
    }

    ///更新颜色名字
    m_colLineEdit->setText(configColor.name());
    /// 颜色Alpha值
    m_alphaControlWidget->updateAlphaControlWidget(configColor.alpha());
}
