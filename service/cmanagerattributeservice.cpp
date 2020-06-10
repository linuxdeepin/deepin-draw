/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "cmanagerattributeservice.h"
#include "drawshape/cdrawtoolmanagersigleton.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicsrectitem.h"
#include "drawshape/cgraphicsellipseitem.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicstriangleitem.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include "drawshape/cgraphicsitemselectedmgr.h"
#include "drawshape/cgraphicspenitem.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicsmasicoitem.h"
#include "drawshape/ccuttool.h"

#include "frame/cundocommands.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "widgets/ctextedit.h"

#include <QGraphicsItem>
#include <QDebug>

//升序排列用
static bool zValueSortASC(CGraphicsItem *info1, CGraphicsItem *info2)
{
    return info1->zValue() <= info2->zValue();
}

CManagerAttributeService *CManagerAttributeService::instance = nullptr;
CManagerAttributeService *CManagerAttributeService::getInstance()
{
    if (nullptr == instance) {
        instance = new CManagerAttributeService();
    }
    return instance;
}

void CManagerAttributeService::showSelectedCommonProperty(CDrawScene *scence, QList<CGraphicsItem *> items, bool write2Cache)
{
    Q_UNUSED(scence)
    updateCurrentScence();

    //图片图元不需要展示属性，如果都是图片图元，直接发送信号展示属性栏
    if (allPictureItem(scence, items)) {
        return;
    }

    qSort(items.begin(), items.end(), zValueSortASC);
    EGraphicUserType mode = EGraphicUserType::NoType;
    QMap<EDrawProperty, QVariant> propertys;//临时存放
    propertys.clear();
    if (items.size() <= 0) {
        mode = EGraphicUserType::NoType;
    } else {
        mode = static_cast<EGraphicUserType>(items.at(0)->type());
        switch (mode) {
        case RectType://矩形
            propertys[FillColor] = static_cast<CGraphicsRectItem *>(items.at(0))->brush();
            propertys[LineWidth] = static_cast<CGraphicsRectItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsRectItem *>(items.at(0))->pen().color();
            propertys[RectRadius] = static_cast<CGraphicsRectItem *>(items.at(0))->getXRedius();
            break;
        case EllipseType://圆形
            propertys[FillColor] = static_cast<CGraphicsEllipseItem *>(items.at(0))->brush();
            propertys[LineWidth] = static_cast<CGraphicsEllipseItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsEllipseItem *>(items.at(0))->pen().color();
            break;
        case TriangleType://三角形
            propertys[FillColor] = static_cast<CGraphicsTriangleItem *>(items.at(0))->brush();
            propertys[LineWidth] = static_cast<CGraphicsTriangleItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsTriangleItem *>(items.at(0))->pen().color();
            break;
        case PolygonalStarType://多角星
            propertys[FillColor] = static_cast<CGraphicsPolygonalStarItem *>(items.at(0))->brush();
            propertys[LineWidth] = static_cast<CGraphicsPolygonalStarItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsPolygonalStarItem *>(items.at(0))->pen().color();
            propertys[Anchors] = static_cast<CGraphicsPolygonalStarItem *>(items.at(0))->anchorNum();
            propertys[StarRadius] = static_cast<CGraphicsPolygonalStarItem *>(items.at(0))->innerRadius();
            break;
        case PolygonType://多边形
            propertys[FillColor] = static_cast<CGraphicsPolygonItem *>(items.at(0))->brush();
            propertys[LineWidth] = static_cast<CGraphicsPolygonItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsPolygonItem *>(items.at(0))->pen().color();
            propertys[SideNumber] = static_cast<CGraphicsPolygonItem *>(items.at(0))->nPointsCount();
            break;
        case LineType://线
            propertys[LineWidth] = static_cast<CGraphicsLineItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsLineItem *>(items.at(0))->pen().color();
            propertys[LineAndPenStartType] = static_cast<CGraphicsLineItem *>(items.at(0))->getLineStartType();
            propertys[LineAndPenEndType] = static_cast<CGraphicsLineItem *>(items.at(0))->getLineEndType();
            break;
        case PenType://画笔
            propertys[LineWidth] = static_cast<CGraphicsPenItem *>(items.at(0))->pen().width();
            propertys[LineColor] = static_cast<CGraphicsPenItem *>(items.at(0))->pen().color();
            propertys[LineAndPenStartType] = static_cast<CGraphicsPenItem *>(items.at(0))->getPenStartType();
            propertys[LineAndPenEndType] = static_cast<CGraphicsPenItem *>(items.at(0))->getPenEndType();
            break;
        case TextType://文本
            // 刷新文本属性前先调用一次获取属性的函数，有效避免没有选中文字的时候属性不更新等问题
            static_cast<CGraphicsTextItem *>(items.at(0))->getTextEdit()->checkTextProperty();
            propertys[TextColor] = static_cast<CGraphicsTextItem *>(items.at(0))->getSelectedTextColor();
            propertys[TextFont] = static_cast<CGraphicsTextItem *>(items.at(0))->getSelectedFontFamily();
            propertys[TextSize] = static_cast<CGraphicsTextItem *>(items.at(0))->getSelectedFontSize();
            propertys[TextHeavy] = static_cast<CGraphicsTextItem *>(items.at(0))->getSelectedFontStyle();
            propertys[TextColorAlpha] = static_cast<CGraphicsTextItem *>(items.at(0))->getSelectedTextColorAlpha();
            qDebug() << "Text Item = " << propertys;
            break;
        case BlurType://模糊
            propertys[Blurtype] = static_cast<int>(static_cast<CGraphicsMasicoItem *>(items.at(0))->getBlurEffect());
            propertys[BlurWidth] = static_cast<CGraphicsMasicoItem *>(items.at(0))->getBlurWidth();
            break;
        default:
            break;
        }
    }
    QMap<EDrawProperty, QVariant> allPropertys;//信号中传递
    QVariant tmpVariant;
    tmpVariant.clear();
    for (int i = 1; i < items.size(); i++) {
        CGraphicsItem *item = items.at(i);
        item->setData(1, 1);
        allPropertys.clear();
        switch (static_cast<EGraphicUserType>(item->type())) {
        case RectType://矩形
            if (propertys.contains(FillColor)) {
                if (propertys[FillColor] == static_cast<CGraphicsRectItem *>(item)->brush()) {
                    allPropertys[FillColor] = propertys[FillColor];
                } else {
                    allPropertys[FillColor] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsRectItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsRectItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            if (propertys.contains(RectRadius)) {
                if (propertys[RectRadius] == static_cast<CGraphicsRectItem *>(item)->getXRedius()) {
                    allPropertys[RectRadius] = propertys[RectRadius];
                } else {
                    allPropertys[RectRadius] = tmpVariant;
                }
            }
            break;
        case EllipseType://圆形
            if (propertys.contains(FillColor)) {
                if (propertys[FillColor] == static_cast<CGraphicsEllipseItem *>(item)->brush()) {
                    allPropertys[FillColor] = propertys[FillColor];
                } else {
                    allPropertys[FillColor] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsEllipseItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsEllipseItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            break;
        case TriangleType://三角形
            if (propertys.contains(FillColor)) {
                if (propertys[FillColor] == static_cast<CGraphicsTriangleItem *>(item)->brush()) {
                    allPropertys[FillColor] = propertys[FillColor];
                } else {
                    allPropertys[FillColor] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsTriangleItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsTriangleItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            break;
        case PolygonalStarType://多角星
            if (propertys.contains(FillColor)) {
                if (propertys[FillColor] == static_cast<CGraphicsPolygonalStarItem *>(item)->brush()) {
                    allPropertys[FillColor] = propertys[FillColor];
                } else {
                    allPropertys[FillColor] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsPolygonalStarItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsPolygonalStarItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(Anchors)) {
                if (propertys[Anchors] == static_cast<CGraphicsPolygonalStarItem *>(item)->anchorNum()) {
                    allPropertys[Anchors] = propertys[Anchors];
                } else {
                    allPropertys[Anchors] = tmpVariant;
                }
            }
            if (propertys.contains(StarRadius)) {
                if (propertys[StarRadius] == static_cast<CGraphicsPolygonalStarItem *>(item)->innerRadius()) {
                    allPropertys[StarRadius] = propertys[StarRadius];
                } else {
                    allPropertys[StarRadius] = tmpVariant;
                }
            }
            break;
        case PolygonType://多边形
            if (propertys.contains(FillColor)) {
                if (propertys[FillColor] == static_cast<CGraphicsPolygonItem *>(item)->brush()) {
                    allPropertys[FillColor] = propertys[FillColor];
                } else {
                    allPropertys[FillColor] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsPolygonItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsPolygonItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            if (propertys.contains(SideNumber)) {
                if (propertys[SideNumber] == static_cast<CGraphicsPolygonItem *>(item)->nPointsCount()) {
                    allPropertys[SideNumber] = propertys[SideNumber];
                } else {
                    allPropertys[SideNumber] = tmpVariant;
                }
            }
            break;
        case LineType://线
            if (propertys.contains(LineAndPenStartType)) {
                if (propertys[LineAndPenStartType] == static_cast<CGraphicsLineItem *>(item)->getLineStartType()) {
                    allPropertys[LineAndPenStartType] = propertys[LineAndPenStartType];
                } else {
                    allPropertys[LineAndPenStartType] = tmpVariant;
                }
            }
            if (propertys.contains(LineAndPenEndType)) {
                if (propertys[LineAndPenEndType] == static_cast<CGraphicsLineItem *>(item)->getLineEndType()) {
                    allPropertys[LineAndPenEndType] = propertys[LineAndPenEndType];
                } else {
                    allPropertys[LineAndPenEndType] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsLineItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsLineItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            break;
        case PenType://画笔
            if (propertys.contains(LineAndPenStartType)) {
                if (propertys[LineAndPenStartType] == static_cast<CGraphicsPenItem *>(item)->getPenStartType()) {
                    allPropertys[LineAndPenStartType] = propertys[LineAndPenStartType];
                } else {
                    allPropertys[LineAndPenStartType] = tmpVariant;
                }
            }
            if (propertys.contains(LineAndPenEndType)) {
                if (propertys[LineAndPenEndType] == static_cast<CGraphicsPenItem *>(item)->getPenEndType()) {
                    allPropertys[LineAndPenEndType] = propertys[LineAndPenEndType];
                } else {
                    allPropertys[LineAndPenEndType] = tmpVariant;
                }
            }
            if (propertys.contains(LineWidth)) {
                if (propertys[LineWidth] == static_cast<CGraphicsPenItem *>(item)->pen().width()) {
                    allPropertys[LineWidth] = propertys[LineWidth];
                } else {
                    allPropertys[LineWidth] = tmpVariant;
                }
            }
            if (propertys.contains(LineColor)) {
                if (propertys[LineColor] == static_cast<CGraphicsPenItem *>(item)->pen().color()) {
                    allPropertys[LineColor] = propertys[LineColor];
                } else {
                    allPropertys[LineColor] = tmpVariant;
                }
            }
            break;
        case TextType: { //文本
            CGraphicsTextItem *textItem = dynamic_cast<CGraphicsTextItem *>(item);
            if (nullptr == textItem) {
                qDebug() << "static_cast<CGraphicsTextItem *> nullptr";
                continue;
            }
            // [0]  手动调用刷新，否则会出现属性未及时更新导致属性显示异常
            textItem->getTextEdit()->checkTextProperty();

            if (propertys.contains(TextColor)) {
                mode = EGraphicUserType::TextType;
                if (propertys[TextColor] == textItem->getSelectedTextColor()) {
                    allPropertys[TextColor] = propertys[TextColor];
                } else {
                    allPropertys[TextColor] = tmpVariant;
                }
            }
            if (propertys.contains(TextFont)) {
                QFont font = textItem->getFont();
                if (propertys[TextFont].value<QFont>().family() == textItem->getSelectedFontFamily()) {
                    allPropertys[TextFont] = propertys[TextFont];
                } else {
                    allPropertys[TextFont] = tmpVariant;
                }
            }
            if (propertys.contains(TextHeavy)) {
                QFont font = textItem->getFont();
                if (propertys[TextHeavy] == textItem->getSelectedFontStyle()) {
                    allPropertys[TextHeavy] = propertys[TextHeavy];
                } else {
                    allPropertys[TextHeavy] = tmpVariant;
                }
            }
            if (propertys.contains(TextSize)) {
                if (propertys[TextSize] == textItem->getSelectedFontSize()) {
                    allPropertys[TextSize] = propertys[TextSize];
                } else {
                    allPropertys[TextSize] = tmpVariant;
                }
            }
            if (propertys.contains(TextColorAlpha)) {
                if (propertys[TextColorAlpha] == textItem->getSelectedTextColorAlpha()) {
                    allPropertys[TextColorAlpha] = propertys[TextColorAlpha];
                } else {
                    allPropertys[TextColorAlpha] = tmpVariant;
                }
            }
            break;
        }
        case BlurType://模糊
            if (propertys.contains(Blurtype)) {
                if (propertys[Blurtype] == static_cast<int>(static_cast<CGraphicsMasicoItem *>(item)->getBlurEffect())) {
                    allPropertys[Blurtype] = propertys[Blurtype];
                } else {
                    allPropertys[Blurtype] = tmpVariant;
                }
            }
            if (propertys.contains(BlurWidth)) {
                if (propertys[BlurWidth] == static_cast<CGraphicsMasicoItem *>(item)->getBlurWidth()) {
                    allPropertys[BlurWidth] = propertys[BlurWidth];
                } else {
                    allPropertys[BlurWidth] = tmpVariant;
                }
            }
            break;
        default:
            break;
        }
        propertys = allPropertys;
    }
    if (items.size() == 1) {
        allPropertys = propertys;
    }
    if (allPropertys.size() == 0) {
        mode = EGraphicUserType::NoType;
    }
    emit signalShowWidgetCommonProperty(mode, allPropertys, write2Cache);
}

void CManagerAttributeService::refreshSelectedCommonProperty(bool write2Cache)
{
    updateCurrentScence();
    if (m_currentScence) {
        QList<CGraphicsItem *> allItems;
        if (m_currentScence->getItemsMgr()->getItems().size() > 1) {
            allItems = m_currentScence->getItemsMgr()->getItems();
        } else {
            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
                if (allSelectItems.at(i)->zValue() == 0.0) {
                    allSelectItems.removeAt(i);
                    continue;
                }
                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                    allSelectItems.removeAt(i);
                }
            }

            if (allSelectItems.size() >= 1) {
                CGraphicsItem *item = static_cast<CGraphicsItem *>(allSelectItems.at(0));
                if (item != nullptr) {
                    allItems.append(item);
                }
            }
        }
        this->showSelectedCommonProperty(m_currentScence, allItems, write2Cache);
    }
}

void CManagerAttributeService::setItemsCommonPropertyValue(EDrawProperty property, QVariant value,
                                                           bool pushTostack,
                                                           QMap<CGraphicsItem *, QVariant> *outOldValues,
                                                           QMap<CGraphicsItem *, QVariant> *inUndoValues, bool write2Cache)
{
    if (CManageViewSigleton::GetInstance()->getCurView() == nullptr)
        return;

    QList<CGraphicsItem *> allItems;

    updateCurrentScence();

    if (m_currentScence && m_currentScence->getItemsMgr()) {
        if (m_currentScence->getItemsMgr()->getItems().size() > 1) {
            allItems = m_currentScence->getItemsMgr()->getItems();
        } else {
            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
                if (allSelectItems.at(i)->zValue() == 0.0) {
                    allSelectItems.removeAt(i);
                    continue;
                }
                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                    allSelectItems.removeAt(i);
                }
            }

            if (allSelectItems.size() >= 1) {
                CGraphicsItem *item = static_cast<CGraphicsItem *>(allSelectItems.at(0));
                if (item != nullptr) {
                    allItems.append(item);
                }
            }
        }
        if (allItems.size() <= 0) {
            return;
        }
        static int i = 0;
        qDebug() << "new CSetItemsCommonPropertyValueCommand i = "
                 << ++i << "value = " << value << "pushTostack = " << pushTostack
                 << "write2Cache:" << write2Cache;
        CSetItemsCommonPropertyValueCommand *addCommand = nullptr;
        if (inUndoValues == nullptr) {
            addCommand = new CSetItemsCommonPropertyValueCommand(m_currentScence, allItems, property, value, write2Cache);
        } else {
            addCommand = new CSetItemsCommonPropertyValueCommand(m_currentScence, *inUndoValues, property, value, write2Cache);
        }
        if (pushTostack) {
            CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(addCommand);
            if (outOldValues != nullptr) {
                *outOldValues = addCommand->undoInfoValues();
            }
        } else {
            addCommand->redo();

            if (outOldValues != nullptr) {
                *outOldValues = addCommand->undoInfoValues();
            }

            delete addCommand;
            addCommand = nullptr;
        }
    }
}

CManagerAttributeService::CManagerAttributeService()
{
    m_currentScence = nullptr;
}

void CManagerAttributeService::updateCurrentScence()
{
    m_currentScence = static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene());
}

void CManagerAttributeService::doSceneAdjustment()
{
    if (CManageViewSigleton::GetInstance()->getCurView() == nullptr)
        return;

    updateCurrentScence();

    if (m_currentScence && m_currentScence->getItemsMgr()) {
        QList<CGraphicsItem *> allItems;
        if (m_currentScence->getItemsMgr()->getItems().size() > 1) {
            m_currentScence->doAdjustmentScene(m_currentScence->getItemsMgr()->mapRectToScene(m_currentScence->getItemsMgr()->boundingRect()), nullptr);
        } else {
            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
                if (allSelectItems.at(i)->zValue() == 0.0) {
                    allSelectItems.removeAt(i);
                    continue;
                }
                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                    allSelectItems.removeAt(i);
                }
            }

            if (allSelectItems.size() >= 1) {
                CGraphicsItem *item = static_cast<CGraphicsItem *>(allSelectItems.at(0));
                if (item != nullptr) {
                    m_currentScence->doAdjustmentScene(item->mapRectToScene(item->boundingRect()), item);
                }
            }
        }
    }
}

void CManagerAttributeService::doCut()
{
    updateCurrentScence();
    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    ///区分裁剪
    if (cut == currentMode) {
        ECutAttributeType attributeType = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutAttributeType();
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(cut);
        if (attributeType == ECutAttributeType::ButtonClickAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutType(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutType(), m_currentScence);
            }
        } else if (attributeType == ECutAttributeType::LineEditeAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutSize(m_currentScence, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize());
            }
        }
    }
}

int CManagerAttributeService::getSelectedColorAlpha(DrawStatus drawstatus)
{
    int alpha = 255;
    if (CManageViewSigleton::GetInstance()->getCurView() == nullptr)
        return alpha;

    updateCurrentScence();

    if (m_currentScence && m_currentScence->getItemsMgr()) {
        QList<CGraphicsItem *> allItems;
        if (m_currentScence->getItemsMgr()->getItems().size() > 1) {
            allItems = m_currentScence->getItemsMgr()->getItems();
            int alphaTemp = 255;
            CGraphicsItem *itemFirst = dynamic_cast<CGraphicsItem *>(allItems.at(0));
            if (itemFirst != nullptr) {
                if (drawstatus == DrawStatus::Fill) {
                    alphaTemp = itemFirst->brush().color().alpha();
                } else if (drawstatus == DrawStatus::Stroke) {
                    alphaTemp = itemFirst->pen().color().alpha();
                } else if (drawstatus == DrawStatus::TextFill) {
                    if (itemFirst->type() == TextType) {
                        alphaTemp = static_cast<CGraphicsTextItem *>(itemFirst)->getTextColor().alpha();
                    }
                }
            }
            for (int i = allItems.size() - 1; i >= 0; i--) {
                CGraphicsItem *item = dynamic_cast<CGraphicsItem *>(allItems.at(i));
                if (item != nullptr) {
                    if (drawstatus == DrawStatus::Fill) {
                        alpha = item->brush().color().alpha();
                    } else if (drawstatus == DrawStatus::Stroke) {
                        alpha = item->pen().color().alpha();
                    } else if (drawstatus == DrawStatus::TextFill) {
                        if (item->type() == TextType) {
                            alpha = static_cast<CGraphicsTextItem *>(item)->getTextColor().alpha();
                        }
                    }
                    if (alpha != alphaTemp) {
                        alpha = 255;
                        break;
                    } else {
                        alphaTemp = alpha;
                    }
                }
            }
        } else {
            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
                if (allSelectItems.at(i)->zValue() == 0.0) {
                    allSelectItems.removeAt(i);
                    continue;
                }
                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                    allSelectItems.removeAt(i);
                }
            }

            if (allSelectItems.size() >= 1) {
                CGraphicsItem *item = dynamic_cast<CGraphicsItem *>(allSelectItems.at(0));
                if (item != nullptr) {
                    if (drawstatus == DrawStatus::Fill) {
                        alpha = item->brush().color().alpha();
                    } else if (drawstatus == DrawStatus::Stroke) {
                        alpha = item->pen().color().alpha();
                    } else if (drawstatus == DrawStatus::TextFill) {
                        if (item->type() == TextType) {
                            alpha = static_cast<CGraphicsTextItem *>(item)->getTextColor().alpha();
                        }
                    }
                }
            }
        }
    }
    return alpha;
}

