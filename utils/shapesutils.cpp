#include "shapesutils.h"

#include <QDebug>

Toolshape::Toolshape()
{
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
    qRegisterMetaType<Toolshape>();
    qRegisterMetaType<FourPoints>();
}

const QPointF Toolshape::topLeftPointF()
{
    qreal x1 = 8000, y1 = 8000;
    for(int i = 0; i < mainPoints.length(); i++)
    {
        x1 = std::min(x1, mainPoints[i].x() - lineWidth);
        y1 = std::min(y1, mainPoints[i].y() - lineWidth);
    }
    for(int j = 0; j < points.length(); j++)
    {
        x1 = std::min(x1, points[j].x() - lineWidth);
        y1 = std::min(y1, points[j].y() - lineWidth);
    }

    qDebug() << "XXXXX" << lineWidth;
    return QPointF(x1, y1);
}

const QPointF Toolshape::bottomRightPointF()
{
    qreal x2 = 0, y2 = 0;
    for(int i = 0; i < mainPoints.length(); i++)
    {
        x2 = std::max(x2, mainPoints[i].x());
        y2 = std::max(y2, mainPoints[i].y());
    }
    for(int j = 0; j < points.length(); j++)
    {
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
            << obj.index<<","
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
            && this->rotate == other.rotate
            && this->isStraight == other.isStraight && this->points == other.points) {
        return true;
    } else {
        return false;
    }
}
