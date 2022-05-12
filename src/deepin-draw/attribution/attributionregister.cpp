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

#include "attributionregister.h"
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

#include <QAbstractItemView>
#include <QPoint>
#include <QObject>

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

AttributionRegister::AttributionRegister(DrawBoard *d): m_drawBoard(d)
{
}

/**
 * @brief 注册各项属性工具栏控件，注册仅调用一次
 */
void AttributionRegister::registe()
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

    // 等待其它控件注册完成后调用
    registeStyleAttri();

    connect(m_drawBoard, qOverload<Page *>(&DrawBoard::currentPageChanged), this, [ = ](Page * page) {
        if (page->scene()->selectedItemCount()) {
            m_rotateAttri->setEnabled(false);
            m_groupWidget->setEnabled(false);
            m_orderAttri->setEnabled(false);
        }
    });
}

/**
 * @brief 注册属性工具栏群组工具
 */
void AttributionRegister::registeGroupAttri()
{
    if (nullptr != m_groupWidget) {
        return;
    }

    m_groupWidget = new GroupButtonWidget;
    setWgtAccesibleName(m_groupWidget, "groupButtonWidget");
    m_drawBoard->attributionManager()->installComAttributeWgt(EGroupWgt, m_groupWidget);

    connect(m_groupWidget, &GroupButtonWidget::buttonClicked, this, [ = ](bool doGroup, bool doUngroup) {
        auto currentPage = m_drawBoard->currentPage();
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
        auto pView = m_drawBoard->currentPage()->view();
        pView->setFocus();
    });

    connect(m_drawBoard->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, this,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_groupWidget) {
            bool canGroup = false;
            bool canUnGroup = false;
            QList<QVariant> bools = var.toList();
            if (bools.count() == 2) {
                canGroup   = bools[0].toBool();
                canUnGroup = bools[1].toBool();
            }

            m_groupWidget->setGroupFlag(canGroup, canUnGroup);
        }
    });
}

/**
 * @brief 注册属性工具栏旋转工具
 */
void AttributionRegister::resgisteRotateAttri()
{
    if (nullptr != m_rotateAttri) {
        return;
    }

    m_rotateAttri = new RotateAttriWidget(m_drawBoard);
    setWgtAccesibleName(m_rotateAttri, "rotateAttriWidget");
    m_drawBoard->attributionManager()->installComAttributeWgt(ERotProperty, m_rotateAttri, 0);
}

/**
 * @brief 注册属性工具栏层级控制工具
 */
void AttributionRegister::registeOrderAttri()
{
    if (nullptr != m_orderAttri) {
        return;
    }

    m_orderAttri = new OrderWidget(m_drawBoard);
    setWgtAccesibleName(m_orderAttri, "orderWidget");
    m_drawBoard->attributionManager()->installComAttributeWgt(EOrderProperty, m_orderAttri, 0);
}

/**
 * @brief 注册属性工具栏图片自适应工具
 */
void AttributionRegister::registeAdjustImageAttri()
{
    AdjustmentAtrriWidget *widget = new AdjustmentAtrriWidget;
    connect(widget->button(), &QPushButton::clicked, widget, [ = ]() {
        auto page = m_drawBoard->currentPage();
        if (page == nullptr)
            return;

        auto curScene = page->scene();
        if (curScene != nullptr) {
            auto rect = curScene->selectedPageItems().first()->sceneBoundingRect();
            if (rect != curScene->sceneRect()) {
                UndoRecorder block(page->scene()->currentTopLayer(), LayerUndoCommand::RectChanged);
                page->scene()->currentTopLayer()->setRect(rect);
                page->view()->setSceneRect(rect);
                widget->button()->setFlat(true);
            }
        }
    });

    connect(m_drawBoard->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, this,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == widget) {
            widget->setEnabled(var.toBool());
        }
    });

    m_drawBoard->attributionManager()->installComAttributeWgt(EImageAdaptScene, widget, false);
}

/**
 * @brief 注册属性工具栏样式工具组，样式工具包含子属性，
 *      部分属性工具将显示到此工具组下
 */
