/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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

#include "commonattributionregister.h"
#include "pageitem.h"
#include "drawboard.h"
#include "groupbuttonwidget.h"
#include "rotateattriwidget.h"
#include "orderwidget.h"
#include "attributemanager.h"
#include "pagescene.h"
#include "pageview.h"
#include "adjustmentatrriwidget.h"
#include "cundoredocommand.h"
#include "layeritem.h"
#include "styleattriwidget.h"
#include "colorstylewidget.h"
#include "csidewidthwidget.h"
#include "rectradiusstylewidget.h"
#include "cspinbox.h"
#include "sliderspinboxwidget.h"
#include "attributewidget.h"
#include "application.h"
#include "hboxlayoutwidget.h"

#include <QAbstractItemView>
#include <QPoint>
#include <QObject>
#include <QListView>
#include <QFontComboBox>

/**
 * @brief 用于默认设置的属性工具配置属性值，
 *      包含设置范围及默认值
 */
enum AttriDefaultValue {
    EStarAnchorMin = 3,         // 星形图元锚点默认值
    EStarAnchorMax = 50,
    EStarAnchorDefault = 5,

    EStarRadioMin = 0,          // 星形图元半径默认值
    EStarRadioMax = 100,
    EStarRadioDefault = 50,

    EPolygonSidesMin = 4,       // 多边形图元侧边数默认值
    EPolygonSidesMax = 10,
    EPolygonSidesDefault = 5,
};


/**
 * @brief 注册各项属性工具栏控件，注册仅调用一次
 */


void CommonAttributionRegister::registe()
{
    if (m_isInit) {
        return;
    }
    m_isInit = true;

    registeBaseStyleAttrri();
    registeGroupAttri();
    resgisteRotateAttri();
    registeOrderAttri();
    registeAdjustImageAttri();
    registeStarAnchorAttri();
    registeStarInnerOuterRadioAttri();
    registePolygonSidesAttri();
    registePenAttri();
    registeLineArrowAttri();

    connect(drawBoard(), qOverload<Page *>(&DrawBoard::currentPageChanged), this, [ = ](Page * page) {
        if (page->scene()->selectedItemCount() <= 0) {
            m_rotateAttri->setEnabled(false);
            m_groupWidget->setEnabled(false);
            m_orderAttri->setEnabled(false);
        }
    });

}

/**
 * @brief 注册属性工具栏群组工具
 */
void CommonAttributionRegister::registeGroupAttri()
{
    if (nullptr != m_groupWidget) {
        return;
    }

    m_groupWidget = new GroupButtonWidget;
    setWgtAccesibleName(m_groupWidget, "groupButtonWidget");
    drawBoard()->attributionManager()->installComAttributeWgt(EGroupWgt, m_groupWidget);

    connect(m_groupWidget, &GroupButtonWidget::buttonClicked, this, [ = ](bool doGroup, bool doUngroup) {
        auto currentPage = drawBoard()->currentPage();
        if (currentPage == nullptr)
            return;

        auto currentScene = currentPage->scene();
        if (doGroup) {
            auto tmpGroupItem = currentScene->creatGroup(currentScene->selectedPageItems(), nullptr);
            currentScene->clearSelections();
            currentScene->selectPageItem(tmpGroupItem);
        }

        if (doUngroup) {
            auto selectedItems = currentScene->selectedPageItems();
            if (!selectedItems.isEmpty()) {
                for (auto item : selectedItems) {
                    if (item->type() == GroupItemType) {
                        auto gp = static_cast<GroupItem *>(item);
                        currentScene->cancelGroup(gp);
                    }
                }
            }
        }
        //另外需要将焦点转移到text
        auto pView = drawBoard()->currentPage()->view();
        pView->setFocus();
    });

    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, this,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_groupWidget) {

            PageScene *curScene = drawBoard()->currentPage()->scene();
            if (nullptr == curScene) {
                return;
            }

            QList<QVariant> couple;
            bool canGroup = curScene->isGroupable(curScene->selectedPageItems());
            bool canUnGroup = curScene->isUnGroupable(curScene->selectedPageItems());

            m_groupWidget->setGroupFlag(canGroup, canUnGroup);
        }
    });
}

