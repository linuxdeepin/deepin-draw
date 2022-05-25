/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ccolorpanel.h"
#include "utils/global.h"
#include "utils/baseutils.h"
#include "pagecontext.h"
#include "pageview.h"
#include "ciconbutton.h"
//#include "colorlabel.h"
#include "colorslider.h"
#include "cpickcolorwidget.h"
//#include "calphacontrolwidget.h"
//#include "ccolorpickwidget.h"
#include "cnumbersliderwidget.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QDebug>
#include <QRegExpValidator>
#include <QTimer>
#include <QGraphicsEffect>
#include <DWidget>

#ifdef USE_DTK
#include <DGuiApplicationHelper>
#include <DWindowManagerHelper>
#include <DArrowLineExpand>
#include <DArrowLineDrawer>

DGUI_USE_NAMESPACE
#endif

const int ORGIN_WIDTH = 324;
const int PANEL_WIDTH = 334;
//const int ORIGIN_HEIGHT = 250;
const int ORIGIN_HEIGHT = 56;
const int EXPAND_HEIGHT = 475;
const int RADIUS = 8;
const QSize COLOR_BORDER_SIZE = QSize(34, 34);


class ColorPanel::ColorPanel_private
{
public:
    explicit ColorPanel_private(ColorPanel *qq, bool bOldStyle): q(qq), m_bOldStyle(bOldStyle) {}

    void initUI()
    {
        if (m_bOldStyle) {
            initUiOld();
        } else {
            intiUiNew();
        }
    }

