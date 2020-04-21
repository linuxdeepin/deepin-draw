/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#ifndef GLODEFINE_H
#define GLODEFINE_H
#include <QGraphicsItem>
enum EDrawToolMode {
    selection,      //选择
    importPicture,  //导入图片
    rectangle,      //矩形
    ellipse,        //圆形
    triangle,       //三角形
    polygonalStar,  //多角星
    polygon,        //多边形
    line,           //线
    pen,            //画笔
    text,           //文本
    blur,           //模糊
    cut            //裁剪画板
};

enum ESelectMode {
    none,
    netSelect,
    move,
    size,
    rotate
};

enum DrawStatus {
    Fill,
    Stroke,
    TextFill
};

enum EPenType {
    straight,   // 直线
    arrow       // 箭头
};

enum ELineType {
    straightType,   // 直线
    arrowType      // 箭头
};

enum EGraphicUserType {
    RectType = QGraphicsItem::UserType + 1,      //矩形
    EllipseType,        //圆形
    TriangleType,       //三角形
    PolygonalStarType,  //多角星
    PolygonType,        //多边形
    LineType,           //线
    PenType,            //画笔
    TextType,           //文本
    PictureType,        //图片
    CutType,            //裁剪
    BlurType,           //模糊
    MgrType =  QGraphicsItem::UserType + 100    //多选管理图元
};

enum ECutAttributeType {
    LineEditeAttribute,
    ButtonClickAttribute,
    NoneAttribute
};

enum ECutType {
    cut_1_1,        //1:1裁剪
    cut_2_3,        //2:3裁剪
    cut_8_5,        //8:5裁剪
    cut_16_9,       //16:9裁剪
    cut_free,        //自由裁剪
    cut_original     //裁剪框恢复画板初始大小
};

enum EBlurEffect {
    BlurEffect = 0,
    MasicoEffect
};

enum ESaveDDFTriggerAction {
    QuitApp,             //保存完后退出画板
    LoadDDF,             //保存完后导入ＤＤＦ文件
    StartByDDF,          //通过DDF文件打开后弹出保存
    NewDrawingBoard,     //保存完后新建画板
    ImportPictrue,       //保存完后打开图片文件
    SaveAction           //正常保存,保存完后不做任何事
};
#endif // GLODEFINE_H