void AttributionRegister::registeStyleAttri()
{
    StyleAttriWidget *styleAtti = new StyleAttriWidget(EStyleProper, m_drawBoard);
    styleAtti->setProperty(ParentAttriWidget, true);
    styleAtti->addChildAtrri(m_fillStyle);
    styleAtti->addChildAtrri(m_borderStyle);
    styleAtti->addChildAtrri(m_rectRadius);
    styleAtti->addChildAtrri(m_starAnchorAttri);
    styleAtti->addChildAtrri(m_starRadioAttri);
    styleAtti->addChildAtrri(m_polygonSidesAttri);
    styleAtti->addChildAtrri(m_penStyle);

    m_penWidth->show();
    m_fillStyle->setProperty(ChildAttriWidget, true);
    m_borderStyle->setProperty(ChildAttriWidget, true);
    m_penWidth->setProperty(ChildAttriWidget, true);
    m_rectRadius->setProperty(ChildAttriWidget, true);
    m_penStyle->setProperty(ChildAttriWidget, true);
    // 设置星形和多边形图元的属性为子属性，用于显示在样式控件之下
    m_starAnchorAttri->setProperty(ChildAttriWidget, true);
    m_starRadioAttri->setProperty(ChildAttriWidget, true);
    m_polygonSidesAttri->setProperty(ChildAttriWidget, true);

    m_drawBoard->attributionManager()->installComAttributeWgt(styleAtti->attribution(), styleAtti, QVariant());
    m_rectRadius->setProperty(ChildAttriWidget, true);

}

/**
 * @brief 注册属性工具栏基础样式工具，包括颜色，画笔宽度等
 */
void AttributionRegister::registeBaseStyleAttrri()
{
    if (nullptr != m_fillStyle
            || nullptr != m_borderStyle
            || nullptr != m_penWidth) {
        return;
    }

    m_fillStyle = new ColorStyleWidget(m_drawBoard);
    m_fillStyle->setColorFill(0);
    m_borderStyle = new ColorStyleWidget(m_drawBoard);
    m_borderStyle->setTitleText(tr("border"));
    m_borderStyle->setColorFill(1);
    m_borderStyle->setColorTextVisible(false);
    m_drawBoard->attributionManager()->installComAttributeWgt(EBrushColor, m_fillStyle, QColor(0, 0, 0));
    m_drawBoard->attributionManager()->installComAttributeWgt(EPenColor, m_borderStyle, QColor(0, 0, 0));

    connect(m_fillStyle, &ColorStyleWidget::colorChanged, this, [ = ](const QColor & color, int phase) {
        if (m_fillStyle->isEnableAttriVisible()) {
            QVariantList l;
            l << m_fillStyle->isAttriApply() << color;
            m_drawBoard->setDrawAttribution(EBrushColor, l, phase);
        } else {
            m_drawBoard->setDrawAttribution(EBrushColor, color, phase);
        }

    });

    connect(m_borderStyle, &ColorStyleWidget::colorChanged, this, [ = ](const QColor & color, int phase) {
        if (m_fillStyle->isEnableAttriVisible()) {
            QVariantList l;
            l << m_fillStyle->isAttriApply() << color;
            m_drawBoard->setDrawAttribution(EPenColor, l, phase);
        } else {
            m_drawBoard->setDrawAttribution(EPenColor, color, phase);
        }
    });

    m_penWidth = new CSpinBox(m_drawBoard);
    m_penWidth->setEnabledEmbedStyle(true);
    m_penWidth->setSpinRange(0, 10);
    m_penWidth->setMinimumWidth(90);
    m_penWidth->setProperty(ChildAttriWidget, true);


    QObject::connect(m_penWidth, &CSpinBox::valueChanged, m_penWidth, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(phase)
        m_drawBoard->setDrawAttribution(EPenWidth, value);
    });

    connect(m_drawBoard->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, m_penWidth,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_penWidth) {
            QSignalBlocker bloker(m_penWidth);
            if (!var.isValid()) {
                m_penWidth->setSpecialText();
            } else {
                m_penWidth->setValue(var.toInt());
            }
        }
    });
    m_drawBoard->attributionManager()->installComAttributeWgt(EPenWidth, m_penWidth, 2);

    //在border中显示
    m_borderStyle->addWidget(m_penWidth);

    m_rectRadius = new RectRadiusStyleWidget(m_drawBoard);
    m_drawBoard->attributionManager()->installComAttributeWgt(m_rectRadius->attribution(), m_rectRadius, QVariant());
    QObject::connect(m_rectRadius, &RectRadiusStyleWidget::valueChanged, m_penWidth, [ = ](QVariant value, EChangedPhase phase) {
        m_drawBoard->setDrawAttribution(m_rectRadius->attribution(), value, phase);
    });
}

