#ifndef SITEMDATA_H
#define SITEMDATA_H
#include <QDataStream>
#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QTextDocument>

#pragma pack(push, 1)
//定义图元类型
struct SColor {
    int r;
    int g;
    int b;
    int a;
};

//画笔
struct SPen {
    qint32 width;
    SColor col;
};

//画刷
struct SBrush {
    SColor col;
};

//图元头部
struct SGraphicsUnitHead {
    qint8 headCheck[4]; //头部校验
    qint32 dataType; //图元类型
    qint64 dataLength; //数据长度
    SPen    pen;
    SBrush  brush;
    QPointF  pos;  //图元起始位置
    qreal rotate;   //旋转角度
    qreal zValue;  //Z值 用来保存图形层次
};

//图元尾部
struct SGraphicsUnitTail {
    qint8 tailCheck[4]; //尾部校验
};

//矩形
struct SGraphicsRectUnitData {
    QPointF point1;
    QPointF point2;
};

//圆
struct SGraphicsCircleUnitData {
    SGraphicsRectUnitData rect;
    //TODO 序列化和反序列化
};

//多边形
struct SGraphicsPolygonUnitData {
    SGraphicsRectUnitData rect; //外接矩形
    quint8 pointNum;
};

//多角星
struct SGraphicsPolygonStarUnitData {
    SGraphicsRectUnitData rect; //外接矩形
    qint32 pointNum;
    qint32 radius;      //内接半径
};

//线
struct SGraphicsLineUnitData {
    QPointF point1;
    QPointF point2;
};

//文本
struct SGraphicsTextUnitData {
    SGraphicsRectUnitData rect;
    char strText[1024 * 1024 + 1]; //文本信息以HTML存储富文本内容 最大支持1024×1024字节
};

//图片
struct SGraphicsPictureUnitData {
    qint32 length;   //图片长度
    const char *pic;  //图片
};

//画笔
struct SGraphicsPenUnitData {
    qint32 length;
    QPointF *point;
};

//数据封装
union CGraphicsItemData {
    SGraphicsRectUnitData *pRect;
    SGraphicsCircleUnitData *pCircle;
    SGraphicsPolygonUnitData *pPolygon;
    SGraphicsPolygonStarUnitData *pPolygonStar;
    SGraphicsLineUnitData *pLine;
    SGraphicsTextUnitData *pText;
    SGraphicsPictureUnitData *pPic;
};


//单个图数据
struct CGraphicsUnit {
    SGraphicsUnitHead head;
    CGraphicsItemData data;
    SGraphicsUnitTail tail;
};

//整个图元数据
struct CGraphics {
    qint64 unitCount;   //图元数量
    qreal width;  //画板长度
    qreal height;  //画板宽度
    QList<CGraphicsUnit> vecGraphicsUnit; //所有图元集合
};


#pragma pack(pop)

#endif // SITEMDATA_H