/**
 * @brief 注册属性工具栏旋转工具
 */
void CommonAttributionRegister::resgisteRotateAttri()
{
    if (nullptr != m_rotateAttri) {
        return;
    }

    m_rotateAttri = new RotateAttriWidget(drawBoard());
    setWgtAccesibleName(m_rotateAttri, "rotateAttriWidget");
    drawBoard()->attributionManager()->installComAttributeWgt(ERotProperty, m_rotateAttri, 0);
    // 关联旋转属性控件控制的角度变更信号，当图元角度变更时，更新属性值
    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::internalAttibutionUpdate, this,
    [ = ](int attris, const QVariant & var, int) {
        if (ERotProperty == attris) {
            m_rotateAttri->setVar(var);
        }
    });
}

/**
 * @brief 注册属性工具栏层级控制工具
 */
void CommonAttributionRegister::registeOrderAttri()
{
    if (nullptr != m_orderAttri) {
        return;
    }

    m_orderAttri = new OrderWidget(drawBoard());
    setWgtAccesibleName(m_orderAttri, "orderWidget");
    drawBoard()->attributionManager()->installComAttributeWgt(EOrderProperty, m_orderAttri, 0);
}

/**
 * @brief 注册属性工具栏图片自适应工具
 */
void CommonAttributionRegister::registeAdjustImageAttri()
{
    AdjustmentAtrriWidget *widget = new AdjustmentAtrriWidget;
    connect(widget->button(), &QPushButton::clicked, widget, [ = ]() {
        auto page = drawBoard()->currentPage();
        if (page == nullptr)
            return;

        auto curScene = page->scene();
        if (curScene != nullptr) {
            auto rect = curScene->selectedPageItems().first()->sceneBoundingRect();
            if (rect != curScene->sceneRect()) {
                UndoRecorder block(page->scene()->currentTopLayer(), LayerUndoCommand::RectChanged);
                page->scene()->currentTopLayer()->setRect(rect);
                page->view()->setSceneRect(rect);
            }
        }
    });

    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, this,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == widget) {
            widget->setEnabled(var.toBool());
        }
    });

    drawBoard()->attributionManager()->installComAttributeWgt(EImageAdaptScene, widget, false);
}

/**
 * @brief 注册属性工具栏基础样式工具，包括颜色，画笔宽度等
 */
