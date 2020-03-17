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
#ifndef SITEMDATA_H
#define SITEMDATA_H
#include <QDataStream>
#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QTextDocument>
#include "drawshape/globaldefine.h"

#pragma pack(push, 1)
//定义图元类型


////画笔
//struct SPen {
//    qint32 width;
//    QColor col;
//};

////画刷
//struct SBrush {
//    QColor col;
//};

//版本号
enum VersionNum {RoundRect = 1};

//图元头部
struct SGraphicsUnitHead {
    qint8 headCheck[4];          //头部校验
    qint32 dataType;             //图元类型
    qint64 dataLength;           //数据长度
    QPen    pen;                 //图元使用的画笔信息
    QBrush  brush;               //图元使用的画刷信息
    QPointF  pos;                //图元起始位置
    qreal rotate;                //旋转角度
    qreal zValue;                //Z值 用来保存图形层次

    friend QDataStream &operator<<(QDataStream &out, const SGraphicsUnitHead &head)
    {
        out << head.headCheck[0];
        out << head.headCheck[1];
        out << head.headCheck[2];
        out << head.headCheck[3];
        out << head.dataType;
        out << head.dataLength;
        out << head.pen;
        out << head.brush;
        out << head.pos;
        out << head.rotate;
        out << head.zValue;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsUnitHead &head)
    {
        in >> head.headCheck[0];
        in >> head.headCheck[1];
        in >> head.headCheck[2];
        in >> head.headCheck[3];
        in >> head.dataType;
        in >> head.dataLength;
        in >> head.pen;
        in >> head.brush;
        in >> head.pos;
        in >> head.rotate;
        in >> head.zValue;
        return in;
    }
};

//图元尾部
struct SGraphicsUnitTail {
    qint8 tailCheck[4]; //尾部校验


    friend QDataStream &operator<<(QDataStream &out, const SGraphicsUnitTail &tail)
    {
        out << tail.tailCheck[0];
        out << tail.tailCheck[1];
        out << tail.tailCheck[2];
        out << tail.tailCheck[3];

        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsUnitTail &tail)
    {
        in >> tail.tailCheck[0];
        in >> tail.tailCheck[1];
        in >> tail.tailCheck[2];
        in >> tail.tailCheck[3];

        return in;
    }
};

//矩形
struct SGraphicsRectUnitData {
    QPointF topLeft;
    QPointF bottomRight;
    int xRedius;
    int yRedius;
    friend QDataStream &operator<<(QDataStream &out, const SGraphicsRectUnitData &rectUnitData)
    {
        out << rectUnitData.topLeft;
        out << rectUnitData.bottomRight;
        out << rectUnitData.xRedius;
        out << rectUnitData.xRedius;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsRectUnitData &rectUnitData)
    {
        qint64 pos = in.device()->pos();
        in.device()->seek(0);
        quint32 type;
        in >> type;
        int version;
        in >> version;
        in.device()->seek(pos);
        if (type == (quint32)0xA0B0C0D0 && version == RoundRect) {
            in >> rectUnitData.topLeft;
            in >> rectUnitData.bottomRight;
            in >> rectUnitData.xRedius;
            in >> rectUnitData.yRedius;
        } else {
            in >> rectUnitData.topLeft;
            in >> rectUnitData.bottomRight;
            rectUnitData.xRedius = 0;
            rectUnitData.yRedius = 0;
        }

        return in;
    }
};

//圆
struct SGraphicsCircleUnitData {
    SGraphicsRectUnitData rect;
    friend  QDataStream &operator<<(QDataStream &out, const SGraphicsCircleUnitData &cirUnitData)
    {
        out << cirUnitData.rect;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsCircleUnitData &cirUnitData)
    {
        in >> cirUnitData.rect;
        return in;
    }
};

//三角形
struct SGraphicsTriangleUnitData {
    SGraphicsRectUnitData rect;
    //TODO 序列化和反序列化
    friend  QDataStream &operator<<(QDataStream &out, const SGraphicsTriangleUnitData &triangleUnitData)
    {
        out << triangleUnitData.rect;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsTriangleUnitData &triangleUnitData)
    {
        in >> triangleUnitData.rect;
        return in;
    }
};

//多边形
struct SGraphicsPolygonUnitData {
    SGraphicsRectUnitData rect; //外接矩形
    qint32 pointNum;

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsPolygonUnitData &polygonUnitData)
    {
        out << polygonUnitData.rect;
        out << polygonUnitData.pointNum;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsPolygonUnitData &polygonUnitData)
    {
        in >> polygonUnitData.rect;
        in >> polygonUnitData.pointNum;
        return in;
    }
};

