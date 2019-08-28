#ifndef GLODEFINE_H
#define GLODEFINE_H

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
};

#endif // GLODEFINE_H
