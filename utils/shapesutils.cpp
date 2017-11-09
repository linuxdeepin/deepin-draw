#include "shapesutils.h"

#include <QDebug>

Toolshape::Toolshape() {
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    mainPoints.append(QPointF(0, 0));
    portion.clear();
}

Toolshape::~Toolshape() {
}

void Toolshape::registerMetaType() {
    qRegisterMetaType<Toolshape>();
    qRegisterMetaType<FourPoints>();
}

QDebug &operator<<(QDebug &argument, const Toolshape &obj) {
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
            << obj.isShiftPressed << ","
            << obj.isHorFlip << ","
            << obj.isVerFlip << ","
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
    in >> obj.isHorFlip;
    in >> obj.isVerFlip;
    in >> obj.isShiftPressed;
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

Toolshape Toolshape::operator=(Toolshape obj) {
    type = obj.type;
    mainPoints = obj.mainPoints;
    index = obj.index;
    lineWidth = obj.lineWidth;
    fillColor = obj.fillColor;
    strokeColor = obj.strokeColor;
    isBlur = obj.isBlur;
    isMosaic = obj.isMosaic;
    isStraight = obj.isStraight;
    isShiftPressed = obj.isShiftPressed;
    isHorFlip = obj.isHorFlip;
    isVerFlip = obj.isVerFlip;
    imagePath = obj.imagePath;
    editImagePath = obj.editImagePath;
    rotate = obj.rotate;
    imageSize = obj.imageSize;
    fontSize = obj.fontSize;
    points = obj.points;

    return (*this);
}

bool Toolshape::operator==(const Toolshape &other) const {
    if (this->mainPoints == other.mainPoints
            && this->index == other.index && this->lineWidth == other.lineWidth
            && this->fontSize == other.fontSize
            && this->fillColor == other.fillColor && this->strokeColor == other.strokeColor
            && this->isBlur == other.isBlur && this->isMosaic == other.isMosaic
            && this->isShiftPressed == other.isShiftPressed
            && this->isHorFlip == other.isHorFlip && this->isVerFlip == other.isVerFlip
            && this->imagePath == other.imagePath && this->editImagePath == other.editImagePath
            && this->rotate == other.rotate
            && this->isStraight == other.isStraight && this->points == other.points) {
        return true;
    } else {
        return false;
    }
}