//多角星
struct SGraphicsPolygonStarUnitData {
    SGraphicsRectUnitData rect; //外接矩形
    qint32 anchorNum;
    qint32 radius;      //内接半径

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsPolygonStarUnitData &polygonStarUnitData)
    {
        out << polygonStarUnitData.rect;
        out << polygonStarUnitData.anchorNum;
        out << polygonStarUnitData.radius;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsPolygonStarUnitData &polygonStarUnitData)
    {
        in >> polygonStarUnitData.rect;
        in >> polygonStarUnitData.anchorNum;
        in >> polygonStarUnitData.radius;
        return in;
    }
};

//线
struct SGraphicsLineUnitData {
    QPointF point1;
    QPointF point2;
    qint32 type;//0: 无剪头 1: 有前头

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsLineUnitData &lineUnitData)
    {
        out << lineUnitData.point1;
        out << lineUnitData.point2;
        out << lineUnitData.type;

        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsLineUnitData &lineUnitData)
    {
        in >> lineUnitData.point1;
        in >> lineUnitData.point2;
        in >> lineUnitData.type;
        return in;
    }
};

//文本
struct SGraphicsTextUnitData {
    SGraphicsRectUnitData rect;
    QFont font;
    bool manResizeFlag;
    QString content;

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsTextUnitData &textUnitData)
    {
        out << textUnitData.rect;
        out << textUnitData.font;
        out << textUnitData.manResizeFlag;
        out << textUnitData.content;

        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsTextUnitData &textUnitData)
    {
        in >> textUnitData.rect;
        in >> textUnitData.font;
        in >> textUnitData.manResizeFlag;
        in >> textUnitData.content;

        return in;
    }
};

//图片
struct SGraphicsPictureUnitData {
    SGraphicsRectUnitData rect;
    QImage image;

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsPictureUnitData &pictureUnitData)
    {
        out << pictureUnitData.rect;
        out << pictureUnitData.image;

        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsPictureUnitData &pictureUnitData)
    {
        in >> pictureUnitData.rect;
        in >> pictureUnitData.image;

        return in;
    }
};

//画笔
struct SGraphicsPenUnitData {
    qint8 penType;
    QPainterPath path;
    QPolygonF arrow;

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsPenUnitData &penUnitData)
    {
        out << penUnitData.penType;
        out << penUnitData.path;
        out << penUnitData.arrow;

        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsPenUnitData &penUnitData)
    {
        in >> penUnitData.penType;
        in >> penUnitData.path;
        in >> penUnitData.arrow;


        return in;
    }
};

struct SGraphicsBlurUnitData {
    SGraphicsPenUnitData data;
    qint8 effect;

    friend  QDataStream &operator << (QDataStream &out, const SGraphicsBlurUnitData &blurUnitData)
    {
        out << blurUnitData.data;
        out << blurUnitData.effect;

        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, SGraphicsBlurUnitData &blurUnitData)
    {
        in >> blurUnitData.data;
        in >> blurUnitData.effect;
        return in;
    }
};

//数据封装
union CGraphicsItemData {
    SGraphicsRectUnitData *pRect;                 //矩形图元数据
    SGraphicsCircleUnitData *pCircle;             //椭圆图元数据
    SGraphicsTriangleUnitData *pTriangle;         //三角形图元数据
    SGraphicsPolygonUnitData *pPolygon;           //多边形图元数据
    SGraphicsPolygonStarUnitData *pPolygonStar;   //星形图元数据
    SGraphicsLineUnitData *pLine;                 //直线图元数据
    SGraphicsTextUnitData *pText;                 //文字图元数据
    SGraphicsPictureUnitData *pPic;               //图片图元数据
    SGraphicsPenUnitData *pPen;                   //画笔图元数据
    SGraphicsBlurUnitData *pBlur;                 //模糊图元数据

    CGraphicsItemData() {
        pRect = nullptr;
        pCircle = nullptr;
        pPolygon = nullptr;
        pPolygonStar = nullptr;
        pLine = nullptr;
        pText = nullptr;
        pPic = nullptr;
        pPen = nullptr;
        pBlur = nullptr;
    }

//    friend  QDataStream &operator << (QDataStream &out, const CGraphicsItemData &itemData) {
//        out << penUnitData.penType;
//        out << penUnitData.path;
//        out << penUnitData.arrow;
//        out << penUnitData.poitsVector;

//        return out;
//    }

//    friend QDataStream &operator >>( QDataStream &in, CGraphicsItemData &itemData) {
//        in >> penUnitData.penType;
//        in >> penUnitData.path;
//        in >> penUnitData.arrow;
//        in >> penUnitData.poitsVector;

//        return in;
//    }

};