    void initUiOld()
    {
        m_colList = specifiedColorList();
        m_colorsButtonGroup = new QButtonGroup(q);
        m_colorsButtonGroup->setExclusive(true);

        DArrowLineExpand *expand = new DArrowLineExpand;
        expand->setTitle(tr("color panel"));
        expand->setAnimationDuration(300);
        expand->setExpand(true);
        connect(expand, &DArrowLineExpand::expandChange, q, [ = ] {
            updateExpendArea();
        });

        QGridLayout *gLayout = new QGridLayout;
        gLayout->setVerticalSpacing(4);
        gLayout->setHorizontalSpacing(4);

        for (int i = 0; i < m_colList.length(); i++) {
            CColorButton *cb = new CColorButton(m_colList[i], q);
            setWgtAccesibleName(cb, QString("Panel %1 pushbutton").arg(cb->color().name()));
            cb->setFocusPolicy(Qt::NoFocus);
            m_cButtonList.append(cb);
            gLayout->addWidget(cb, i / 8, i % 8);
            m_colorsButtonGroup->addButton(cb, i);
        }

        DWidget *colorValueWidget = new DWidget(q);
        colorValueWidget->setFocusPolicy(Qt::NoFocus);
        colorValueWidget->setFixedWidth(PANEL_WIDTH);
        DLabel *colLabel = new DLabel(colorValueWidget);
        QFont colLabelFont = colLabel->font();
        colLabelFont.setPixelSize(13);
        colLabel->setFixedWidth(35);
        colLabel->setText(tr("Color"));
        colLabel->setFont(colLabelFont);
        colLabel->hide();

        m_colLineEdit = new DLineEdit(colorValueWidget);
        m_colLineEdit->setObjectName("ColorLineEdit");
        m_colLineEdit->setFixedSize(180, 36);
        m_colLineEdit->setClearButtonEnabled(false);
        m_colLineEdit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[0-9A-Fa-f]{6}"), q));
        m_colLineEdit->setText("ffffff");
        m_colLineEdit->hide();

        QMap<int, QMap<CIconButton::EIconButtonSattus, QString>> pictureMap;
        pictureMap[DGuiApplicationHelper::LightType][CIconButton::Normal] = QString(":/theme/light/images/draw/palette_normal.svg");
        pictureMap[DGuiApplicationHelper::LightType][CIconButton::Hover] = QString(":/theme/light/images/draw/palette_normal.svg");
        pictureMap[DGuiApplicationHelper::LightType][CIconButton::Press] = QString(":/theme/light/images/draw/palette_normal.svg");
        pictureMap[DGuiApplicationHelper::LightType][CIconButton::Active] = QString(":/theme/light/images/draw/palette_normal.svg");

        pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Normal] = QString(":/theme/dark/images/draw/palette_normal.svg");
        pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Hover] = QString(":/theme/dark/images/draw/palette_normal.svg");
        pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Press] = QString(":/theme/dark/images/draw/palette_normal.svg");
        pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Active] = QString(":/theme/dark/images/draw/palette_normal.svg");

        m_colorfulBtn = new CIconButton(pictureMap, QSize(55, 36), colorValueWidget, false);
        m_colorfulBtn->setObjectName("CIconButton");
        m_colorfulBtn->setFocusPolicy(Qt::NoFocus);
        m_colorfulBtn->hide();

        m_pickColWidget = new PickColorWidget(q, true);
        m_pickColWidget->setObjectName("PickColorWidget");
        m_pickColWidget->setFocusPolicy(Qt::NoFocus);

        QWidget *w = new QWidget(q);
        w->setLayout(gLayout);
        expand->setContent(w);

        QVBoxLayout *layout = new QVBoxLayout(q);
        layout->setAlignment(Qt::AlignTop);
        layout->setMargin(0);
        layout->setSpacing(10);
        layout->addSpacing(20);
        layout->addWidget(expand);
        layout->addWidget(m_pickColWidget, 0, Qt::AlignCenter);

        connect(m_pickColWidget, &PickColorWidget::heightChanged, q, &ColorPanel::sizeChanged);
        connect(q, &ColorPanel::sizeChanged, q, [ = ] {
            updateExpendArea();
        });
        s_expand = true;
        this->updateExpendArea();
    }

    void intiUiNew()
    {
        colorBtnWidget = new QWidget(q);
        QHBoxLayout *gLayout = new QHBoxLayout(colorBtnWidget);
        gLayout->setContentsMargins(0, 0, 0, 0);
        gLayout->setSpacing(4);

        m_colList = specifiedColorList2();
        m_colorsButtonGroup = new QButtonGroup(q);
        m_colorsButtonGroup->setExclusive(true);

        for (int i = 0; i < m_colList.length(); i++) {
            CColorButton *cb = new CColorButton(m_colList[i], q);
            setWgtAccesibleName(cb, QString("Panel %1 pushbutton").arg(cb->color().name()));
            cb->setFocusPolicy(Qt::NoFocus);
            m_cButtonList.append(cb);
            gLayout->addWidget(cb);
            m_colorsButtonGroup->addButton(cb, i);
        }

        int styleA = 0;
        int styleB = 1;
#ifdef USE_DTK
        styleA = DGuiApplicationHelper::LightType;
        styleB = DGuiApplicationHelper::DarkType;
#endif

        QMap<int, QMap<CIconButton::EIconButtonSattus, QString>> pictureMap;
        pictureMap[styleA][CIconButton::Normal] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");
        pictureMap[styleA][CIconButton::Hover] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");
        pictureMap[styleA][CIconButton::Press] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");
        pictureMap[styleA][CIconButton::Active] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");

        pictureMap[styleB][CIconButton::Normal] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");
        pictureMap[styleB][CIconButton::Hover] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");
        pictureMap[styleB][CIconButton::Press] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");
        pictureMap[styleB][CIconButton::Active] = QString(":/icons/deepin/builtin/texts/palette_normal.svg");

        m_colorfulBtn = new CIconButton(pictureMap, QSize(/*55*/36, 36), colorBtnWidget, false);
        m_colorfulBtn->setObjectName("CIconButton");
        m_colorfulBtn->setFocusPolicy(Qt::NoFocus);
        m_colorfulBtn->setToolTip(tr("More color"));

        gLayout->addWidget(m_colorfulBtn);

        m_pickColWidget = new PickColorWidget(q);
        m_pickColWidget->setObjectName("PickColorWidget");
        m_pickColWidget->setFocusPolicy(Qt::NoFocus);

        QVBoxLayout *layout = new QVBoxLayout(q);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(m_pickColWidget, 0, Qt::AlignCenter);
        layout->addWidget(colorBtnWidget, 0, Qt::AlignCenter);

        updateExpendArea();
    }
    void initConnection()
    {
        //1.颜色按钮组
        connect(m_colorsButtonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
        q, [ = ](QAbstractButton * pBtn, bool checked) {
            CColorButton *pColorBtn = qobject_cast<CColorButton *>(pBtn);
            if (checked) {
                q->setColor(pColorBtn->color());
            }
        });

        //2.pick板颜色设置完成
        connect(m_pickColWidget, QOverload<const QColor &>::of(&PickColorWidget::colorChanged), q, [ = ](const QColor & color) {
            q->setColor(color);
        });

        //3.pick板颜色预览
        connect(m_pickColWidget, &PickColorWidget::previewedColorChanged, q, [ = ](const QColor & color) {
            q->setColor(color, true, EChangedUpdate);
        });


        //展开按钮
        connect(m_colorfulBtn, &CIconButton::buttonClick, q, [ = ] {
            s_expand = !s_expand;
            updateExpendArea(true);
        });

        connect(m_pickColWidget, QOverload<const QColor &, EChangedPhase>::of(&PickColorWidget::colorChanged), q,
                &ColorPanel::colorChanged);
    }

    void updateColor(const QColor &previewColor)
    {
        QColor c = previewColor.isValid() ? previewColor : curColor;

        //1.检查当前颜色是否是颜色组的颜色值
        int id = m_colList.indexOf(QColor(c.red(), c.green(), c.blue()));
        if (id != -1) {
            m_colorsButtonGroup->blockSignals(true);
            m_colorsButtonGroup->button(id)->setChecked(true);
            m_colorsButtonGroup->blockSignals(false);
        } else {
            //没有对应颜色，就不选中
            auto pBtn = m_colorsButtonGroup->checkedButton();
            m_colorsButtonGroup->setExclusive(false);
            if (nullptr != pBtn) {
                pBtn->setChecked(false);
            }
            m_colorsButtonGroup->setExclusive(true);
        }

        //2.pick界面刷新设置为该颜色
        m_pickColWidget->setColor(c, false);
        m_pickColWidget->setAlpha(c.alpha(), false);
        m_colorfulBtn->setCenterColor(c);
        m_colorfulBtn->update();
    }

    void updateExpendArea(bool updateByExtend = false)
    {
#ifndef Q_OS_WIN
        if (s_expand) {
            m_pickColWidget->setAttribute(Qt::WA_UpdatesDisabled);
        }
#endif

        if (!s_expand) {
            m_pickColWidget->hide();
        } else {
            m_pickColWidget->show();
        }

        {
            q->updateGeometry();
            QWidget *tpWin = q;
            while (auto w = tpWin->parentWidget()) {
                w->updateGeometry();
                tpWin = w;
                if (w->isTopLevel()) {
                    break;
                }
            }
#ifndef Q_OS_WIN
            if (s_expand && tpWin->graphicsEffect() != nullptr)
                tpWin->graphicsEffect()->setEnabled(false);
#endif

            if (tpWin->isTopLevel()) {
                tpWin->adjustSize();
            }

#ifndef Q_OS_WIN
            if (s_expand) {
                QMetaObject::invokeMethod(q, [ = ]() {
                    if (tpWin->graphicsEffect() != nullptr) {
                        tpWin->graphicsEffect()->setEnabled(true);
                        tpWin->graphicsEffect()->update();
                        tpWin->update();
                    }
                    m_pickColWidget->setAttribute(Qt::WA_UpdatesDisabled, false);
                    m_pickColWidget->update();
                }, Qt::QueuedConnection);
            }
#endif

        }
    }

    /* pick widget 十六进制颜色编辑 */
    DLineEdit *m_colLineEdit = nullptr;

    /* pick widget 开关按钮 */
    CIconButton *m_colorfulBtn = nullptr;

    /* pick color 界面*/
    PickColorWidget *m_pickColWidget = nullptr;
    /* 默认的颜色组 */
    QList<CColorButton *> m_cButtonList;
    QList<QColor> m_colList;
    QButtonGroup *m_colorsButtonGroup = nullptr;

    QWidget *colorBtnWidget = nullptr;

    /* 当前 pick widget 是否是展开的标记 */
    bool s_expand = false;

    /* 当前 颜色 */
    QColor curColor;

    ColorPanel *q;
    bool    m_bOldStyle;
};

