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
#ifndef CDDFMANAGER_H
#define CDDFMANAGER_H

#include <QObject>
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"

/*
inline QDataStream &operator<<(QDataStream &out, const CGraphics &graphics)
{
    out << graphics.unitCount;
    out << graphics.rect;

    foreach (CGraphicsUnit unit, graphics.vecGraphicsUnit) {

        ///head
        out << unit.head.headCheck[0];
        out << unit.head.headCheck[1];
        out << unit.head.headCheck[2];
        out << unit.head.headCheck[3];
        out << unit.head.dataType;
        out << unit.head.dataLength;
        out << unit.head.pen;
        out << unit.head.brush;
        out << unit.head.pos;
        out << unit.head.rotate;
        out << unit.head.zValue;
        //body
        if (RectType == unit.head.dataType && nullptr != unit.data.pRect) {
            out << unit.data.pRect->topLeft;
            out << unit.data.pRect->bottomRight;

        } else if (EllipseType == unit.head.dataType && nullptr != unit.data.pCircle) {
            out << unit.data.pCircle->rect.topLeft;
            out << unit.data.pCircle->rect.bottomRight;
        } else if (TriangleType == unit.head.dataType && nullptr != unit.data.pTriangle) {
            out << unit.data.pTriangle->rect.topLeft;
            out << unit.data.pTriangle->rect.bottomRight;

        } else if (PolygonType == unit.head.dataType && nullptr != unit.data.pPolygon) {
            out << unit.data.pPolygon->rect.topLeft;
            out << unit.data.pPolygon->rect.bottomRight;
            out << unit.data.pPolygon->pointNum;
        } else if (PolygonalStarType == unit.head.dataType && nullptr != unit.data.pPolygonStar) {
            out << unit.data.pPolygonStar->rect.topLeft;
            out << unit.data.pPolygonStar->rect.bottomRight;
            out << unit.data.pPolygonStar->anchorNum;
            out << unit.data.pPolygonStar->radius;
        } else if (LineType == unit.head.dataType && nullptr != unit.data.pLine) {
            out << unit.data.pLine->point1;
            out << unit.data.pLine->point2;
        } else if (TextType == unit.head.dataType && nullptr != unit.data.pText) {
            out << unit.data.pText->rect.topLeft;
            out << unit.data.pText->rect.bottomRight;
            out << unit.data.pText->font;
            out << unit.data.pText->content;
        } else if (PictureType == unit.head.dataType && nullptr != unit.data.pPic) {
            out << unit.data.pPic->rect.topLeft;
            out << unit.data.pPic->rect.bottomRight;
            out << unit.data.pPic->image;
        } else if (PenType == unit.head.dataType && nullptr != unit.data.pPen) {
            out << unit.data.pPen->penType;
            out << unit.data.pPen->path;
            out << unit.data.pPen->arrow;
            out << unit.data.pPen->poitsVector;
        }
        //tail
        out << unit.tail.tailCheck[0];
        out << unit.tail.tailCheck[1];
        out << unit.tail.tailCheck[2];
        out << unit.tail.tailCheck[3];
    }

    return out;
}

inline QDataStream &operator>>(QDataStream &in, CGraphics &graphics)
{
    in >> graphics.unitCount;
    in >> graphics.rect;

//    QList<CGraphicsUnit> vecGraphicsUnit;

    for (int i = 0; i < graphics.unitCount; i++) {

        CGraphicsUnit unit;

        ///head
        in >> unit.head.headCheck[0];
        in >> unit.head.headCheck[1];
        in >> unit.head.headCheck[2];
        in >> unit.head.headCheck[3];
        in >> unit.head.dataType;
        in >> unit.head.dataLength;
        in >> unit.head.pen;
        in >> unit.head.brush;
        in >> unit.head.pos;
        in >> unit.head.rotate;
        in >> unit.head.zValue;
        //body
        if (RectType == unit.head.dataType) {
            SGraphicsRectUnitData *data = new  SGraphicsRectUnitData();
            in >> data->topLeft;
            in >> data->bottomRight;
            unit.data.pRect = data;
        } else if (EllipseType == unit.head.dataType) {
            SGraphicsCircleUnitData *data = new  SGraphicsCircleUnitData();
            in >> data->rect.topLeft;
            in >> data->rect.bottomRight;
            unit.data.pCircle = data;
        } else if (TriangleType == unit.head.dataType) {
            SGraphicsTriangleUnitData *data = new SGraphicsTriangleUnitData();
            in >> data->rect.topLeft;
            in >> data->rect.bottomRight;
            unit.data.pTriangle = data;
        } else if (PolygonType == unit.head.dataType) {
            SGraphicsPolygonUnitData *data = new SGraphicsPolygonUnitData();
            in >> data->rect.topLeft;
            in >> data->rect.bottomRight;
            in >> data->pointNum;
            unit.data.pPolygon = data;
        } else if (PolygonalStarType == unit.head.dataType) {
            SGraphicsPolygonStarUnitData *data = new SGraphicsPolygonStarUnitData();
            in >> data->rect.topLeft;
            in >> data->rect.bottomRight;
            in >> data->anchorNum;
            in >> data->radius;
            unit.data.pPolygonStar = data;
        } else if (LineType == unit.head.dataType) {
            SGraphicsLineUnitData *data = new SGraphicsLineUnitData();
            in >> data->point1;
            in >> data->point2;
            unit.data.pLine = data;
        } else if (TextType == unit.head.dataType) {
            SGraphicsTextUnitData *data = new SGraphicsTextUnitData();
            in >> data->rect.topLeft;
            in >> data->rect.bottomRight;
            in >> data->font;
            in >> data->content;
            unit.data.pText = data;
        } else if (PictureType == unit.head.dataType) {
            SGraphicsPictureUnitData *data = new SGraphicsPictureUnitData();
            in >> data->rect.topLeft;
            in >> data->rect.bottomRight;
            in >> data->image;
            unit.data.pPic = data;
        } else if (PenType == unit.head.dataType) {
            SGraphicsPenUnitData *data = new SGraphicsPenUnitData();
            in >> data->penType;
            in >> data->path;
            in >> data->arrow;
            in >> data->poitsVector;

            unit.data.pPen = data;
        }

        //tail
        in >> unit.tail.tailCheck[0];
        in >> unit.tail.tailCheck[1];
        in >> unit.tail.tailCheck[2];
        in >> unit.tail.tailCheck[3];

        graphics.vecGraphicsUnit.push_back(unit);
    }

    return in;
}

*/

class QGraphicsItem;
class QGraphicsScene;
class CGraphicsView;

class CDDFManager : public QObject
{
    Q_OBJECT

public:
    explicit CDDFManager(QObject *parent = nullptr);

    bool saveToDDF(const QString &path, const QGraphicsScene *scene);

    bool loadDDF(const QString &path, QGraphicsScene *scene, CGraphicsView *view);

public slots:

private:

};

#endif // CDDFMANAGER_H