void CommonAttributionRegister::registeBaseStyleAttrri()
{
    if (nullptr != m_fillBrushStyle
            || nullptr != m_borderPenStyle
            || nullptr != m_penWidth) {
        return;
    }

    m_fillBrushStyle = new ColorStyleWidget(drawBoard());
    m_fillBrushStyle->setColorFill(0);
    m_borderPenStyle = new ColorStyleWidget(drawBoard());
    m_borderPenStyle->setTitleText(tr("border"));
    m_borderPenStyle->setColorFill(1);
    m_borderPenStyle->setColorTextVisible(false);
    drawBoard()->attributionManager()->installComAttributeWgt(EBrushColor, m_fillBrushStyle, QColor(0, 0, 0));
    drawBoard()->attributionManager()->installComAttributeWgt(EPenColor, m_borderPenStyle, QColor(0, 0, 0));

    connect(m_fillBrushStyle, &ColorStyleWidget::colorChanged, this, [ = ](const QColor & color, int phase) {

        drawBoard()->setDrawAttribution(EBrushColor, color, phase);
    });

    connect(m_borderPenStyle, &ColorStyleWidget::colorChanged, this, [ = ](const QColor & color, int phase) {
        drawBoard()->setDrawAttribution(EPenColor, color, phase);
    });

    m_penWidth = new CSpinBox(drawBoard());
    m_penWidth->setEnabledEmbedStyle(true);
    m_penWidth->setSpinRange(0, 10);
    m_penWidth->setMinimumWidth(90);
    m_penWidth->setProperty(ChildAttriWidget, true);


    QObject::connect(m_penWidth, &CSpinBox::valueChanged, m_penWidth, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(phase)
        drawBoard()->setDrawAttribution(EPenWidth, value);
    });

    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, m_penWidth,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_penWidth) {
            QSignalBlocker bloker(m_penWidth);
            if (!var.isValid()) {
                m_penWidth->setSpecialText("...");
            } else {
                m_penWidth->setValue(var.toInt());
            }
        }
    });
    drawBoard()->attributionManager()->installComAttributeWgt(EPenWidth, m_penWidth, 2);

    //在border中显示
    m_borderPenStyle->addWidget(m_penWidth);

    m_rectRadius = new RectRadiusStyleWidget(drawBoard());
    drawBoard()->attributionManager()->installComAttributeWgt(m_rectRadius->attribution(), m_rectRadius, QVariant());
    QObject::connect(m_rectRadius, &RectRadiusStyleWidget::valueChanged, m_penWidth, [ = ](QVariant value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(m_rectRadius->attribution(), value, phase);
    });


    m_enablePenStyle = new CheckBoxSettingWgt("", m_borderPenStyle);
    m_enablePenStyle->setAttribution(EEnablePenStyle);
    drawBoard()->attributionManager()->installComAttributeWgt(m_enablePenStyle->attribution(), m_enablePenStyle, true);
    QObject::connect(m_enablePenStyle, &CheckBoxSettingWgt::checkChanged, drawBoard(), [ = ](bool value) {
        drawBoard()->setDrawAttribution(m_enablePenStyle->attribution(), value);
        m_borderPenStyle->setContentEnable(value);
    });

    QObject::connect(m_enablePenStyle, &CheckBoxSettingWgt::checkStatusChanged, drawBoard(), [ = ](int status) {
        m_borderPenStyle->setContentEnable(status != Qt::CheckState::Unchecked);
    });

    QObject::connect(m_borderPenStyle, &ColorStyleWidget::colorChanged, drawBoard(), [ = ] {
        if (m_enablePenStyle->checkBox()->isTristate())
        {
            emit m_enablePenStyle->checkChanged(true);
            m_enablePenStyle->checkBox()->setTristate(false);
            m_enablePenStyle->checkBox()->setChecked(true);
            m_enablePenStyle->checkBox()->update();
        }
    });

    m_enableBrushStyle = new CheckBoxSettingWgt("", m_fillBrushStyle);
    m_enableBrushStyle->setAttribution(EEnableBrushStyle);
    drawBoard()->attributionManager()->installComAttributeWgt(m_enableBrushStyle->attribution(), m_enableBrushStyle, true);
    QObject::connect(m_enableBrushStyle, &CheckBoxSettingWgt::checkChanged, drawBoard(), [ = ](bool value) {
        m_fillBrushStyle->setContentEnable(value);
        drawBoard()->setDrawAttribution(m_enableBrushStyle->attribution(), value);
    });

    QObject::connect(m_enableBrushStyle, &CheckBoxSettingWgt::checkStatusChanged, drawBoard(), [ = ](int status) {
        m_fillBrushStyle->setContentEnable(status != Qt::CheckState::Unchecked);
    });

    QObject::connect(m_fillBrushStyle, &ColorStyleWidget::colorChanged, drawBoard(), [ = ] {
        if (m_enableBrushStyle->checkBox()->isTristate())
        {
            emit m_enableBrushStyle->checkChanged(true);
            m_enableBrushStyle->checkBox()->setTristate(false);
            m_enableBrushStyle->checkBox()->setChecked(true);
            m_enableBrushStyle->checkBox()->update();
        }
    });


    m_borderPenStyle->setProperty(ChildAttriWidget, true);
    m_fillBrushStyle->setProperty(ChildAttriWidget, true);
    m_borderPenStyle->addTitleWidget(m_enablePenStyle, Qt::AlignRight);
    m_fillBrushStyle->addTitleWidget(m_enableBrushStyle, Qt::AlignRight);
}

