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

#include "drawshape/cgraphicsitem.h"
#include "drawshape/cdrawscene.h"

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
    */
    void showSelectedCommonProperty(CDrawScene *scence,QList<CGraphicsItem*> items);

    /*
     * @bref: setItemsCommonPropertyValue 设置被选中的图元的公共属性
     * @param: property 属性类型
     * @param: value 属性值
    */
    void setItemsCommonPropertyValue(EDrawProperty property,QVariant value);

signals:
    /*
     * @bref: showWidgetCommonProperty 发送多个图元被选中后需要显示的公共属性信号
     * @param: mode 当前需要显示的属性类型
     * @param: propertys 属性类型值
    */
    void signalShowWidgetCommonProperty(EDrawToolMode mode,QMap<EDrawProperty,QVariant> propertys);

private:
    CManagerAttributeService();
    static CManagerAttributeService *instance;

    CDrawScene *m_currentScence;
};

#endif // CMANAGERATTRIBUTESERVICE_H