//单个图数据
struct CGraphicsUnit {
    SGraphicsUnitHead head;          //单个图元的头部信息及校验
    CGraphicsItemData data;          //单个图元的数据部分
    SGraphicsUnitTail tail;          //单个图元的尾部校验


    friend  QDataStream &operator << (QDataStream &out, const CGraphicsUnit &graphicsUnitData)
    {
        out << graphicsUnitData.head;

        if (RectType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pRect) {
            out << *(graphicsUnitData.data.pRect);
        } else if (EllipseType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pCircle) {
            out << *(graphicsUnitData.data.pCircle);
        } else if (TriangleType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pTriangle) {
            out << *(graphicsUnitData.data.pTriangle);
        } else if (PolygonType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pPolygon) {
            out << *(graphicsUnitData.data.pPolygon);
        } else if (PolygonalStarType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pPolygonStar) {
            out << *(graphicsUnitData.data.pPolygonStar);
        } else if (LineType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pLine) {
            out << *(graphicsUnitData.data.pLine);
        } else if (TextType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pText) {
            out << *(graphicsUnitData.data.pText);
        } else if (PictureType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pPic) {
            out << *(graphicsUnitData.data.pPic);
        } else if (PenType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pPen) {
            out << *(graphicsUnitData.data.pPen);
        } else if (BlurType == graphicsUnitData.head.dataType && nullptr != graphicsUnitData.data.pBlur) {
            out << *(graphicsUnitData.data.pBlur);
        }

        out << graphicsUnitData.tail;
        return out;
    }

    friend QDataStream &operator >>( QDataStream &in, CGraphicsUnit &graphicsUnitData)
    {

        in >> graphicsUnitData.head;

        if (RectType == graphicsUnitData.head.dataType) {
            SGraphicsRectUnitData *data = new  SGraphicsRectUnitData();
            in >> *data;
            graphicsUnitData.data.pRect = data;
        } else if (EllipseType == graphicsUnitData.head.dataType) {
            SGraphicsCircleUnitData *data = new  SGraphicsCircleUnitData();
            in >> *data;
            graphicsUnitData.data.pCircle = data;
        } else if (TriangleType == graphicsUnitData.head.dataType) {
            SGraphicsTriangleUnitData *data = new SGraphicsTriangleUnitData();
            in >> *data;
            graphicsUnitData.data.pTriangle = data;
        } else if (PolygonType == graphicsUnitData.head.dataType) {
            SGraphicsPolygonUnitData *data = new SGraphicsPolygonUnitData();
            in >> *data;
            graphicsUnitData.data.pPolygon = data;
        } else if (PolygonalStarType == graphicsUnitData.head.dataType) {
            SGraphicsPolygonStarUnitData *data = new SGraphicsPolygonStarUnitData();
            in >> *data;
            graphicsUnitData.data.pPolygonStar = data;
        } else if (LineType == graphicsUnitData.head.dataType) {
            SGraphicsLineUnitData *data = new SGraphicsLineUnitData();
            in >> *data;
            graphicsUnitData.data.pLine = data;
        } else if (TextType == graphicsUnitData.head.dataType) {
            SGraphicsTextUnitData *data = new SGraphicsTextUnitData();
            in >> *data;
            graphicsUnitData.data.pText = data;
        } else if (PictureType == graphicsUnitData.head.dataType) {
            SGraphicsPictureUnitData *data = new SGraphicsPictureUnitData();
            in >> *data;
            graphicsUnitData.data.pPic = data;
        } else if (PenType == graphicsUnitData.head.dataType) {
            SGraphicsPenUnitData *data = new SGraphicsPenUnitData();
            in >> *data;
            graphicsUnitData.data.pPen = data;
        } else if (BlurType == graphicsUnitData.head.dataType) {
            SGraphicsBlurUnitData *data = new SGraphicsBlurUnitData();
            in >> *data;
            graphicsUnitData.data.pBlur = data;
        }

        in >> graphicsUnitData.tail;

        return in;
    }

};

//整个图元数据
struct CGraphics {
    qint64 version;   //数据版本
    qint64 unitCount;   //图元数量
    QRectF rect;    // 画板大小和位置
    QVector<CGraphicsUnit> vecGraphicsUnit; //所有图元集合

    friend QDataStream &operator<<(QDataStream &out, const CGraphics &graphics)
    {
        out << graphics.unitCount;
        out << graphics.rect;
        out << graphics.vecGraphicsUnit;

        return out;
    }

    friend  QDataStream &operator>>(QDataStream &in, CGraphics &graphics)
    {
        in >> graphics.unitCount;
        in >> graphics.rect;
        in >> graphics.vecGraphicsUnit;

        return in;
    }
};


#pragma pack(pop)

#endif // SITEMDATA_H