/**
 * @brief 注册用于设置/更新显示星形图元锚点数量的属性工具栏
 */
void CommonAttributionRegister::registeStarAnchorAttri()
{
    if (nullptr != m_starAnchorAttri) {
        return;
    }

    m_starAnchorAttri = new SliderSpinBoxWidget(EStarAnchor);
    m_starAnchorAttri->setRange(EStarAnchorMin, EStarAnchorMax);
    m_starAnchorAttri->setVar(EStarAnchorDefault);
    m_starAnchorAttri->setTitle(tr("Vertices"));

    setWgtAccesibleName(m_starAnchorAttri, "starAnchorAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(EStarAnchor, m_starAnchorAttri, 5);

    connect(m_starAnchorAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EStarAnchor, value, phase);
    });
}

/**
 * @brief 注册用于设置/更新显示星形图元半径的属性工具栏
 */
void CommonAttributionRegister::registeStarInnerOuterRadioAttri()
{
    if (nullptr != m_starRadioAttri) {
        return;
    }

    m_starRadioAttri = new SliderSpinBoxWidget(EStarInnerOuterRadio, SliderSpinBoxWidget::EPercentStyle);
    m_starRadioAttri->setRange(EStarRadioMin, EStarRadioMax);
    m_starRadioAttri->setVar(EStarRadioDefault);
    m_starRadioAttri->setTitle(tr("Radius"));

    setWgtAccesibleName(m_starRadioAttri, "starInnerOuterRadio");
    drawBoard()->attributionManager()->installComAttributeWgt(EStarInnerOuterRadio, m_starRadioAttri, 5);

    connect(m_starRadioAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EStarInnerOuterRadio, value, phase);
    });
}

/**
 * @brief 注册用于设置/更新显示多边形图元侧边数量的属性工具栏
 */
void CommonAttributionRegister::registePolygonSidesAttri()
{
    if (nullptr != m_polygonSidesAttri) {
        return;
    }

    m_polygonSidesAttri = new SliderSpinBoxWidget(EPolygonSides);
    m_polygonSidesAttri->setRange(EPolygonSidesMin, EPolygonSidesMax);
    m_polygonSidesAttri->setVar(EPolygonSidesDefault);
    m_polygonSidesAttri->setTitle(tr("Sides"));

    setWgtAccesibleName(m_polygonSidesAttri, "polygonSidesAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(EPolygonSides, m_polygonSidesAttri, 5);

    connect(m_polygonSidesAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EPolygonSides, value, phase);
    });
}