void CManagerAttributeService::setPictureRotateOrFlip(ERotationType type)
{
    updateCurrentScence();

    QList<QGraphicsItem *> items = m_currentScence->selectedItems();
    if ( items.count() != 0 ) {
        CGraphicsItem *item = static_cast<CGraphicsItem *>(items.first());

        if (item != nullptr) {
            CItemRotationCommand *addCommand = nullptr;
            addCommand = new CItemRotationCommand(m_currentScence, item, type);
            CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(addCommand);
        }
    }
}

bool CManagerAttributeService::allPictureItem(CDrawScene *scence, QList<CGraphicsItem *> items)
{
    Q_UNUSED(scence)
    bool isAllPictureItem = true;
    if (items.size() >= 1) {
        for (int i = 0; i < items.size(); i++) {
            CGraphicsItem *item = items.at(i);
            EGraphicUserType mode = static_cast<EGraphicUserType>(item->type());
            if (mode != PictureType) {
                isAllPictureItem = false;
                break;
            }
        }
    } else {
        isAllPictureItem = false;
    }

    if (isAllPictureItem) {
        if (m_currentScence->getItemsMgr()->getItems().size() > 1) {
            emit signalIsAllPictureItem(!(m_currentScence->getItemsMgr()->mapRectToScene(m_currentScence->getItemsMgr()->boundingRect()) == m_currentScence->sceneRect())
                                        , items.size() > 1 ? true : false);
        } else {
            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
                if (allSelectItems.at(i)->zValue() == 0.0) {
                    allSelectItems.removeAt(i);
                    continue;
                }
                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                    allSelectItems.removeAt(i);
                }
            }

            if (allSelectItems.size() >= 1) {
                CGraphicsItem *item = static_cast<CGraphicsItem *>(allSelectItems.at(0));
                if (item != nullptr) {
                    emit signalIsAllPictureItem(!(item->mapRectToScene(item->boundingRect()) == m_currentScence->sceneRect())
                                                , items.size() > 1 ? true : false);
                }
            }
        }
    }
    return isAllPictureItem;
}