/**
 * @brief 注册用于设置/更新显示星形图元锚点数量的属性工具栏
 */
void AttributionRegister::registeStarAnchorAttri()
{
    if (nullptr != m_starAnchorAttri) {
        return;
    }

    m_starAnchorAttri = new SliderSpinBoxWidget(EStarAnchor);
    m_starAnchorAttri->setRange(EStarAnchorMin, EStarAnchorMax);
    m_starAnchorAttri->setVar(EStarAnchorDefault);
    m_starAnchorAttri->setTitle(tr("Vertices"));

    setWgtAccesibleName(m_starAnchorAttri, "starAnchorAttri");
    m_drawBoard->attributionManager()->installComAttributeWgt(EStarAnchor, m_starAnchorAttri, 5);

    connect(m_starAnchorAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value) {
        m_drawBoard->setDrawAttribution(EStarAnchor, value);
    });
}

/**
 * @brief 注册用于设置/更新显示星形图元半径的属性工具栏
 */
void AttributionRegister::registeStarInnerOuterRadioAttri()
{
    if (nullptr != m_starRadioAttri) {
        return;
    }

    m_starRadioAttri = new SliderSpinBoxWidget(EStarInnerOuterRadio, SliderSpinBoxWidget::EPercentStyle);
    m_starRadioAttri->setRange(EStarRadioMin, EStarRadioMax);
    m_starRadioAttri->setVar(EStarRadioDefault);
    m_starRadioAttri->setTitle(tr("Radius"));

    setWgtAccesibleName(m_starRadioAttri, "starInnerOuterRadio");
    m_drawBoard->attributionManager()->installComAttributeWgt(EStarInnerOuterRadio, m_starRadioAttri, 5);

    connect(m_starRadioAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value) {
        m_drawBoard->setDrawAttribution(EStarInnerOuterRadio, value);
    });
}

/**
 * @brief 注册用于设置/更新显示多边形图元侧边数量的属性工具栏
 */
void AttributionRegister::registePolygonSidesAttri()
{
    if (nullptr != m_polygonSidesAttri) {
        return;
    }

    m_polygonSidesAttri = new SliderSpinBoxWidget(EPolygonSides);
    m_polygonSidesAttri->setRange(EPolygonSidesMin, EPolygonSidesMax);
    m_polygonSidesAttri->setVar(EPolygonSidesDefault);
    m_polygonSidesAttri->setTitle(tr("Sides"));

    setWgtAccesibleName(m_polygonSidesAttri, "polygonSidesAttri");
    m_drawBoard->attributionManager()->installComAttributeWgt(EPolygonSides, m_polygonSidesAttri, 5);

    connect(m_polygonSidesAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value) {
        m_drawBoard->setDrawAttribution(EPolygonSides, value);
    });
}

void AttributionRegister::registePenAttri()
{
    m_penStyle = new ComboBoxSettingWgt;
    m_penStyle->setAttribution(EPenStyle);
    QComboBox *m_pPenStyleComboBox = new QComboBox;
    drawApp->setWidgetAccesibleName(m_pPenStyleComboBox, "Pen style combobox");

    m_pPenStyleComboBox->view()->installEventFilter(this);

    m_pPenStyleComboBox->setFixedSize(QSize(182, 36));
    m_pPenStyleComboBox->setIconSize(QSize(24, 20));
    m_pPenStyleComboBox->setFocusPolicy(Qt::NoFocus);

    m_pPenStyleComboBox->addItem(QIcon::fromTheme("icon_marker"), tr("Watercolor"));
    m_pPenStyleComboBox->addItem(QIcon::fromTheme("icon_calligraphy"), tr("Calligraphy pen"));
    m_pPenStyleComboBox->addItem(QIcon::fromTheme("icon_crayon"), tr("Crayon"));

    m_penStyle->setComboBox(m_pPenStyleComboBox);

    connect(m_pPenStyleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), m_penStyle, [ = ](int index) {
        emit m_drawBoard->attributionManager()->helper()->attributionChanged(EPenStyle, index + 1);
    });

    connect(m_drawBoard->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, m_penStyle,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_penStyle) {
            QSignalBlocker bloker(m_pPenStyleComboBox);
            m_pPenStyleComboBox->setCurrentIndex(var.toInt() - 1);
        }
    });

    m_drawBoard->attributionManager()->installComAttributeWgt(m_penStyle->attribution(), m_penStyle, 1);
    m_penStyle->installEventFilter(this);
}
