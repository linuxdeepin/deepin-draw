#include "shapesutils.h"

#include <QDebug>

Toolshape::Toolshape() {
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));

    rotatedPoints.append(QPointF(0, 0));
    rotatedPoints.append(QPointF(0, 0));
    rotatedPoints.append(QPointF(0, 0));
    rotatedPoints.append(QPointF(0, 0));
    portion.clear();
}

Toolshape::~Toolshape() {
}

void Toolshape::registerMetaType() {
    qRegisterMetaType<Toolshape>();
}

QDebug &operator<<(QDebug &argument, const Toolshape &obj) {
    argument.nospace()
            << obj.type << ","
            << "[" << obj.mainPoints << "]" << ","
            << "[" << obj.rotatedPoints << "]" << ","
            << obj.index<<","
            << obj.lineWidth << ","
            << obj.colorIndex <<","
            << obj.fillColor << ","
            << obj.strokeColor << ","
            << obj.isBlur << ","
            << obj.isMosaic << ","
            << obj.isStraight << ","
            << obj.isShiftPressed << ","
            << obj.imagePath << ","
            << obj.editImagePath << ","
            << obj.rotate << ","
            << obj.imageSize << ","
            << obj.fontSize << ","
            << obj.points;
    return argument.space();
}

QDataStream &operator>>(QDataStream &in, Toolshape &obj) {
    in >> obj.points;
    in >> obj.imageSize;
    in >> obj.rotate;
    in >> obj.editImagePath;
    in >> obj.imagePath;
    in >> obj.fontSize;
    in >> obj.isShiftPressed;
    in >> obj.isBlur;
    in >> obj.isMosaic;
    in >> obj.isStraight;
    in >> obj.fillColor;
    in >> obj.strokeColor;
    in >> obj.colorIndex;
    in >> obj.lineWidth;
    in >> obj.index;
    in >> obj.rotatedPoints;
    in >> obj.mainPoints;
    in >> obj.type;

    return in;
}

Toolshape Toolshape::operator=(Toolshape obj) {
    type = obj.type;
    mainPoints = obj.mainPoints;
    rotatedPoints = obj.rotatedPoints;
    index = obj.index;
    lineWidth = obj.lineWidth;
    colorIndex = obj.colorIndex;
    fillColor = obj.fillColor;
    strokeColor = obj.strokeColor;
    isBlur = obj.isBlur;
    isMosaic = obj.isMosaic;
    isStraight = obj.isStraight;
    isShiftPressed = obj.isShiftPressed;
    imagePath = obj.imagePath;
    editImagePath = obj.editImagePath;
    rotate = obj.rotate;
    imageSize = obj.imageSize;
    fontSize = obj.fontSize;
    points = obj.points;

    return (*this);
}

bool Toolshape::operator==(const Toolshape &other) const {
    if (this->mainPoints == other.mainPoints && this->rotatedPoints == other.rotatedPoints
            && this->index == other.index && this->lineWidth == other.lineWidth
            && this->colorIndex == other.colorIndex && this->fontSize == other.fontSize
            && this->fillColor == other.fillColor && this->strokeColor == other.strokeColor
            && this->isBlur == other.isBlur && this->isMosaic == other.isMosaic
            && this->isShiftPressed == other.isShiftPressed
            && this->imagePath == other.imagePath && this->editImagePath == other.editImagePath
            && this->rotate == other.rotate
            && this->isStraight == other.isStraight && this->points == other.points) {
        return true;
    } else {
        return false;
    }
}
