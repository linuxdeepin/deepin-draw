/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
    cut             //切图
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
    BlurType            //模糊
};

enum ECutAttributeType {
    LineEditeAttribute,
    ButtonClickAttribute,
    NoneAttribute
};

enum ECutType {
    cut_1_1,
    cut_2_3,
    cut_8_5,
    cut_16_9,
    cut_free,
    cut_original
};

enum EBlurEffect {
    BlurEffect = 0,
    MasicoEffect
};

#endif // GLODEFINE_H
