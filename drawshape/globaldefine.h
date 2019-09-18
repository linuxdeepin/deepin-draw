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
    CutType
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

#endif // GLODEFINE_H
