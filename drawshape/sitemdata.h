#ifndef SITEMDATA_H
#define SITEMDATA_H
#include <QDataStream>
#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QTextDocument>
enum GraphicsItemType {
    picture,
    circle,
    rect,
    polygon
};

#pragma pack(push,1)
//定义图元类型
struct SColor {
    quint8 r;
    quint8 g;
    quint8 b;
    quint8 a;
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
struct SGraphicUnitHead {
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
struct SGraphicUnitTail {
    qint8 tailCheck[4]; //尾部校验
};

//矩形
struct SGraphicRectUnitData {
    QPointF point1;
    QPointF point2;
};

//圆
struct SGraphicCircleUnitData {
    SGraphicRectUnitData rect;
    //TODO 序列化和反序列化
};

//多边形
struct SGraphicsPolygonUnitData {
    SGraphicRectUnitData rect; //外接矩形
    quint8 pointNum;
};

//多角星
struct SGraphicsPolygonStarUnitData {
    SGraphicRectUnitData rect; //外接矩形
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
    SGraphicRectUnitData rect;
    char strText[1024 * 1024 + 1]; //文本信息以HTML存储富文本内容 最大支持1024×1024字节
};

//图片
struct SGraphicPictureUnitData {
    qint32 length;   //图片长度
    const char *pic;  //图片
};

//画笔
struct SGraphicPenUnitData {
    qint32 length;
    QPointF *point;
};

//数据封装
union CGraphicItemData {
    SGraphicRectUnitData *pRect;
    SGraphicCircleUnitData *pCircle;
    SGraphicsPolygonUnitData *pPolygon;
    SGraphicsPolygonStarUnitData *pPolygonStar;
    SGraphicsLineUnitData *pLine;
    SGraphicsTextUnitData *pText;
    SGraphicPictureUnitData *pPic;
};


//单个图数据
struct CGraphicUnit {
    SGraphicUnitHead head;
    CGraphicItemData data;
    SGraphicUnitTail tail;
};

//整个图元数据
struct CGraphic {
    qint64 unitCount;   //图元数量
    qreal width;  //画板长度
    qreal height;  //画板宽度
    QList<CGraphicUnit> vecGraphicUnit; //所有图元集合
};


#pragma pack(pop)

#endif // SITEMDATA_H
