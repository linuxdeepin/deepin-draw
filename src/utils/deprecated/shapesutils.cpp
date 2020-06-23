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
#include "shapesutils.h"

#include <QDebug>

Toolshape::Toolshape()
{
    registerMetaType();
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    portion.clear();
}

Toolshape::~Toolshape()
{
}

void Toolshape::registerMetaType()
{
    qRegisterMetaType<Toolshape>("Toolshape");
    qRegisterMetaType<Toolshapes>("Toolshapes");
    qRegisterMetaType<FourPoints>("FourPoints");
}

const QPointF Toolshape::topLeftPointF()
{
    qreal x1 = 8000, y1 = 8000;
    for (int i = 0; i < mainPoints.length(); i++) {
        x1 = std::min(x1, mainPoints[i].x() - lineWidth);
        y1 = std::min(y1, mainPoints[i].y() - lineWidth);
    }
    for (int j = 0; j < points.length(); j++) {
        x1 = std::min(x1, points[j].x() - lineWidth);
        y1 = std::min(y1, points[j].y() - lineWidth);
    }

    qDebug() << "XXXXX" << lineWidth;
    return QPointF(x1, y1);
}

const QPointF Toolshape::bottomRightPointF()
{
    qreal x2 = 0, y2 = 0;
    for (int i = 0; i < mainPoints.length(); i++) {
        x2 = std::max(x2, mainPoints[i].x());
        y2 = std::max(y2, mainPoints[i].y());
    }
    for (int j = 0; j < points.length(); j++) {
        x2 = std::max(x2, points[j].x());
        y2 = std::max(y2, points[j].y());
    }
    qDebug() << "YYYYY" << lineWidth;
    return QPointF(x2 + lineWidth, y2 + lineWidth);
}

QDebug &operator<<(QDebug &argument, const Toolshape &obj)
{
    argument.nospace()
            << obj.type << ","
            << "[" << obj.mainPoints << "]" << ","
            << obj.index << ","
            << obj.lineWidth << ","
            << obj.fillColor << ","
            << obj.strokeColor << ","
            << obj.isBlur << ","
            << obj.isMosaic << ","
            << obj.isStraight << ","
            << obj.isHorFlip << ","
            << obj.isVerFlip << ","
            << obj.imagePath << ","
            << obj.text << ","
            << obj.rotate << ","
            << obj.scaledRation << ","
            << obj.imageSize << ","
            << obj.fontSize << ","
            << obj.points;
    return argument.space();
}

QDataStream &operator>>(QDataStream &in, Toolshape &obj)
{
    in >> obj.points;
    in >> obj.imageSize;
    in >> obj.rotate;
    in >> obj.scaledRation;
    in >> obj.text;
    in >> obj.imagePath;
    in >> obj.fontSize;
    in >> obj.isHorFlip;
    in >> obj.isVerFlip;
    in >> obj.isBlur;
    in >> obj.isMosaic;
    in >> obj.isStraight;
    in >> obj.fillColor;
    in >> obj.strokeColor;
    in >> obj.lineWidth;
    in >> obj.index;
    in >> obj.mainPoints;
    in >> obj.type;

    return in;
}

Toolshape Toolshape::operator=(Toolshape obj)
{
    type = obj.type;
    mainPoints = obj.mainPoints;
    index = obj.index;
    lineWidth = obj.lineWidth;
    fillColor = obj.fillColor;
    strokeColor = obj.strokeColor;
    isBlur = obj.isBlur;
    isMosaic = obj.isMosaic;
    isStraight = obj.isStraight;
    isHorFlip = obj.isHorFlip;
    isVerFlip = obj.isVerFlip;
    imagePath = obj.imagePath;
    text = obj.text;
    rotate = obj.rotate;
    scaledRation = obj.scaledRation;
    imageSize = obj.imageSize;
    fontSize = obj.fontSize;
    points = obj.points;

    return (*this);
}

bool Toolshape::operator==(const Toolshape &other) const
{
    if (this->mainPoints == other.mainPoints
            && this->index == other.index && this->lineWidth == other.lineWidth
            && this->fontSize == other.fontSize
            && this->fillColor == other.fillColor && this->strokeColor == other.strokeColor
            && this->isBlur == other.isBlur && this->isMosaic == other.isMosaic
            && this->isHorFlip == other.isHorFlip && this->isVerFlip == other.isVerFlip
            && this->imagePath == other.imagePath && this->text == other.text
            && this->scaledRation == other.scaledRation && this->rotate == other.rotate
            && this->isStraight == other.isStraight && this->points == other.points) {
        return true;
    } else {
        return false;
    }
}