CColorButton::CColorButton(const QColor &color, QWidget *parent)
    : QPushButton(parent)
    , m_color(color)
{
    resize(COLOR_BORDER_SIZE);
    setCheckable(true);

    connect(this, &CColorButton::clicked, this, [ = ]() {
        this->setChecked(true);
    });
}

void CColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setBrush(QBrush(m_color));
    QColor colorTrasparent(Qt::white);
    colorTrasparent.setAlpha(0);

    QPen pen;
    pen.setWidth(1);
    if (m_color == colorTrasparent) {
        pen.setColor(QColor(204, 204, 204));
    } else {
        pen.setColor(QColor(0, 0, 0, 55));
    }
    painter.setPen(pen);
    painter.drawRoundedRect(QRect(3, 3, this->width() - 6,
                                  this->height() - 6),
                            RADIUS, RADIUS);
    if (m_color == colorTrasparent) {
        pen.setColor(QColor(255, 128, 77));
        painter.setPen(pen);
        painter.drawLine(6, this->height() - 6, this->width() - 6, 6);
    }

    if (isChecked()) {
        painter.setBrush(QBrush());
        QPen borderPen;
        borderPen.setWidth(2);
        borderPen.setColor(QColor(0, 142, 255));
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(1, 1, this->width() - 2,
                                      this->height() - 2),
                                RADIUS, RADIUS);
    }
}

