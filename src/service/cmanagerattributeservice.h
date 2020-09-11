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
#ifndef CMANAGERATTRIBUTESERVICE_H
#define CMANAGERATTRIBUTESERVICE_H

#include <QObject>

#include "bzItems/cgraphicsitem.h"
#include "drawshape/cdrawscene.h"

class QGraphicsScene;
class QGraphicsItem;
class CGraphicsItem;
class CGraphicsRectItem;
class CGraphicsEllipseItem;
class CGraphicsPolygonItem;
class CGraphicsTriangleItem;
class CGraphicsPolygonalStarItem;
class CGraphicsPenItem;
class CGraphicsLineItem;
class CGraphicsMasicoItem;
class CDrawScene;
class QUndoCommand;
class CSetItemsCommonPropertyValueCommand;
class CGraphicsItemSelectedMgr;
class CManageViewSigleton;
class CGraphicsView;
class CGraphicsTextItem;

/*
* @bref: CManagerAttributeService 用于中转单选或者多选属性展示与设置
*/
class CManagerAttributeService : public QObject
{
    Q_OBJECT
public:
    static CManagerAttributeService *getInstance();

    /*
     * @bref: showSelectedCommonProperty drawscence 中多选或者单选图元公共属性展示解析函数
     * @param: scence 当前的场景
     * @param: items 被选中的图元
     * @param: write2Cache 是否将属性写入缓存
    */
    void showSelectedCommonProperty(CDrawScene *scence, QList<CGraphicsItem *> items, bool write2Cache = true);
    /*
     * @bref: refreshSelectedCommonProperty 刷新公共属性
    */
    void refreshSelectedCommonProperty(bool write2Cache = true);

//    /*
//     * @bref: setItemsCommonPropertyValue 设置被选中的图元的公共属性
//     * @param: property 属性类型
//     * @param: value 属性值
//    */
//    void setItemsCommonPropertyValue(EDrawProperty property, QVariant value,
//                                     bool pushTostack = true,
//                                     QMap<CGraphicsItem *, QVariant> *outOldValues = nullptr,
//                                     QMap<CGraphicsItem *, QVariant> *inUndoValues = nullptr,
//                                     bool write2Cache = true);

//    /*
//     * @bref: doSceneAdjustment 画布根据图片自适应
//    */
//    void doSceneAdjustment();
    /*
     * @bref: doCut 裁剪
    */
    void doCut();
    /*
     * @bref: getSelectedColorAlpha 获取选中图元的透明度
     * @param: DrawStatus 想要获取的是填充还是描边的透明度
    */
    //int getSelectedColorAlpha(DrawStatus drawstatus);

//    /*
//    * @bref: setPictureRotateOrFlip 设置图片图元翻转
//    * @param: type 翻转类型
//    */
//    void setPictureRotateOrFlip(ERotationType type);

signals:
    /*
     * @bref: showWidgetCommonProperty 发送多个图元被选中后需要显示的公共属性信号
     * @param: mode 当前需要显示的属性类型
     * @param: propertys 属性类型值
    */
    void signalShowWidgetCommonProperty(EGraphicUserType mode, QMap<EDrawProperty, QVariant> propertys, bool write2Cache = true);

    /*
     * @bref: signalIsAllPictureItem 发送画笔图元被选中后需要显示的公共属性信号
     * @param: isEnable 按钮是否可用，即画布与选中图片大小是否相同
     * @param: single 是否是单个图片
    */
    void signalIsAllPictureItem(bool isEnable, bool single);
private:
//    /*
//     * @bref: allPictureItem 判断是否全部是图片图元
//     * @param: scence
//     * @param: items 选中图元
//    */
//    bool allPictureItem(CDrawScene *scence, QList<CGraphicsItem *> items);

private:
    CManagerAttributeService();
    static CManagerAttributeService *instance;

    CDrawScene *m_currentScence;

    void updateCurrentScence();
};

#endif // CMANAGERATTRIBUTESERVICE_H