void CommonAttributionRegister::registeLineArrowAttri()
{
    m_streakStyle = new HBoxLayoutWidget(drawBoard());
    m_comboxstart = new QComboBox(m_streakStyle);
    m_comboxend = new QComboBox(m_streakStyle);
    m_comboxstart->setFixedSize(QSize(110, 36));
    m_comboxstart->setIconSize(QSize(90, 36));
    m_comboxstart->setFocusPolicy(Qt::NoFocus);

    m_comboxend->setFixedSize(QSize(110, 36));
    m_comboxend->setIconSize(QSize(90, 36));
    m_comboxend->setFocusPolicy(Qt::NoFocus);

    QStringList icon_leftarry = {"ddc_none_arrow", "ddc_leftcircle_hollow", "ddc_leftcircle_solid"
                                 , "ddc_leftarrow_fine", "ddc_leftarrow_crude"
                                };
    QStringList icon_rightarry = {"ddc_none_arrow", "ddc_rightcircle_hollow", "ddc_rightcircle_solid"
                                  , "ddc_rightarrow_fine", "ddc_rightarrow_crude"
                                 };

    for (int i = 0; i < icon_leftarry.size(); ++i) {
        m_comboxstart->addItem(QIcon::fromTheme(icon_leftarry[i]), "");
        m_comboxend->addItem(QIcon::fromTheme(icon_rightarry[i]), "");
    }

    m_streakStyle->addWidget(m_comboxstart);
    m_streakStyle->addWidget(m_comboxend);
    m_comboxstart->setProperty(ChildAttriWidget, true);
    m_comboxend->setProperty(ChildAttriWidget, true);

    drawBoard()->attributionManager()->installComAttributeWgt(EStreakStyle, m_streakStyle);
    drawBoard()->attributionManager()->installComAttributeWgt(EStreakBeginStyle, m_comboxstart);
    drawBoard()->attributionManager()->installComAttributeWgt(EStreakEndStyle, m_comboxend);

    connect(m_comboxstart, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [ = ](int index) {drawBoard()->setDrawAttribution(EStreakBeginStyle, index);});
    connect(m_comboxend, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [ = ](int index) {drawBoard()->setDrawAttribution(EStreakEndStyle, index);});
}

void CommonAttributionRegister::registePenAttri()
{
    m_penStyle = new ComboBoxSettingWgt(tr("Pen"));
    m_penStyle->setAttribution(EPenStyle);
    QComboBox *m_pPenStyleComboBox = new QComboBox;

    m_pPenStyleComboBox->view()->setAlternatingRowColors(false);
    drawApp->setWidgetAccesibleName(m_pPenStyleComboBox, "Pen style combobox");

    m_pPenStyleComboBox->setIconSize(QSize(90, 30));
    m_pPenStyleComboBox->setFocusPolicy(Qt::NoFocus);

    m_pPenStyleComboBox->addItem(QIcon::fromTheme("water_color_pen"), tr("Watercolor"));
    m_pPenStyleComboBox->addItem(QIcon::fromTheme("calligraphy"), tr("Calligraphy"));
    m_pPenStyleComboBox->addItem(QIcon::fromTheme("crayon"), tr("Crayon"));

    m_penStyle->setComboBox(m_pPenStyleComboBox);

    connect(m_pPenStyleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), m_penStyle, [ = ](int index) {
        emit drawBoard()->attributionManager()->helper()->attributionChanged(EPenStyle, index + 1);
    });

    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, m_penStyle,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_penStyle) {
            QSignalBlocker bloker(m_pPenStyleComboBox);
            m_pPenStyleComboBox->setCurrentIndex(var.toInt() - 1);
        }
    });

    drawBoard()->attributionManager()->installComAttributeWgt(m_penStyle->attribution(), m_penStyle, 1);
    m_penStyle->installEventFilter(this);

    m_sliderPenWidth = new SliderSpinBoxWidget(EPenWidthProperty);
    m_sliderPenWidth->setRange(1, 10);
    m_sliderPenWidth->setTitle(tr("Pen Width"));

    setWgtAccesibleName(m_sliderPenWidth, "penWidth");
    drawBoard()->attributionManager()->installComAttributeWgt(EPenWidthProperty, m_sliderPenWidth, 2);

    connect(m_sliderPenWidth, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value/*, int phase*/) {
        drawBoard()->setDrawAttribution(EPenWidthProperty, value/*, phase*/);
    });
}

QList<QWidget *> CommonAttributionRegister::getStyleAttriWidgets()
{
    QList<QWidget *> l;
    l.append(m_fillBrushStyle);
    l.append(m_borderPenStyle);
    l.append(m_rectRadius);
    l.append(m_starAnchorAttri);
    l.append(m_starRadioAttri);
    l.append(m_polygonSidesAttri);
    l.append(m_penStyle);
    l.append(m_streakStyle);

    return l;
}