QColor CColorButton::color()
{
    return m_color;
}

CColorButton::~CColorButton()
{
}

ColorPanel::ColorPanel(QWidget *parent, bool bOldStyle)
    : QWidget(parent), ColorPanel_d(new ColorPanel_private(this, bOldStyle))
{
    d_ColorPanel()->initUI();
    d_ColorPanel()->initConnection();
}

ColorPanel::~ColorPanel()
{
}

void ColorPanel::setTheme(int theme)
{
    int themeType = theme;
    d_ColorPanel()->m_colorfulBtn->setTheme(themeType);
    d_ColorPanel()->m_pickColWidget->setTheme(themeType);
}

void ColorPanel::setColor(const QColor &c,
                          bool internalChanged,
                          EChangedPhase phase)
{
    if (phase == EChangedFinished || phase == EChanged)
        d_ColorPanel()->curColor = c;

    d_ColorPanel()->updateColor(c);

    if (internalChanged) {
        emit colorChanged(c, phase);
    }
}

QColor ColorPanel::color()
{
    return d_ColorPanel()->curColor;
}

//DLineEdit *ColorPanel::colorLineEditor()
//{
//    return m_colLineEdit;
//}

void ColorPanel::setExpandWidgetVisble(bool visble)
{
    d_ColorPanel()->updateExpendArea();
}

QSize ColorPanel::sizeHint() const
{
    return QSize(334, QWidget::sizeHint().height());
}

QSize ColorPanel::minimumSizeHint() const
{
    return QSize(334, QWidget::minimumSizeHint().height());
}

void ColorPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}


