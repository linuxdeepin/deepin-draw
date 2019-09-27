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
#include "calculaterect.h"
#include <cmath>

const int PADDING = 5;
const int ROTATEPOINT_PADDING = 30;
const int TANT_EDGEVALUE = 0.78539;
const int TANT2_EDGEVALUE = 2.35619;
const int MIN_PADDING = 3;
const qreal SLOPE = 0.5522848;

/* judge whether the point1 is on the point2 or not */
bool pointClickIn(QPointF point2, QPointF point1, int PADDING)
{
    if (point2.x() >= point1.x() - PADDING && point2.x() <= point1.x() + PADDING &&
            point2.y() >= point1.y() - PADDING && point2.y() <= point1.y() + PADDING) {
        return true;
    } else {
        return false;
    }
}

/* judge whether the point3 is on the segment*/
bool pointOnLine(QPointF point1, QPointF point2, QPointF point3)
{
    if (point1.x() == point2.x()) {
        if (point3.x() >= point1.x() - PADDING && point3.x() <= point1.x() + PADDING &&
                point3.y() >= std::min(point1.y(), point2.y()) - PADDING && point3.y() <= std::max(point1.y(), point2.y()) + PADDING) {
            return true;
        }
    } else {
        qreal k =  (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal b = point1.y() - point1.x() * k;

        if ( point3.x() >= std::min(point1.x(), point2.x()) - PADDING && point3.x() <= std::max(point1.x(), point2.x() + PADDING)
                && point3.y() >= k * point3.x() + b - PADDING && point3.y() <= k * point3.x() + b + PADDING) {
            return true;
        }
    }
    return false;
}

/* get the distance between two points*/
qreal getDistance(QPointF point1, QPointF point2)
{
    return std::sqrt(std::pow(point1.x() - point2.x(), 2) + std::pow(point1.y() - point2.y(), 2));
}

/* get the point who splid a distance on a line */
QPointF pointSplid(QPointF point1, QPointF point2, qreal PADDING)
{
    if (point1.x() == point2.x()) {
        return QPointF(0, PADDING);
    } else {
        qreal tmpX = PADDING * std::cos(std::atan2(std::abs(point1.y() - point2.y()), std::abs(point1.x() - point2.x())));
        qreal tmpY = PADDING * std::sin(std::atan2(std::abs(point1.y() - point2.y()), std::abs(point1.x() - point2.x())));
        return QPointF(tmpX, tmpY);
    }
}

/* get the rotate point by four points in a rectangle*/
QPointF getRotatePoint(QPointF point1, QPointF point2, QPointF point3, QPointF point4)
{
    QPointF leftPoint = QPointF(0, 0);
    QPointF rightPoint = QPointF(0, 0);
    QPointF rotatePoint = QPointF(0, 0);

    QPointF leftSplidPoint = pointSplid(point1, point2, ROTATEPOINT_PADDING);
    QPointF rightSplidPoint = pointSplid(point3, point4, ROTATEPOINT_PADDING);

    /* first position*/
    if (point2.x() - point4.x() <= 0 && point2.y() - point4.y() >= 0) {
        leftPoint = QPointF(point1.x() - leftSplidPoint.x(), point1.y() - leftSplidPoint.y());
        rightPoint = QPointF(point3.x() - rightSplidPoint.x(), point3.y() - rightSplidPoint.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }

    /* second position*/
    if (point2.x() - point4.x() > 0 && point2.y() - point4.y() > 0) {
        leftPoint = QPointF(point1.x() - leftSplidPoint.x(), point1.y() + leftSplidPoint.y());
        rightPoint = QPointF(point3.x() - rightSplidPoint.x(), point3.y() + rightSplidPoint.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }

    /* third position*/
    if (point2.x() - point4.x() < 0 && point2.y() - point4.y() < 0) {
        leftPoint = QPointF(point1.x() + leftSplidPoint.x(), point1.y() - leftSplidPoint.y());
        rightPoint = QPointF(point3.x() + rightSplidPoint.x(), point3.y() - rightSplidPoint.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }

    /* fourth position*/
    if (point2.x() - point4.x() > 0 && point2.y() - point4.y() < 0) {
        leftPoint = QPointF(point1.x() + leftSplidPoint.x(), point1.y() + leftSplidPoint.y());
        rightPoint = QPointF(point3.x() + rightSplidPoint.x(), point3.y() + rightSplidPoint.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }

    /* seventh position*/
    if (point1.x() == point3.x() && point1.x() > point2.x()) {
        leftPoint = QPointF(point1.x() + leftSplidPoint.x(), point1.y());
        rightPoint = QPointF(point3.x() + rightSplidPoint.x(), point3.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }

    /* sixth position */
    if (point1.x() == point2.x() && point1.x() > point3.x()) {
        leftPoint = QPointF(point1.x(), point1.y() + leftSplidPoint.y());
        rightPoint = QPointF(point3.x(), point3.y() + rightSplidPoint.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }

    /* eight position*/
    if (point2.x() == point4.x() && point2.x() > point1.x()) {
        leftPoint = QPointF(point1.x() - leftSplidPoint.x(), point1.y());
        rightPoint = QPointF(point3.x() - rightSplidPoint.x(), point3.y());
        rotatePoint = QPointF((leftPoint.x() + rightPoint.x()) / 2, (leftPoint.y() + rightPoint.y()) / 2);
        return rotatePoint;
    }
    qDebug() << "Rotate shape failed!!";

    return rotatePoint;
}

/* get the four points from a line */
FourPoints fourPointsOfLine(QList<QPointF> points)
{
    FourPoints resultFPoint;
    const int MIN_PADDING = 10;
    resultFPoint = initFourPoints(resultFPoint);
    if (points.length() < 2) {
        return initFourPoints(resultFPoint);
    }

    QPointF minPointF = points[0];
    QPointF maxPointF = points[0];
    foreach (QPointF point, points) {
        minPointF = QPointF(std::min(minPointF.x(), point.x()), std::min(minPointF.y(), point.y()));
        maxPointF = QPointF(std::max(maxPointF.x(), point.x()), std::max(maxPointF.y(), point.y()));
    }

    resultFPoint[0] = QPointF(minPointF.x() - MIN_PADDING, minPointF.y() - MIN_PADDING);
    resultFPoint[1] = QPointF(minPointF.x() - MIN_PADDING, maxPointF.y() + MIN_PADDING);
    resultFPoint[2] = QPointF(maxPointF.x() + MIN_PADDING, minPointF.y() - MIN_PADDING);
    resultFPoint[3] = QPointF(maxPointF.x() + MIN_PADDING, maxPointF.y() + MIN_PADDING);
    return resultFPoint;
}
FourPoints getAnotherFPoints(FourPoints mainPoints)
{
    FourPoints  otherFPoints;
    otherFPoints = initFourPoints(otherFPoints);
    if (mainPoints.length() != 4) {
        return otherFPoints;
    }
    otherFPoints[0] = QPoint((mainPoints[0].x() + mainPoints[1].x()) / 2,
                             (mainPoints[0].y() + mainPoints[1].y()) / 2);
    otherFPoints[1] = QPoint((mainPoints[0].x() + mainPoints[2].x()) / 2,
                             (mainPoints[0].y() + mainPoints[2].y()) / 2);
    otherFPoints[2] = QPoint((mainPoints[2].x() + mainPoints[3].x()) / 2,
                             (mainPoints[2].y() + mainPoints[3].y()) / 2);
    otherFPoints[3] = (QPoint((mainPoints[1].x() + mainPoints[3].x()) / 2,
                              (mainPoints[1].y() + mainPoints[3].y()) / 2));
    return otherFPoints;
}
/*
 *  this function is get the angle of the mouse'moving*/
/* the angle in point3 */

qreal calculateAngle(QPointF point1, QPointF point2, QPointF point3)
{
    if (point1 == point2) {
        return 0;
    }

    qreal a = std::pow(point1.x() - point3.x(), 2) + std::pow(point1.y() - point3.y(), 2);
    qreal b = std::pow(point2.x() - point3.x(), 2) + std::pow(point2.y() - point3.y(), 2);
    qreal c = std::pow(point1.x() - point2.x(), 2) + std::pow(point1.y() - point2.y(), 2);

    qreal angle = std::cos(( a + b - c) / (2 * std::sqrt(a) * std::sqrt(b)));
    if (point1.x() <= point3.x() && point1.y() < point3.y()) {
        if (point2.x() < point1.x() || point2.y() > point1.y()) {
            angle = - angle;
        }
    }
    if (point1.x() < point3.x() && point1.y() >= point3.y()) {
        if (point2.x() > point1.x() || point2.y() > point1.y()) {
            angle = - angle;
        }
    }
    if (point1.x() >= point3.x() && point1.y() > point3.y()) {
        if (point2.x() > point1.x() || point2.y() < point1.y()) {
            angle = - angle;
        }
    }
    if (point1.x() > point3.x() && point1.y() <= point3.y()) {
        if (point2.x() < point1.x() || point2.y() < point1.y()) {
            angle = - angle;
        }
    }
    return angle;
}

/* point2 is the rotatePoint, point1 is the centerPoint, point3 is point2 who rotate */
QPointF pointRotate(QPointF point1, QPointF point2, qreal angle)
{
    QPointF middlePoint = QPointF(point2.x() - point1.x(), point2.y() - point1.y());
    QPointF tmpPoint = QPointF(middlePoint.x() * std::cos(angle) - middlePoint.y() * std::sin(angle),
                               middlePoint.x() * std::sin(angle) + middlePoint.y() * std::cos(angle));
    QPointF point3 = QPointF(tmpPoint.x() + point1.x(), tmpPoint.y() + point1.y());

    return point3;
}

/* the distance from a point(point3) to a line(point1, point2) */
qreal pointToLineDistance(QPointF point1, QPointF point2, QPointF point3)
{
    if (point1.x() == point2.x()) {
        return std::abs(point3.x() - point1.x());
    } else {
        qreal k = (point1.y() - point2.y()) / (point1.x() - point2.x());
        qreal b = point1.y() - point1.x() * k;
        return std::abs(point3.x() * k + b - point3.y()) / std::sqrt(std::pow(k, 2) + 1);
    }
}

/* judge the direction of point3 of line(point1, point2) */
qreal pointLineDir(QPointF point1, QPointF point2, QPointF point3)
{
    if (point1.x() == point2.x()) {
        if (point3.x() <= point1.x()) {
            return -1;
        } else {
            return 1;
        }
    } else {
        qreal k = (point1.y() - point2.y()) / (point1.x() - point2.x());
        qreal b = point1.y() - point1.x() * k;
        if (point3.y() <= point3.x() * k + b) {
            return -1;
        } else {
            return 1;
        }
    }
}

/* calculate the control point of the beizer */
QPointF getControlPoint(QPointF point1, QPointF point2, bool direction)
{
    qreal k1 = SLOPE;
    qreal k2 = 1 - SLOPE;
    qreal k3;

    if (direction) {
        k3 = k2 / k1;
    } else {
        k3 = k1 / k2;
    }
    QPointF resultPoint = QPoint((point1.x() + k3 * point2.x()) / (1 + k3),
                                 (point1.y() + k3 * point2.y()) / (1 + k3));
    return resultPoint;
}

/* get eight control points */
QList<QPointF> getEightControlPoint(FourPoints rectFPoints)
{
    FourPoints anotherFPoints = getAnotherFPoints(rectFPoints);

    QList<QPointF> resultPointList;
    resultPointList.append(getControlPoint(rectFPoints[0], anotherFPoints[0], true));
    resultPointList.append(getControlPoint(rectFPoints[0], anotherFPoints[1], true));
    resultPointList.append(getControlPoint(anotherFPoints[0], rectFPoints[1], false));
    resultPointList.append(getControlPoint(rectFPoints[1], anotherFPoints[3], true));
    resultPointList.append(getControlPoint(anotherFPoints[1], rectFPoints[2], false));
    resultPointList.append(getControlPoint(rectFPoints[2], anotherFPoints[2], true));
    resultPointList.append(getControlPoint(anotherFPoints[2], rectFPoints[3], false));
    resultPointList.append(getControlPoint(rectFPoints[3], anotherFPoints[3], true));

    return resultPointList;
}

/* judge whether the clickOnPoint is on the bezier */
/* 0 <= pos.x() <= 1*/
bool pointOnBezier(QPointF point1, QPointF point2, QPointF point3, QPointF point4, QPointF pos)
{
    const int MIN_PADDING = 10;
    for (qreal t = 0; t <= 1; t = t + 0.1) {
        qreal bx = point1.x() * (1 - t) * std::pow(1 - t, 2) + 3 * point2.x() * t * std::pow(1 - t, 2)
                   + 3 * point3.x() * std::pow(t, 2) * (1 - t) + point4.x() * t * std::pow(t, 2);
        qreal by = point1.y() * (1 - t) * std::pow(1 - t, 2) + 3 * point2.y() * t * std::pow(1 - t, 2)
                   + 3 * point3.y() * std::pow(t, 2) * (1 - t) + point4.y() * t * std::pow(t, 2);
        if (pos.x() >= bx - MIN_PADDING && pos.x() <= bx + MIN_PADDING &&
                pos.y() >= by - MIN_PADDING && pos.y() <= by + MIN_PADDING) {
            return true;
        }
    }
    return false;
}

/* judge whether the clickOnPoint is on the ellipse */
bool pointOnEllipse(FourPoints rectFPoints, QPointF pos)
{
    FourPoints anotherFPoints = getAnotherFPoints(rectFPoints);
    QList<QPointF> controlPointList;
    controlPointList.append(getControlPoint(rectFPoints[0], anotherFPoints[0], true));
    controlPointList.append(getControlPoint(rectFPoints[0], anotherFPoints[1], true));
    controlPointList.append(getControlPoint(anotherFPoints[0], rectFPoints[1], false));
    controlPointList.append(getControlPoint(rectFPoints[1], anotherFPoints[3], true));
    controlPointList.append(getControlPoint(anotherFPoints[1], rectFPoints[2], false));
    controlPointList.append(getControlPoint(rectFPoints[2], anotherFPoints[2], true));
    controlPointList.append(getControlPoint(anotherFPoints[2], rectFPoints[3], false));
    controlPointList.append(getControlPoint(rectFPoints[3], anotherFPoints[3], true));

    if (pointOnBezier(anotherFPoints[0], controlPointList[0], controlPointList[1],
                      anotherFPoints[1], pos) || pointOnBezier(anotherFPoints[1], controlPointList[4],
                                                               controlPointList[5], anotherFPoints[2], pos) || pointOnBezier(anotherFPoints[2],
                                                                       controlPointList[6], controlPointList[7], anotherFPoints[3], pos) || pointOnBezier(
                anotherFPoints[3], controlPointList[3], controlPointList[2], anotherFPoints[0], pos)) {
        return true;
    } else {
        return false;
    }
}

/* get the three points of arrow A/B/D */
QList<QPointF> pointOfArrow(QPointF startPoint, QPointF endPoint, qreal arrowLength)
{
    qreal xMultiplier, yMultiplier;
    if (startPoint.x() == endPoint.x()) {
        xMultiplier = 1;
    } else {
        xMultiplier = (startPoint.x() - endPoint.x()) / std::abs(startPoint.x() - endPoint.x());
    }

    if (startPoint.y() == endPoint.y()) {
        yMultiplier = 1;
    } else {
        yMultiplier = (startPoint.y() - endPoint.y()) / std::abs(startPoint.y() - endPoint.y());
    }

    QPointF add = pointSplid(startPoint, endPoint, arrowLength * 2);
    QPointF pointA = QPointF(endPoint.x() + xMultiplier * add.x(), endPoint.y() + yMultiplier * add.y());
    qreal angle = qreal(M_PI / 12);
    QPointF pointB = pointRotate(endPoint, pointA, angle);
    QPointF pointD = pointRotate(endPoint, pointA,  angle * 11 + M_PI);
    add = pointSplid(startPoint, endPoint, arrowLength);

    QList<QPointF> arrowPoints;
    arrowPoints.append(pointA);
    arrowPoints.append(pointB);
    arrowPoints.append(pointD);
    arrowPoints.append(endPoint);
    return arrowPoints;
}

/* judge whether the pos is on the points of arbitrary- curved*/
bool pointOnArLine(QList<QPointF> points, QPointF pos, int padding)
{
    for (int i = 0; i < points.length(); i++) {
        if (pointClickIn(points[i], pos, padding)) {
            return true;
        } else {
            continue;
        }
    }

    return false;
}

/* resize arbitrary curved */
QPointF relativePosition(FourPoints mainPoints,  QPointF pos)
{
    if (mainPoints.length() != 4) {
        return QPointF();
    }
    qreal firstRelaPosit, secondRelaPosit;
    qreal distance12 = pointToLineDistance(mainPoints[0], mainPoints[1], pos);
    qreal distance34 = pointToLineDistance(mainPoints[2], mainPoints[3], pos);
    qreal distance13 = pointToLineDistance(mainPoints[0], mainPoints[2], pos);
    qreal distance24 = pointToLineDistance(mainPoints[1], mainPoints[3], pos);

    if (distance34 == 0) {
        firstRelaPosit = -2;
    } else {
        firstRelaPosit = distance12 / distance34;
    }

    if (distance24 == 0) {
        secondRelaPosit = -2;
    } else {
        secondRelaPosit = distance13 / distance24;
    }

    return QPointF(firstRelaPosit, secondRelaPosit);
}

QPointF           getNewPosition(FourPoints mainPoints, QPointF re)
{
    qreal changeX, changeY;

    if (re.x() == -2) {
        changeX = mainPoints[2].x();
        changeY = (mainPoints[0].y() + re.y() * mainPoints[1].y()) / (1 + re.y());
    }
    if (re.y() == -2) {
        changeX = (mainPoints[1].x() + re.x() * mainPoints[3].x()) / (1 + re.x());
        changeY = mainPoints[1].y();
    }
    if (re.x() != -2 && re.y() != -2) {
        QPointF pointi = QPointF((mainPoints[1].x() + re.x() * mainPoints[3].x()) / (1 + re.x()),
                                 (mainPoints[1].y() + re.x() * mainPoints[3].y()) / (1 + re.x()));
        QPointF pointj = QPointF((mainPoints[0].x() + re.y() * mainPoints[1].x()) / (1 + re.y()),
                                 (mainPoints[0].y() + re.y() * mainPoints[1].y()) / (1 + re.y()));
        if (mainPoints[0].x() == mainPoints[1].x()) {
            changeX = pointi.x();
            changeY = pointj.y();
        }
        if (mainPoints[0].x() == mainPoints[2].x()) {
            changeX = pointj.x();
            changeY = pointi.y();
        }
        if (mainPoints[0].x() != mainPoints[1].x() && mainPoints[0].x() != mainPoints[2].x()) {
            qreal k1 = (mainPoints[0].y() - mainPoints[1].y()) / (mainPoints[0].x() - mainPoints[1].x());
            qreal b1 = pointi.y() - k1 * pointi.x();

            qreal k2 = (mainPoints[0].y() - mainPoints[2].y()) / (mainPoints[0].x() - mainPoints[2].x());
            qreal b2 = pointj.y() - k2 * pointj.x();

            changeX = (b1 - b2) / (k2 - k1);
            changeY = changeX * k1 + b1;
        }
    }
    return QPointF(changeX, changeY);
}

/* init FourPoints*/
FourPoints initFourPoints(FourPoints fourPoints)
{
    fourPoints.clear();
    fourPoints.append(QPointF(0, 0));
    fourPoints.append(QPointF(0, 0));
    fourPoints.append(QPointF(0, 0));
    fourPoints.append(QPointF(0, 0));
    return fourPoints;
}

FourPoints mainPointsOrder(FourPoints fourPoints)
{
    FourPoints mainFPoints;
    mainFPoints = initFourPoints(mainFPoints);

    mainFPoints[0] = fourPoints[0];
    mainFPoints[3] = fourPoints[3];
    for (int i = 0; i < fourPoints.length(); i++) {
        mainFPoints[0] = QPointF(
                             std::min(mainFPoints[0].x(), fourPoints[i].x()),
                             std::min(mainFPoints[0].y(), fourPoints[i].y()));

        mainFPoints[3] = QPointF(
                             std::max(mainFPoints[3].x(), fourPoints[i].x()),
                             std::max(mainFPoints[3].y(), fourPoints[i].y()));
    }
    mainFPoints[1] = QPointF(mainFPoints[0].x(), mainFPoints[3].y());
    mainFPoints[2] = QPointF(mainFPoints[3].x(), mainFPoints[0].y());

    return mainFPoints;
}
/* handle resize of eight points in rectangle */
FourPoints resizePointPosition(QPointF point1, QPointF point2, QPointF point3, QPointF point4,
                               QPointF pos, int key, const QString &ration,  bool isShift)
{
    FourPoints resizeFPoints ;
    resizeFPoints = initFourPoints(resizeFPoints);
    resizeFPoints[0] = point1;
    resizeFPoints[1] = point2;
    resizeFPoints[2] = point3;
    resizeFPoints[3] = point4;

    if (qFuzzyCompare(point1.x(), point2.x()) && point1.y() < point2.y()) {
        qDebug() << "resizePointPosition:" << ration;
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize5(point1, point2, point3, point4, pos, ration, isShift);
            return resizeFPoints;
        }
        }
    }

    if (qFuzzyCompare(point1.x(), point2.x()) && point1.y() > point2.y()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize6(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }

    if (qFuzzyCompare(point1.y(), point2.y()) && point1.x() > point2.x()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize7(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (qFuzzyCompare(point1.y(), point2.y()) && point2.x() > point1.x()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize8(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    //position first.
    if (point1.x() - point2.x() < 0 && point1.y() - point2.y() < 0 &&
            point1.x() - point3.x() < 0 && point1.y() - point3.y() > 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize1(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (point1.x() - point2.x() < 0 && point1.y() - point2.y() > 0 &&
            point1.x() - point3.x() > 0 && point1.y() - point3.y() > 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize2(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (point1.x() - point2.x() > 0 && point1.y() - point2.y() < 0 &&
            point1.x() - point3.x() < 0 && point1.y() - point3.y() < 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize3(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (point1.x() - point2.x() > 0 && point1.y() - point2.y() > 0 &&
            point1.x() - point3.x() > 0 && point1.y() - point3.y() < 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize4(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }

    return resizeFPoints;
}

FourPoints resizePointPositionByAlt(QPointF point1, QPointF point2, QPointF point3, QPointF point4,
                                    QPointF pos, int key, const QString &rationName, bool isShift)
{
    FourPoints resizeFPoints;
    resizeFPoints = initFourPoints(resizeFPoints);
    resizeFPoints[0] = point1;
    resizeFPoints[1] = point2;
    resizeFPoints[2] = point3;
    resizeFPoints[3] = point4;

    if (qFuzzyCompare(point1.x(), point2.x()) && point1.y() < point2.y()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize5ByAlt(point1, point2, point3, point4, pos, rationName, isShift);
            return resizeFPoints;
        }
        }
    }

    if (qFuzzyCompare(point1.x(), point2.x()) && point1.y() > point2.y()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize6ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }

    if (qFuzzyCompare(point1.y(), point2.y()) && point1.x() > point2.x()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize7ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (qFuzzyCompare(point1.y(), point2.y()) && point2.x() > point1.x()) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize8ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    //position first.
    if (point1.x() - point2.x() < 0 && point1.y() - point2.y() < 0 &&
            point1.x() - point3.x() < 0 && point1.y() - point3.y() > 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize1ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (point1.x() - point2.x() < 0 && point1.y() - point2.y() > 0 &&
            point1.x() - point3.x() > 0 && point1.y() - point3.y() > 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize2ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (point1.x() - point2.x() > 0 && point1.y() - point2.y() < 0 &&
            point1.x() - point3.x() < 0 && point1.y() - point3.y() < 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize3ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }
    if (point1.x() - point2.x() > 0 && point1.y() - point2.y() > 0 &&
            point1.x() - point3.x() > 0 && point1.y() - point3.y() < 0) {
        switch (key) {
        case 0: {
            resizeFPoints = point1Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 1: {
            resizeFPoints = point2Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 2: {
            resizeFPoints = point3Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 3: {
            resizeFPoints = point4Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 4: {
            resizeFPoints = point5Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 5: {
            resizeFPoints = point6Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 6: {
            resizeFPoints = point7Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        case 7: {
            resizeFPoints = point8Resize4ByAlt(point1, point2, point3, point4, pos, isShift);
            return resizeFPoints;
        }
        }
    }

    return resizeFPoints;
}

FourPoints resizeByAltNoTiltKeepRation(FourPoints mainPoints, QPointF pos, const QString &rationName)
{
    FourPoints fourPoints;
    fourPoints = initFourPoints(fourPoints);

    qreal width = getDistance(mainPoints[0], mainPoints[2]);
    qreal height = getDistance(mainPoints[0], mainPoints[1]);
    qreal ration = width / std::max(height, 0.01);

    QPointF centerPos = QPointF((mainPoints[0].x() + mainPoints[3].x()) / 2,
                                (mainPoints[0].y() + mainPoints[3].y()) / 2);

    qreal newHeight = std::max(std::min(std::abs(pos.y() - centerPos.y()),
                                        std::abs(pos.x() - centerPos.x())), qreal(10));
    qreal newWidth = newHeight * ration;

    //position 5
    if (qFuzzyCompare(mainPoints[0].x(), mainPoints[1].x())
            && mainPoints[0].y() < mainPoints[1].y()) {
        bool withRation = true;
        if (rationName == "1:1") {
            qDebug() << "1rationName:" << rationName;
            ration = 1;
        } else if (rationName == "4:3") {
            ration = qreal(4) / qreal(3);
        } else if (rationName == "8:5") {
            ration = qreal(8) / qreal(5);
        } else if (rationName == "16:9") {
            ration = qreal(16) / qreal(9);
        } else {
            withRation = false;
        }

        if (withRation)
            newWidth = ration * newHeight;

        fourPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        fourPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        fourPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        fourPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
    } else if (qFuzzyCompare(mainPoints[0].x(), mainPoints[1].x())
               && mainPoints[0].y() > mainPoints[1].y()) {
        fourPoints[0] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        fourPoints[1] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        fourPoints[2] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        fourPoints[3] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
    } else if (qFuzzyCompare(mainPoints[0].y(), mainPoints[1].y())
               && mainPoints[0].x() > mainPoints[1].x()) {
        fourPoints[0] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        fourPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        fourPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        fourPoints[3] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
    } else if (qFuzzyCompare(mainPoints[0].y(), mainPoints[1].y())
               && mainPoints[1].x() > mainPoints[0].x()) {
        fourPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        fourPoints[1] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        fourPoints[2] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        fourPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
    }

    return fourPoints;
}
/* first point1 */
/* point1 in the first position*/
FourPoints point1Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4)
        return newResizeFPoints;

    if (!isShift) {
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point1 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }
    return newResizeFPoints;
}

FourPoints point1Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4)
        return newResizeFPoints;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[0] = pos;
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(),
                                      centerPos.y() * 2 - pos.y());
        qreal b1 = newResizeFPoints[0].y() - k1 * newResizeFPoints[0].x();
        qreal b2 = newResizeFPoints[3].y() - k2 * newResizeFPoints[3].x();
        newResizeFPoints[1] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(point1, pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < point1.x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point1 in the second position */
FourPoints point1Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point3.y() + PADDING * 4 ||
            pos.x() > point2.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) <= -TANT_EDGEVALUE
            && (pos.y() - MIN_PADDING < point2.y() || pos.x() - MIN_PADDING < point3.x() ||
                pointLineDir(point3, point4, pos) == -1 || pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) > -TANT_EDGEVALUE
               && (pos.y() - MIN_PADDING < point3.y() || pos.x() + MIN_PADDING > point2.x() ||
                   pointLineDir(point3, point4, pos) == -1 || pointLineDir(point2, point4, pos) == -1))  {
        return newResizeFPoints;
    } else {
        if (!isShift) {
            if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == 1) {
                qreal distance = pointToLineDistance(point1, point2, pos);
                QPointF add = pointSplid(point2, point4, distance);
                point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                distance = pointToLineDistance(point1, point3, pos);
                add = pointSplid(point3, point4, distance);
                point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                point1 = pos;
            }
            if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == -1) {
                qreal distance = pointToLineDistance(point1, point2, pos);
                QPointF add = pointSplid(point2, point4, distance);
                point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                distance = pointToLineDistance(point1, point3, pos);
                add = pointSplid(point3, point4, distance);
                point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                point1 = pos;
            }
            if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == -1) {
                qreal distance = pointToLineDistance(point1, point2, pos);
                QPointF add = pointSplid(point2, point4, distance);
                point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                distance = pointToLineDistance(point1, point3, pos);
                add = pointSplid(point3, point4, distance);
                point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                point1 = pos;
            }
            if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == 1) {
                qreal distance = pointToLineDistance(point1, point2, pos);
                QPointF add = pointSplid(point2, point4, distance);
                point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                distance = pointToLineDistance(point1, point3, pos);
                add = pointSplid(point3, point4, distance);
                point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                point1 = pos;
            }
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
            return newResizeFPoints;
        } else {
            qreal distanceWidth = getDistance(point1, point3);
            qreal distanceHeight = getDistance(point1, point2);
            qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

            qreal distance1 = pointToLineDistance(point1, point2, pos);
            qreal distance2 = pointToLineDistance(point1, point3, pos);
            qreal distance = std::min(distance1, distance2);
            if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point1, point2, pos) == -1) {
                QPointF add = pointSplid(point2, point4, distance * ration);
                point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                add = pointSplid(point3, point4, distance);
                point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
            } else {
                QPointF add = pointSplid(point2, point4, distance * ration);
                point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                add = pointSplid(point3, point4, distance);
                point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
            }
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
            return newResizeFPoints;
        }
    }
    return newResizeFPoints;
}

FourPoints point1Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point3.y() + PADDING * 4 ||
            pos.x() > point2.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[0] = pos;
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
        qreal b1 = newResizeFPoints[0].y() - k1 * newResizeFPoints[0].x();
        qreal b2 = newResizeFPoints[3].y() - k2 * newResizeFPoints[3].x();
        newResizeFPoints[1] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(point1, pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < point1.x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point1 in the third position*/
FourPoints point1Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
            point1 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == 1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point1Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[0] = pos;
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
        qreal b1 = newResizeFPoints[0].y() - k1 * newResizeFPoints[0].x();
        qreal b2 = newResizeFPoints[3].y() - k2 * newResizeFPoints[3].x();
        newResizeFPoints[1] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(point1, pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p2 = (point1.y() - point2.y()) / (point1.x() - point2.x());
        if (tanp1p2 >=  -1) {
            if (pos.y() < point1.y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.y() < point1.y()) {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point1 in the fourth position */
FourPoints point1Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add  = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            distance  = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add  = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            distance  = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add  = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            distance  = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point1 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point1, point2, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add  = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            distance  = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point1 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point1, point2, pos) == 1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point1Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[0] = pos;
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
        qreal b1 = newResizeFPoints[0].y() - k1 * newResizeFPoints[0].x();
        qreal b2 = newResizeFPoints[3].y() - k2 * newResizeFPoints[3].x();
        newResizeFPoints[1] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(point1, pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.x() > point1.x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.x() > point1.x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point1 in the fifth position */
FourPoints point1Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        qDebug() << "1rationName:" << rationName;
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point3, pos) == -1
                && pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x() * ration, point2.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x() * ration, point2.y());
        }
        point1 = QPointF(point2.x(),  point2.y() - (point4.x() - point2.x()) / ration);
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = point4;

        return newResizeFPoints;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point3, pos) == -1
                && pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x() * ration, point2.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x() * ration, point2.y());
        }
        point1 = QPointF(point2.x(),  point2.y() - (point4.x() - point2.x()) / ration);
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point1Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.x() > point3.x() - PADDING * 6 ||
            pos.y() > point2.y() - PADDING * 6) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
    }

    return newResizeFPoints;
}

/* point1 in the sixth position */
FourPoints point1Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if ( pos.x() < point3.x()  + PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "tttt" << pointLineDir(point1, point3, pos)
                 << pointLineDir(point1, point2, pos);
        if (pointLineDir(point1, point3, pos) == 1
                && pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() + add.x(), point2.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() - add.x(), point2.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x(), point3.y() - add.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}
FourPoints point1Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if ( pos.x() < point3.x()  + PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);
    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        return newResizeFPoints;
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point1 in the seventh position */
FourPoints point1Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point3, pos) == 1
                && pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y());

        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point1Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/* point1 in the eighth position */
FourPoints point1Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4 || pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point1, point3, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "point1Resize8:" << pointLineDir(point1, point3, pos)
                 << pointLineDir(point1, point2, pos);
        if (pointLineDir(point1, point3, pos) == 1
                && pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y());

        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point1Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{

    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        if (std::abs(pos.x() - point4.x()) < MIN_PADDING * 10 ||
                std::abs(pos.y() - point4.y()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
            return newResizeFPoints;
        }

        newResizeFPoints[0] = pos;
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/***************** second point2 *******************/
/* point2 in the first position */
FourPoints point2Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point4.x() - PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4)
        return newResizeFPoints;

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE
            && (pos.x() + MIN_PADDING > point4.x() || pos.y() - MIN_PADDING < point1.y() ||
                pointLineDir(point1, point3, pos) == -1 || pointLineDir(point3, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE
               && (pos.x() - MIN_PADDING < point1.x() || pos.y() - MIN_PADDING < point4.y() ||
                   pointLineDir(point1, point3, pos) == -1 || pointLineDir(point3, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point1, point3, pos) < MIN_PADDING ||
                pointToLineDistance(point3, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    distance = pointToLineDistance(point2, point4, pos);
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                    point2 = pos;
                }
                if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    distance = pointToLineDistance(point2, point4, pos);
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                    point2 = pos;
                }
                if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    distance = pointToLineDistance(point2, point4, pos);
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                    point2 = pos;
                }
                if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    distance = pointToLineDistance(point2, point4, pos);
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                    point2 = pos;
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal width = getDistance(point1, point3);
                qreal height = getDistance(point1, point2);
                qreal ration = width / std::max(height, 0.01);

                qreal distance1 = pointToLineDistance(point1, point2, pos);
                qreal distance2 = pointToLineDistance(point2, point4, pos);
                qreal distance = std::min(distance1, distance2);

                if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
                    QPointF add = pointSplid(point1, point3, distance * ration);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                } else {
                    QPointF add = pointSplid(point1, point3, distance * ration);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                }
                point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point2Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point4.x() - PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4)
        return newResizeFPoints;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());

        qreal b2 = newResizeFPoints[1].y() - k2 * newResizeFPoints[1].x();
        qreal b1 = newResizeFPoints[2].y() - k1 * newResizeFPoints[2].x();
        newResizeFPoints[3] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
    } else {
        qreal distance = getDistance(point2, pos);
        QPointF add = pointSplid(centerPos, point2, distance);
        if (pos.x() < point2.x()) {
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            add = pointSplid(point1, centerPos, distance);
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
        } else {
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            add = pointSplid(point1, centerPos, distance);
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point2 in the second position */
FourPoints point2Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() < point4.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
            point2 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
        }
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point2Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() < point4.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());

        qreal b2 = newResizeFPoints[2].y() - k2 * newResizeFPoints[2].x();
        qreal b1 = newResizeFPoints[1].y() - k1 * newResizeFPoints[1].x();
        newResizeFPoints[0] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(point2, pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() > point2.x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point2 in the third position */
FourPoints point2Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() > point4.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
            point2 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
        }
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point2Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());

        qreal b1 = newResizeFPoints[1].y() - k1 * newResizeFPoints[1].x();
        qreal b2 = newResizeFPoints[2].y() - k2 * newResizeFPoints[2].x();
        newResizeFPoints[0] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(point2, pos);
        QPointF add = pointSplid(centerPos, point2, distance);
        qreal tanp1p2 = (point1.y() - point2.y()) / (point1.x() - point2.x());
        if (tanp1p2 >=  -1) {
            if (pos.x() < point2.x()) {
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            } else {
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            }
        } else {
            if (pos.x() < point2.x()) {
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            } else {
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point2 in the fourth position */
FourPoints point2Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() > point4.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
            point2 = pos;
        }
        if (pointLineDir(point1, point2, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point1, point2, pos);
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            distance = pointToLineDistance(point2, point4, pos);
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
            point2 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point2, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
        }
        point2 = QPointF(point1.x() + point4.x() - point3.x(),
                         point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point2Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() > point4.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());

        qreal b2 = newResizeFPoints[1].y() - k2 * newResizeFPoints[1].x();
        qreal b1 = newResizeFPoints[2].y() - k1 * newResizeFPoints[2].x();
        newResizeFPoints[3] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
    } else {
        qreal distance = getDistance(point2, pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.y() < point2.y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.y() < point2.y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point2 in the fifth position*/
FourPoints point2Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point4.x() - PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point2, pos) == -1
                && pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x() * ration, point1.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x() * ration, point1.y());
        }

        point2 = QPointF(point1.x(), (point3.x() - point1.x()) / ration + point1.y());
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point2, pos) == -1
                && pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - add.x() * ration, point1.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + add.x() * ration, point1.y());
        }

        point2 = QPointF(point1.x(), (point3.x() - point1.x()) / ration + point1.y());
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point2Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        if (std::abs(pos.x() - point3.x()) < MIN_PADDING * 10 ||
                std::abs(pos.y() - point3.y()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
        }

        newResizeFPoints[0] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
    }

    return newResizeFPoints;
}

/* point2 in the sixth position */
FourPoints point2Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point3.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "point2Resize6^^^^" << pointLineDir(point1, point2, pos)
                 << pointLineDir(point2, point4, pos);

        if (pointLineDir(point1, point2, pos) == -1
                && pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() - add.x(), point1.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() + add.x(), point1.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }

        point2 = QPointF(point1.x() + point4.x() - point3.x(),
                         point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point2Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point3.x() + PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point2 in the seventh position */
FourPoints point2Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "*point2Resize7:" << pointLineDir(point1, point2, pos)
                 << pointLineDir(point2, point4, pos);
        if (pointLineDir(point1, point2, pos) == 1
                && pointLineDir(point2, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y());
        }

        point2 = QPointF(point1.x() + point4.x() - point3.x(),
                         point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point2Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/* point2 in the eighth position */
FourPoints point2Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point2, pos);
        qreal distance2 = pointToLineDistance(point2, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "*point2Resize8:" << pointLineDir(point1, point2, pos)
                 << pointLineDir(point2, point4, pos);
        if (pointLineDir(point1, point2, pos) == -1
                && pointLineDir(point2, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y());
        }

        point2 = QPointF(point1.x() + point4.x() - point3.x(),
                         point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point2Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        if (std::abs(pos.x() - point3.x()) < MIN_PADDING * 10 ||
                std::abs(pos.y() - point3.y()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
            return newResizeFPoints;
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - point1.x(), pos.y());
        newResizeFPoints[1] = pos;
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/***************** third point3 *******************/
/* point3 in the first position */
FourPoints point3Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() > point4.y() - PADDING * 4)
        return newResizeFPoints;

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE &&
            (pos.x() - MIN_PADDING < point1.x() || pos.y() + MIN_PADDING > point4.y() ||
             pointLineDir(point1, point2, pos) == 1 || pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE &&
               (pos.x() + MIN_PADDING > point4.x() || pos.y() + MIN_PADDING > point1.y() ||
                pointLineDir(point1, point2, pos) == 1 || pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point4, pos) < MIN_PADDING ||
                pointToLineDistance(point1, point2, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                    distance = pointToLineDistance(point1, point3, pos);
                    add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    point3 = pos;
                }
                if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                    distance = pointToLineDistance(point1, point3, pos);
                    add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    point3 = pos;
                }
                if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                    distance = pointToLineDistance(point1, point3, pos);
                    add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    point3 = pos;
                }
                if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                    distance = pointToLineDistance(point1, point3, pos);
                    add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    point3 = pos;
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal width = getDistance(point1, point3);
                qreal height = getDistance(point1, point2);
                qreal ration = width / std::max(height, 0.01);

                qreal distance1 = pointToLineDistance(point1, point3, pos);
                qreal distance2 = pointToLineDistance(point3, point4, pos);
                qreal distance = std::min(distance1, distance2);
                if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                    point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                    point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                    point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point3Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() > point4.y() - PADDING * 4)
        return newResizeFPoints;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[2] = pos;
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());

        qreal b1 = newResizeFPoints[2].y() - k1 * newResizeFPoints[2].x();
        qreal b2 = newResizeFPoints[1].y() - k2 * newResizeFPoints[1].x();
        newResizeFPoints[3] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
    } else {
        qreal distance = getDistance(point3, pos);
        QPointF add = pointSplid(centerPos, point3, distance);
        if (pos.x() > point3.x()) {
            newResizeFPoints[2] = QPointF(point3.x() + add.x(), point3.y() - add.y());
            add = pointSplid(point4, centerPos, distance);
            newResizeFPoints[3] = QPointF(point4.x() + add.x(), point4.y() + add.y());
        } else {
            newResizeFPoints[2] = QPointF(point3.x() - add.x(), point3.y() + add.y());
            add = pointSplid(point4, centerPos, distance);
            newResizeFPoints[3] = QPointF(point4.x() - add.x(), point4.y() - add.y());
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    }

    return newResizeFPoints;
}

/* point3 in the second position */
FourPoints point3Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point4.x() - PADDING * 4 ||
            pos.y() > point1.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            point3 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point3Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[2] = pos;
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(),
                                      centerPos.y() * 2 - pos.y());

        qreal b1 = newResizeFPoints[2].y() - k1 * newResizeFPoints[2].x();
        qreal b2 = newResizeFPoints[1].y() - k2 * newResizeFPoints[1].x();
        newResizeFPoints[3] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
    } else {
        qreal distance = getDistance(point3, pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < point3.x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point3 in the third position */
FourPoints point3Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point4.x() + PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            point3 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point3Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point4.x() + PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[2] = pos;
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());

        qreal b1 = newResizeFPoints[1].y() - k1 * newResizeFPoints[1].x();
        qreal b2 = newResizeFPoints[2].y() - k2 * newResizeFPoints[2].x();
        newResizeFPoints[0] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(point3, pos);
        QPointF add = pointSplid(centerPos, point3, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 >= 1) {
            if (pos.x() > point3.x()) {
                newResizeFPoints[2] = QPointF(point3.x() + add.x(), point3.y() + add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            } else {
                newResizeFPoints[2] = QPointF(point3.x() - add.x(), point3.y() - add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            }
        } else {
            if (pos.x() > point3.x()) {
                newResizeFPoints[2] = QPointF(point3.x() + add.x(), point3.y() - add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            } else {
                newResizeFPoints[2] = QPointF(point3.x() - add.x(), point3.y() + add.y());
                QPointF add = pointSplid(centerPos, point1, distance);
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            }
        }
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point3 in the fourth position */
FourPoints point3Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point4.x() + PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add  = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == 1 && pointLineDir(point3, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add  = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add  = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            point3 = pos;
        }
        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point3, point4, pos);
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
            distance = pointToLineDistance(point1, point3, pos);
            add  = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            point3 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point3, pos) == -1 && pointLineDir(point3, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point3Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[2] = pos;
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());

        qreal b1 = newResizeFPoints[1].y() - k1 * newResizeFPoints[1].x();
        qreal b2 = newResizeFPoints[2].y() - k2 * newResizeFPoints[2].x();
        newResizeFPoints[0] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(point3, pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.y() > point3.y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.y() > point3.y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point3 in the fifth position */
FourPoints point3Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() > point4.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point3, pos) == -1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x() * ration, point4.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x() * ration, point4.y());
        }
        point3 = QPointF(point4.x(), point4.y() - (point4.x() - point2.x()) / ration);
        point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;

        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point3, pos) == -1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point2, point4, distance);
            point4 = QPointF(point4.x() + add.x() * ration, point4.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x() * ration, point4.y());
        }
        point3 = QPointF(point4.x(), point4.y() - (point4.x() - point2.x()) / ration);
        point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

/* point3 in the fifth position */
FourPoints point3Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        if (std::abs(pos.x() - point2.x()) < MIN_PADDING * 10 ||
                std::abs(pos.y() - point2.y()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
    }

    return newResizeFPoints;
}
/* point3 in the sixth position */
FourPoints point3Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
        return newResizeFPoints;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point1, point3, pos) == -1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() - add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() + add.x(), point4.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x(), point4.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(),
                         point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point3Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        return newResizeFPoints;
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point3 in the seventh position */
FourPoints point3Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point4.x() +  PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "point3Resize7:" << pointLineDir(point1, point3, pos) << pointLineDir(point3, point4, pos);
        if (pointLineDir(point1, point3, pos) == 1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(),
                         point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point3Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point4.x() +  PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(),  centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point3 in the eighth position */
FourPoints point3Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point1, point3, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "point3Resize8:" << pointLineDir(point1, point3, pos) << pointLineDir(point3, point4, pos);
        if (pointLineDir(point1, point3, pos) == 1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(),
                         point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point3Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = pos;
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/***************** fourth point4 *******************/
/* point4 in the first position */
FourPoints point4Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }
    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE &&
            (pos.x() - MIN_PADDING < point2.x() || pos.y() - MIN_PADDING < point3.y() ||
             pointLineDir(point1, point2, pos) == 1 || pointLineDir(point1, point3, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE &&
               (pos.x() - MIN_PADDING < point3.x() || pos.y() + MIN_PADDING > point2.y() ||
                pointLineDir(point1, point2, pos) == 1 || pointLineDir(point1, point3, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point1, point2, pos) < MIN_PADDING ||
                pointToLineDistance(point1, point3, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                    point4 = pos;
                }
                if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                    point4 = pos;
                }
                if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                    point4 = pos;
                }
                if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                    point4 = pos;
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal width = getDistance(point1, point3);
                qreal height = getDistance(point1, point2);
                qreal ration = width / std::max(height, 0.01);

                qreal distance1 = pointToLineDistance(point2, point4, pos);
                qreal distance2 = pointToLineDistance(point3, point4, pos);
                qreal distance =  std::min(distance1, distance2);
                if (pointLineDir(point2, point4, pos) == 1 && pointLineDir(point3, point4, pos) == -1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                }
                point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point4Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[3] = pos;
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(),
                                      centerPos.y() * 2 - pos.y());

        qreal b1 = newResizeFPoints[0].y() - k1 * newResizeFPoints[0].x();
        qreal b2 = newResizeFPoints[3].y() - k2 * newResizeFPoints[3].x();
        newResizeFPoints[1] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(point4, pos);
        QPointF add = pointSplid(centerPos, point4, distance);
        if (pos.x() > point4.x()) {
            newResizeFPoints[3] = QPointF(point4.x() + add.x(), point4.y() + add.y());
            add = pointSplid(centerPos, point3, distance);
            newResizeFPoints[2] = QPointF(point3.x() + add.x(), point3.y() - add.y());
        } else {
            newResizeFPoints[3] = QPointF(point4.x() - add.x(), point4.y() - add.y());
            add = pointSplid(centerPos, point3, distance);
            newResizeFPoints[2] = QPointF(point3.x() - add.x(), point3.y() + add.y());
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    }

    return newResizeFPoints;
}

/* point4 in the second position */
FourPoints point4Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point3.x() + PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point4 = pos;
        }
        if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point4 = pos;
        }
        if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
            point4 = pos;
        }
        if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
            point4 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point2, point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point2, point4, pos) == -1 && pointLineDir(point3, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    return newResizeFPoints;
}

FourPoints point4Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point3.x() + PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[3] = pos;
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());

        qreal b1 = newResizeFPoints[0].y() - k1 * newResizeFPoints[0].x();
        qreal b2 = newResizeFPoints[3].y() - k2 * newResizeFPoints[3].x();
        newResizeFPoints[1] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(point4, pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() > point4.x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point4 in the third position */
FourPoints point4Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT2_EDGEVALUE &&
            (pos.y() - MIN_PADDING < point2.y() || pos.x() + MIN_PADDING > point3.x() ||
             pointLineDir(point1, point2, pos) == -1 || pointLineDir(point1, point3, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT2_EDGEVALUE &&
               (pos.y() - MIN_PADDING < point3.y() || pos.x() - MIN_PADDING < point2.x() ||
                pointLineDir(point1, point2, pos) == -1 || pointLineDir(point1, point3, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point1, point2, pos) < MIN_PADDING ||
                pointToLineDistance(point1, point3, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                    point4 = pos;
                }
                if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                    point4 = pos;
                }
                if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                    point4 = pos;
                }
                if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    distance = pointToLineDistance(point3, point4, pos);
                    add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                    point4 = pos;
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance1 = pointToLineDistance(point2, point4, pos);
                qreal distance2 = pointToLineDistance(point3, point4, pos);
                qreal distance = std::min(distance1, distance2);
                if (pointLineDir(point2, point4, pos) == 1 && pointLineDir(point3, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                }
                point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point4Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[3] = pos;
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());

        qreal b1 = newResizeFPoints[3].y() - k1 * newResizeFPoints[3].x();
        qreal b2 = newResizeFPoints[0].y() - k2 * newResizeFPoints[0].x();
        newResizeFPoints[2] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    } else {
        qreal distance = getDistance(point4, pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp3p4 = (point3.y() - point4.y()) / (point3.x() - point4.x());
        if (tanp3p4 <=  -1) {
            if (pos.y() > point4.y()) {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            }
        } else {
            if (pos.y() > point4.y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point4 in the third position */
FourPoints point4Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
            point4 = pos;
        }
        if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == -1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
            point4 = pos;
        }
        if (pointLineDir(point3, point4, pos) == -1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
            point4 = pos;
        }
        if (pointLineDir(point3, point4, pos) == 1 && pointLineDir(point2, point4, pos) == 1) {
            qreal distance = pointToLineDistance(point2, point4, pos);
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
            distance = pointToLineDistance(point3, point4, pos);
            add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
            point4 = pos;
        }
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal distanceWidth = getDistance(point1, point3);
        qreal distanceHeight = getDistance(point1, point2);
        qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

        qreal distance1 = pointToLineDistance(point2,  point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);
        if (pointLineDir(point2, point4, pos) == -1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(),
                         point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point4Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4 ||
            pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        qreal k1 = (point2.y() - point1.y()) / (point2.x() - point1.x());
        qreal k2 = -1 / k1;

        newResizeFPoints[3] = pos;
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());

        qreal b2 = newResizeFPoints[0].y() - k2 * newResizeFPoints[0].x();
        qreal b1 = newResizeFPoints[3].y() - k1 * newResizeFPoints[3].x();
        newResizeFPoints[2] = QPointF((b1 - b2) / (k2 - k1), (b1 - b2) / (k2 - k1) * k1 + b1);
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    } else {
        qreal distance = getDistance(point4, pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.x() < point4.x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.x() < point4.x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }

    return newResizeFPoints;
}

/* point4 in the fifth position */
FourPoints point4Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4 ||
            pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance1 = pointToLineDistance(point2, point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point2, point4, pos) == 1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + add.x() * ration, point3.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - add.x() * ration, point3.y());
        }
        point4 = QPointF(point3.x(), point3.y() + (point3.x() - point1.x()) / ration);
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = pos;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point2, point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point2, point4, pos) == 1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() + add.x(), point3.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(),
                         point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point4Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        if (std::abs(pos.x() - point1.x()) < MIN_PADDING * 10 ||
                std::abs(pos.y() - point1.y()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = pos;
    }

    return newResizeFPoints;
}

/* point4 in the sixth position */
FourPoints point4Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = pos;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point2, point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        if (pointLineDir(point2, point4, pos) == 1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() + add.x(), point3.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(),
                         point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point4Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4 ||
            pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);
    if (!isShift) {
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(),
                                      centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[2] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = pos;
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point4 in the seventh position */
FourPoints point4Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = pos;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point2, point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "point4Resize7:" << pointLineDir(point2, point4, pos)
                 << pointLineDir(point3, point4, pos);
        if (pointLineDir(point2, point4, pos) == 1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() - add.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(),
                         point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point4Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() - PADDING * 4 ||
            pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[3] = pos;
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point4 in the eighth position */
FourPoints point4Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = pos;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance1 = pointToLineDistance(point2, point4, pos);
        qreal distance2 = pointToLineDistance(point3, point4, pos);
        qreal distance = std::min(distance1, distance2);

        qDebug() << "point4Resize8:" << pointLineDir(point2, point4, pos)
                 << pointLineDir(point3, point4, pos);
        if (pointLineDir(point2, point4, pos) == -1
                && pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() - add.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(),
                         point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point4Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.x() < point1.x() + PADDING * 4 ||
            pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }
    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(pos.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), pos.y());
        newResizeFPoints[3] = pos;
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/********************** fifth point5 ************************/
/* point5 in the first position */
FourPoints point5Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point7 = QPointF((point3.x() + point4.x()) / 2, (point3.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE &&
            (pos.x() + MIN_PADDING > point7.x()  || pointLineDir(point3, point4, pos) == -1 ||
             pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE &&
               (pos.y() - MIN_PADDING < point7.y() || pointLineDir(point3, point4, pos) == -1 ||
                pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point3, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point2, pos) == 1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal width = getDistance(point1, point3);
                qreal height = getDistance(point1, point2);
                qreal ration = width / std::max(height, 0.01);

                qreal distance = pointToLineDistance(point1,  point2, pos);
                if (pointLineDir(point1, point2, pos) == -1) {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                } else {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                }
                point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point5Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[0], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.x() < anotherFPoints[0].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(),
                                          point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() - add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[0], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < point1.x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point5 in the second position */
FourPoints point5Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point7 = QPointF((point3.x() + point4.x()) / 2, (point3.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) <= -TANT_EDGEVALUE &&
            (pos.x() - MIN_PADDING < point7.x()  || pointLineDir(point3, point4, pos) == -1 ||
             pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) > -TANT_EDGEVALUE &&
               (pos.y() - MIN_PADDING < point7.y() || pointLineDir(point3, point4, pos) == -1 ||
                pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point3, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point2, pos) == 1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point1,  point2, pos);
                if (pointLineDir(point1, point2, pos) == -1) {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                } else {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                }
                point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point5Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[0], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.x() > anotherFPoints[0].x()) {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(),
                                          point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() - add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[0], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() > anotherFPoints[0].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point5 in the  third position */
FourPoints point5Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point7 = QPointF((point3.x() + point4.x()) / 2, (point3.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT2_EDGEVALUE &&
            (pos.y() + MIN_PADDING > point7.y()  || pointLineDir(point3, point4, pos) == 1 ||
             pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x()))  < TANT2_EDGEVALUE &&
               (pos.x() + MIN_PADDING > point7.x() || pointLineDir(point3, point4, pos) == 1 ||
                pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point3, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point2, pos) == 1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point1,  point2, pos);
                if (pointLineDir(point1, point2, pos) == 1) {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                } else {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                }
                point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point5Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[0], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.x() < anotherFPoints[0].x()) {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() - add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() - add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() + add.y());
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() + add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[0], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp3p4 = (point3.y() - point4.y()) / (point3.x() - point4.x());
        if (tanp3p4 <=  -1) {
            if (pos.x() < anotherFPoints[0].x()) {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            }
        } else {
            if (pos.x() < anotherFPoints[0].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}


/* point5 in the  fourth position */
FourPoints point5Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point7 = QPointF((point3.x() + point4.x()) / 2, (point3.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= -TANT2_EDGEVALUE &&
            (pos.x() - MIN_PADDING < point7.x()  || pointLineDir(point3, point4, pos) == 1 ||
             pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x()))  < -TANT2_EDGEVALUE &&
               (pos.y() + MIN_PADDING > point7.y() || pointLineDir(point3, point4, pos) == 1 ||
                pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point3, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point2, pos) == -1) {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point2, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point1,  point2, pos);
                if (pointLineDir(point1, point2, pos) == 1) {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                } else {
                    QPointF add = pointSplid(point2, point4, distance * ration);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                }
                point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }

    return newResizeFPoints;
}

FourPoints point5Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[0], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.y() < anotherFPoints[0].y()) {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() - add.y());
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() - add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() + add.y());
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() + add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[0], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.y() < anotherFPoints[0].y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.y() < anotherFPoints[0].y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}


/* point5 in the fifth position */
FourPoints point5Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance = pointToLineDistance(point1,  point2, pos);
        if (pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x() * ration, point2.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x() * ration, point2.y());
        }
        point1 = QPointF(point2.x(), point2.y() - (point4.x() - point2.x()) / ration);
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point2, pos);
        if (pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() - add.x() * ration, point2.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance);
            point2 = QPointF(point2.x() + add.x() * ration, point2.y());
        }
        point1 = QPointF(point2.x(), point2.y() - (point4.x() - point2.x()) / ration);
        point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point5Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point3.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(centerPos.y() * 2 - pos.x(), point4.y());
    }

    return newResizeFPoints;
}

/* point5 in the sixth position */
FourPoints point5Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.x() < point3.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point2, pos);
        if (pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() - add.x(), point2.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x(), point3.y() - add.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x() + add.x(), point2.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x(), point3.y() + add.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point5Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.x() < point3.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), point4.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/* point5 in the seventh position */
FourPoints point5Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point2, pos);
        qDebug() << "point5Resize7:" << pointLineDir(point1, point2, pos);

        if (pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point5Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point3.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(point4.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point5 in the eighth position */
FourPoints point5Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point2, pos);
        qDebug() << "point5Resize8:" << pointLineDir(point1, point2, pos);

        if (pointLineDir(point1, point2, pos) == -1) {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() + add.x(), point3.y());
        } else {
            QPointF add = pointSplid(point2, point4, distance * ration);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point1 = QPointF(point2.x() + point3.x() - point4.x(),
                         point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point5Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point3.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(point4.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/********************** sixth point6 ************************/
/* point6 in the first position */
FourPoints point6Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point8 = QPointF((point2.x() + point4.x()) / 2, (point2.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE &&
            (pos.y() + MIN_PADDING > point8.x()  || pointLineDir(point3, point4, pos) == -1 ||
             pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE &&
               (pos.x() + MIN_PADDING > point8.x() || pointLineDir(point3, point4, pos) == -1 ||
                pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pos.x()  >= (point2.x() + point4.x()) / 2 || pos.y() >= (point2.y() + point4.y()) / 2) {
                    return newResizeFPoints;
                } else {
                    if (pointLineDir(point1, point3, pos) == 1) {
                        qreal distance = pointToLineDistance(point1, point3, pos);
                        QPointF add = pointSplid(point1, point2, distance);
                        point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                        add = pointSplid(point3, point4, distance);
                        point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                    } else {
                        qreal distance = pointToLineDistance(point1, point3, pos);
                        QPointF add = pointSplid(point1, point2, distance);
                        point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                        add = pointSplid(point3, point4, distance);
                        point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                    }
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal width = getDistance(point1, point3);
                qreal height = getDistance(point1, point2);
                qreal ration = width / std::max(height, 0.01);

                qreal distance = pointToLineDistance(point1,  point3, pos);
                if (pointLineDir(point1, point3, pos) == 1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                }
                point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point6Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[1], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() < anotherFPoints[1].x()) {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() - add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() - add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() + add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() + add.y());
        }
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[1], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < anotherFPoints[1].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point6 in the second position */
FourPoints point6Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point8 = QPointF((point2.x() + point4.x()) / 2, (point2.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) <= -TANT_EDGEVALUE &&
            (pos.y() - MIN_PADDING < point8.y()  || pointLineDir(point3, point4, pos) == -1 ||
             pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) > -TANT_EDGEVALUE &&
               (pos.x() + MIN_PADDING > point8.x() || pointLineDir(point3, point4, pos) == -1 ||
                pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point3, pos) == 1) {
                    qreal distance = pointToLineDistance(point1, point3, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point3, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point1,  point3, pos);
                if (pointLineDir(point1, point3, pos) == -1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                }
                point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point6Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[1], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() < anotherFPoints[1].x()) {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() + add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() - add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() - add.y());
        }
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[1], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < anotherFPoints[1].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point6 in the third position */
FourPoints point6Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point8 = QPointF((point2.x() + point4.x()) / 2, (point2.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT2_EDGEVALUE &&
            (pos.x() - MIN_PADDING < point8.x()  || pointLineDir(point3, point4, pos) == 1 ||
             pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT2_EDGEVALUE &&
               (pos.y() + MIN_PADDING > point8.y() || pointLineDir(point3, point4, pos) == 1 ||
                pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point3, pos) == -1) {
                    qreal distance = pointToLineDistance(point1, point3, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point3, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point1,  point3, pos);
                if (pointLineDir(point1, point3, pos) == 1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                }
                point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point6Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[1], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() > anotherFPoints[1].x()) {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() - add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() - add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() + add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() + add.y());
        }
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[1], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp3p4 = (point3.y() - point4.y()) / (point3.x() - point4.x());
        if (tanp3p4 <=  -1) {
            if (pos.x() > anotherFPoints[1].x()) {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            }
        } else {
            if (pos.x() > anotherFPoints[1].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point6 in the fourth position */
FourPoints point6Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point8 = QPointF((point2.x() + point4.x()) / 2, (point2.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= -TANT2_EDGEVALUE &&
            (pos.y() - MIN_PADDING < point8.y()  || pointLineDir(point3, point4, pos) == 1 ||
             pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < -TANT2_EDGEVALUE &&
               (pos.x() - MIN_PADDING < point8.x() || pointLineDir(point3, point4, pos) == 1 ||
                pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point4, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point1, point3, pos) == -1) {
                    qreal distance = pointToLineDistance(point1, point3, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point1, point3, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point1,  point3, pos);
                if (pointLineDir(point1, point3, pos) == -1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point2, point4, distance * ration);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                }
                point3 = QPointF(point1.x() + point4.x() - point2.x(), point1.y() + point4.y() - point2.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point6Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[1], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() > anotherFPoints[1].x()) {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() + add.x(),
                                          newResizeFPoints[0].y() + add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(newResizeFPoints[0].x() - add.x(),
                                          newResizeFPoints[0].y() - add.y());
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() - add.y());
        }
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    } else {
        qreal distance = getDistance(anotherFPoints[1], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.x() > anotherFPoints[1].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.x() > anotherFPoints[1].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point6 in the fifth position */
FourPoints point6Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point4.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance = pointToLineDistance(point1,  point3, pos);
        if (pointLineDir(point1, point3, pos) == -1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() - add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() + add.y());
        }
        point3 = QPointF(point1.x() + (point2.y() - point1.y()) * ration, point1.y());
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point3, pos);
        if (pointLineDir(point1, point3, pos) == -1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() - add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() + add.y());
        }
        point3 = QPointF(point1.x() + (point2.y() - point1.y()) * ration, point1.y());
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point6Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point4.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = QPointF(point3.x(), centerPos.y() * 2 - pos.y());
    }

    return newResizeFPoints;
}
/* point6 in the sixth position */
FourPoints point6Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point3, pos);
        if (pointLineDir(point1, point3, pos) == -1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() - add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() + add.x(), point4.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x(), point1.y() + add.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x() - add.x(), point4.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(),
                         point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point6Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point2.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);
    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = QPointF(point3.x(), centerPos.y() * 2 - pos.y());
        return newResizeFPoints;
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/* point6 in the seventh position */
FourPoints point6Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;
    if (pos.x() < point2.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point3, pos);
        qDebug() << "point6Resize7:" << pointLineDir(point1, point3, pos);
        if (pointLineDir(point1, point3, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(),
                         point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point6Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point2.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);
    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), point1.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2  - pos.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point6 in the eighth position */
FourPoints point6Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = point4;
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point1,  point3, pos);
        qDebug() << "point6Resize8:" << pointLineDir(point1, point3, pos);
        if (pointLineDir(point1, point3, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - add.x(), point1.y());
            add = pointSplid(point2, point4, distance * ration);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }
        point3 = QPointF(point1.x() + point4.x() - point2.x(),
                         point1.y() + point4.y() - point2.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point6Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), point1.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - pos.x(), point3.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/********************** seventh point7 ************************/
/* point7 in the first position */
FourPoints point7Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point5 = QPointF((point2.x() + point1.x()) / 2, (point2.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE &&
            (pos.x() - MIN_PADDING < point5.x()  || pointLineDir(point1, point2, pos) == 1 ||
             pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE &&
               (pos.y() + MIN_PADDING > point5.y() || pointLineDir(point1, point2, pos) == 1 ||
                pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point1, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pos.x()  < (point1.x() + point2.x()) / 2 || pos.y() > (point1.y() + point2.y()) / 2) {
                    return newResizeFPoints;
                } else {
                    if (pointLineDir(point3, point4, pos) == -1) {
                        qreal distance = pointToLineDistance(point3, point4, pos);
                        QPointF add = pointSplid(point1, point3, distance);
                        point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                        add = pointSplid(point2, point4, distance);
                        point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                    } else {
                        qreal distance = pointToLineDistance(point3, point4, pos);
                        QPointF add = pointSplid(point1, point3, distance);
                        point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                        add = pointSplid(point2, point4, distance);
                        point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                    }
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal width = getDistance(point1, point3);
                qreal height = getDistance(point1, point2);
                qreal ration = width / std::max(height, 0.01);

                qreal distance = pointToLineDistance(point3,  point4, pos);
                if (pointLineDir(point3, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                }
                point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point7Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[2], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.x() > anotherFPoints[2].x()) {
            newResizeFPoints[2] = QPointF(point3.x() + add.x(), point3.y() - add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[3] = QPointF(point4.x() + add.x(), point4.y() - add.y());
        } else {
            newResizeFPoints[2] = QPointF(point3.x() - add.x(), point3.y() + add.y());
            add = pointSplid(point2, point4, distance);
            newResizeFPoints[3] = QPointF(point4.x() - add.x(), point4.y() + add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    } else {
        qreal distance = getDistance(anotherFPoints[2], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() > anotherFPoints[2].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point7 in the second position */
FourPoints point7Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point5 = QPointF((point2.x() + point1.x()) / 2, (point2.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) <= -TANT_EDGEVALUE &&
            (pos.x() + MIN_PADDING > point5.x()  || pointLineDir(point1, point2, pos) == 1 ||
             pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) > -TANT_EDGEVALUE &&
               (pos.y() + MIN_PADDING > point5.y() || pointLineDir(point1, point2, pos) == 1 ||
                pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point1, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point3, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point3,  point4, pos);
                if (pointLineDir(point3, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                }
                point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point7Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[2], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.x() < anotherFPoints[2].x()) {
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() - add.y());
        } else {
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() + add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    } else {
        qreal distance = getDistance(anotherFPoints[2], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() < anotherFPoints[2].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point7 in the third position */
FourPoints point7Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point5 = QPointF((point2.x() + point1.x()) / 2, (point2.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT2_EDGEVALUE &&
            (pos.y() - MIN_PADDING < point5.y() || pointLineDir(point1, point2, pos) == -1 ||
             pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT2_EDGEVALUE &&
               (pos.x() - MIN_PADDING < point5.x() || pointLineDir(point1, point2, pos) == -1 ||
                pointLineDir(point2, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point1, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point3, point4, pos) == 1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                } else {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point3,  point4, pos);
                if (pointLineDir(point3, point4, pos) == -1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() - add.x(), point3.y() - add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() + add.x(), point3.y() + add.y());
                }
                point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point7Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[2], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.x() > anotherFPoints[2].x()) {
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() + add.y());
        } else {
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() - add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    } else {
        qreal distance = getDistance(anotherFPoints[2], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp3p4 = (point3.y() - point4.y()) / (point3.x() - point4.x());
        if (tanp3p4 <=  -1) {
            if (pos.x() > anotherFPoints[2].x()) {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            }
        } else {
            if (pos.x() > anotherFPoints[2].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point7 in the fourth position */
FourPoints point7Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point5 = QPointF((point2.x() + point4.x()) / 2, (point2.y() + point4.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= -TANT2_EDGEVALUE &&
            (pos.x() + MIN_PADDING > point5.x()  || pointLineDir(point1, point2, pos) == -1 ||
             pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < -TANT2_EDGEVALUE &&
               (pos.y() - MIN_PADDING < point5.y() || pointLineDir(point1, point2, pos) == -1 ||
                pointLineDir(point2, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point2, point1, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point3, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                    add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point3, point4, pos);
                    QPointF add = pointSplid(point1, point3, distance);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                    add = pointSplid(point2, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point3,  point4, pos);
                if (pointLineDir(point3, point4, pos) == -1) {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() + add.x(), point3.y() - add.y());
                } else {
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    add = pointSplid(point1, point3, distance * ration);
                    point3 = QPointF(point3.x() - add.x(), point3.y() + add.y());
                }
                point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point7Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[2], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point3, distance);
        if (pos.y() > anotherFPoints[2].y()) {
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() - add.x(),
                                          newResizeFPoints[2].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() + add.y());
        } else {
            newResizeFPoints[2] = QPointF(newResizeFPoints[2].x() + add.x(),
                                          newResizeFPoints[2].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() - add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - newResizeFPoints[2].x(),
                                      centerPos.y() * 2 - newResizeFPoints[2].y());
    } else {
        qreal distance = getDistance(anotherFPoints[2], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.y() > anotherFPoints[2].y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.x() < anotherFPoints[2].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point7 in the fifth position */
FourPoints point7Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance = pointToLineDistance(point3,  point4, pos);
        if (pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + add.x() * ration, point3.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - add.x() * ration, point3.y());
        }
        point4 = QPointF(point3.x(), point3.y() + (point3.x() - point1.x()) / ration);
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point3,  point4, pos);
        if (pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() + add.x(), point3.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point7Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    }

    return newResizeFPoints;
}

/* point7 in the sixth position */
FourPoints point7Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point3,  point4, pos);
        if (pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() + add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() + add.x(), point3.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x(), point2.y() - add.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x() - add.x(), point3.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point7Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point2.x() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);
    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), pos.y());
        newResizeFPoints[1] = QPointF(point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = QPointF(point3.x(), centerPos.y() * 2 - pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/* point7 in the seventh position */
FourPoints point7Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point3,  point4, pos);
        if (pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() - add.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point7Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(centerPos.x() * 2 - point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(point1.x(), pos.y());
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point7 in the eighth position */
FourPoints point7Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point3,  point4, pos);
        qDebug() << "point7Resize8:" << pointLineDir(point3, point4, pos);
        if (pointLineDir(point3, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + add.x(), point2.y());
            add = pointSplid(point1, point3, distance * ration);
            point3 = QPointF(point3.x(), point3.y() - add.y());
        }
        point4 = QPointF(point2.x() + point3.x() - point1.x(), point2.y() + point3.y() - point1.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point7Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point2.y() - PADDING * 4) {
        return newResizeFPoints;
    }
    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(point2.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), pos.y());
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/********************** eighth point8 ************************/
/* point8 in the first position */
FourPoints point8Resize1(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point6 = QPointF((point3.x() + point1.x()) / 2, (point3.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT_EDGEVALUE &&
            (pos.y() - MIN_PADDING < point6.y()  || pointLineDir(point1, point3, pos) == -1 ||
             pointLineDir(point3, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT_EDGEVALUE &&
               (pos.x() - MIN_PADDING < point6.x() || pointLineDir(point1, point3, pos) == -1 ||
                pointLineDir(point3, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point1, point3, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (pointToLineDistance(point1, point3, pos) < MIN_PADDING) {
                return newResizeFPoints;
            } else {
                if (!isShift) {
                    if (pos.y()  <= (point1.y() + point3.y()) / 2) {
                        return newResizeFPoints;
                    } else {
                        if (pointLineDir(point2, point4, pos) == 1) {
                            qreal distance = pointToLineDistance(point2, point4, pos);
                            QPointF add = pointSplid(point1, point2, distance);
                            point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                            add = pointSplid(point3, point4, distance);
                            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                        } else {
                            qreal distance = pointToLineDistance(point2, point4, pos);
                            QPointF add = pointSplid(point1, point2, distance);
                            point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                            add = pointSplid(point3, point4, distance);
                            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                        }
                    }
                    newResizeFPoints[0] = point1;
                    newResizeFPoints[1] = point2;
                    newResizeFPoints[2] = point3;
                    newResizeFPoints[3] = point4;
                    return newResizeFPoints;
                } else {
                    qreal width = getDistance(point1, point3);
                    qreal height = getDistance(point1, point2);
                    qreal ration = width / std::max(height, 0.01);

                    qreal distance = pointToLineDistance(point2,  point4, pos);
                    if (pointLineDir(point2, point4, pos) == -1) {
                        QPointF add = pointSplid(point1, point3, distance * ration);
                        point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                        add = pointSplid(point3, point4, distance);
                        point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                    } else {
                        QPointF add = pointSplid(point1, point3, distance * ration);
                        point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                        add = pointSplid(point3, point4, distance);
                        point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                    }
                    point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
                    newResizeFPoints[0] = point1;
                    newResizeFPoints[1] = point2;
                    newResizeFPoints[2] = point3;
                    newResizeFPoints[3] = point4;
                    return newResizeFPoints;
                }
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point8Resize1ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[3], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() > anotherFPoints[3].x()) {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() + add.y());
        } else {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() - add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(anotherFPoints[3], pos);
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() > anotherFPoints[3].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point8 in the second position */
FourPoints point8Resize2(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point6 = QPointF((point3.x() + point1.x()) / 2, (point3.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) <= -TANT_EDGEVALUE &&
            (pos.y() + MIN_PADDING > point6.y()  || pointLineDir(point1, point3, pos) == 1 ||
             pointLineDir(point3, point4, pos) == -1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) > -TANT_EDGEVALUE &&
               (pos.x() - MIN_PADDING < point6.x() || pointLineDir(point1, point3, pos) == 1 ||
                pointLineDir(point3, point4, pos) == -1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point1, point3, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point2,  point4, pos);
                if (pointLineDir(point2, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance * ration);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point3, distance);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance * ration);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                }
                point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point8Resize2ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[3], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() > anotherFPoints[3].x()) {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() - add.y());
        } else {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() + add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        QPointF add = pointSplid(point1, centerPos, distance);
        if (pos.x() > anotherFPoints[3].x()) {
            newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
        } else {
            newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
            QPointF add = pointSplid(point2, centerPos, distance);
            newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point8 in the third position */
FourPoints point8Resize3(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point6 = QPointF((point3.x() + point1.x()) / 2, (point3.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= TANT2_EDGEVALUE &&
            (pos.x() + MIN_PADDING > point6.x()  || pointLineDir(point1, point3, pos) == -1 ||
             pointLineDir(point3, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < TANT2_EDGEVALUE &&
               (pos.y() - MIN_PADDING < point6.y() || pointLineDir(point1, point3, pos) == -1 ||
                pointLineDir(point3, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point1, point3, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point2,  point4, pos);
                if (pointLineDir(point2, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point3, distance * ration);
                    point1 = QPointF(point1.x() - add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point3, distance * ration);
                    point1 = QPointF(point1.x() + add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() - add.y());
                }
                point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }
    return newResizeFPoints;
}

FourPoints point8Resize3ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[3], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.y() > anotherFPoints[3].y()) {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() + add.y());
        } else {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() - add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(anotherFPoints[3], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp3p4 = (point3.y() - point4.y()) / (point3.x() - point4.x());
        if (tanp3p4 <=  -1) {
            if (pos.y() > anotherFPoints[3].y()) {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            }
        } else {
            if (pos.y() < anotherFPoints[3].y()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point8 in the fourth position */
FourPoints point8Resize4(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF point6 = QPointF((point3.x() + point1.x()) / 2, (point3.y() + point1.y()) / 2);

    if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) >= -TANT2_EDGEVALUE &&
            (pos.y() + MIN_PADDING > point6.y()  || pointLineDir(point1, point3, pos) == 1 ||
             pointLineDir(point3, point4, pos) == 1)) {
        return newResizeFPoints;
    } else if (std::atan2((point2.y() - point1.y()), (point2.x() - point1.x())) < -TANT2_EDGEVALUE &&
               (pos.x() + MIN_PADDING > point6.x() || pointLineDir(point1, point3, pos) == 1 ||
                pointLineDir(point3, point4, pos) == 1)) {
        return newResizeFPoints;
    } else {
        if (pointToLineDistance(point3, point1, pos) < MIN_PADDING) {
            return newResizeFPoints;
        } else {
            if (!isShift) {
                if (pointLineDir(point2, point4, pos) == -1) {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() - add.x(), point2.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                } else {
                    qreal distance = pointToLineDistance(point2, point4, pos);
                    QPointF add = pointSplid(point1, point2, distance);
                    point2 = QPointF(point2.x() + add.x(), point2.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                }
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            } else {
                qreal distanceWidth = getDistance(point1, point3);
                qreal distanceHeight = getDistance(point1, point2);
                qreal ration = distanceWidth / std::max(distanceHeight, 0.01);

                qreal distance = pointToLineDistance(point2,  point4, pos);
                if (pointLineDir(point2, point4, pos) == 1) {
                    QPointF add = pointSplid(point1, point3, distance * ration);
                    point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
                } else {
                    QPointF add = pointSplid(point1, point3, distance * ration);
                    point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
                    add = pointSplid(point3, point4, distance);
                    point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
                }
                point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
                newResizeFPoints[0] = point1;
                newResizeFPoints[1] = point2;
                newResizeFPoints[2] = point3;
                newResizeFPoints[3] = point4;
                return newResizeFPoints;
            }
        }
    }

    return newResizeFPoints;
}

FourPoints point8Resize4ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2, (point1.y() + point4.y()) / 2);

    FourPoints anotherFPoints = getAnotherFPoints(newResizeFPoints);
    qreal distance = getDistance(anotherFPoints[3], pos);
    if (!isShift) {
        QPointF add = pointSplid(point1, point2, distance);
        if (pos.x() < anotherFPoints[3].x()) {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() - add.x(),
                                          newResizeFPoints[1].y() - add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() - add.x(),
                                          newResizeFPoints[3].y() - add.y());
        } else {
            newResizeFPoints[1] = QPointF(newResizeFPoints[1].x() + add.x(),
                                          newResizeFPoints[1].y() + add.y());
            newResizeFPoints[3] = QPointF(newResizeFPoints[3].x() + add.x(),
                                          newResizeFPoints[3].y() + add.y());
        }
        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - newResizeFPoints[3].x(),
                                      centerPos.y() * 2 - newResizeFPoints[3].y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
    } else {
        qreal distance = getDistance(anotherFPoints[3], pos);
        QPointF add = pointSplid(centerPos, point1, distance);
        qreal tanp1p3 = (point1.y() - point3.y()) / (point1.x() - point3.x());
        if (tanp1p3 <=  -1) {
            if (pos.x() < anotherFPoints[3].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() + add.y());
            }
        } else {
            if (pos.x() < anotherFPoints[3].x()) {
                newResizeFPoints[0] = QPointF(point1.x() + add.x(), point1.y() + add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() + add.x(), point2.y() - add.y());
            } else {
                newResizeFPoints[0] = QPointF(point1.x() - add.x(), point1.y() - add.y());
                QPointF add = pointSplid(centerPos, point2, distance);
                newResizeFPoints[1] = QPointF(point2.x() - add.x(), point2.y() + add.y());
            }
        }
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - newResizeFPoints[1].x(),
                                      centerPos.y() * 2 - newResizeFPoints[1].y());
        newResizeFPoints[3] = QPointF(centerPos.x() * 2 - newResizeFPoints[0].x(),
                                      centerPos.y() * 2 - newResizeFPoints[0].y());
    }
    return newResizeFPoints;
}

/* point8 in the fifth position */
FourPoints point8Resize5(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    bool withRation = true;
    qreal ration;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation) {
        qreal distance = pointToLineDistance(point2,  point4, pos);
        if (pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }
        point2 = QPointF(point4.x() - (point4.y() - point3.y()) * ration, point4.y());
        point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());

        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point2,  point4, pos);
        if (pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x(), point4.y() - add.y());
        }
        point2 = QPointF(point4.x() - (point4.y() - point3.y()) * ration, point4.y());
        point1 = QPointF(point2.x() + point3.x() - point4.x(), point2.y() + point3.y() - point4.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point8Resize5ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, const QString &rationName, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() < point1.y() + PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    qreal width = getDistance(point1, point3);
    qreal height = getDistance(point1, point2);
    qreal ration = width / std::max(height, 0.01);

    qreal newHeight = std::max(std::abs(pos.y() - centerPos.y()),
                               std::abs(pos.x() - centerPos.x()));
    qreal newWidth = newHeight * ration;

    bool withRation = true;
    if (rationName == "1:1") {
        ration = 1;
    } else if (rationName == "4:3") {
        ration = qreal(4) / qreal(3);
    } else if (rationName == "8:5") {
        ration = qreal(8) / qreal(5);
    } else if (rationName == "16:9") {
        ration = qreal(16) / qreal(9);
    } else {
        withRation = false;
    }

    if (withRation || isShift) {
        newWidth = ration * newHeight;

        newResizeFPoints[0] = QPointF(centerPos.x() - newWidth, centerPos.y() - newHeight);
        newResizeFPoints[1] = QPointF(centerPos.x() - newWidth, centerPos.y() + newHeight);
        newResizeFPoints[2] = QPointF(centerPos.x() + newWidth, centerPos.y() - newHeight);
        newResizeFPoints[3] = QPointF(centerPos.x() + newWidth, centerPos.y() + newHeight);
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = QPointF(point1.x(), centerPos.y() * 2 -  pos.y());
        newResizeFPoints[1] = QPointF(point1.x(), pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[3] = QPointF(point3.x(), pos.y());
    }

    return newResizeFPoints;
}

/* point8 in the sixth position */
FourPoints point8Resize6(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point1.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(point2.x(), pos.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(point4.x(), pos.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point2,  point4, pos);
        if (pointLineDir(point2, point4, pos) == 1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() - add.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y() + add.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x() + add.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y() - add.y());
        }
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }
    return newResizeFPoints;
}

FourPoints point8Resize6ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.y() > point1.y() - PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);
    if (!isShift) {
        newResizeFPoints[0] = QPoint(point1.x(), centerPos.y() * 2 - pos.y());
        newResizeFPoints[1] = QPointF(point1.x(), pos.y());
        newResizeFPoints[2] = QPointF(point3.x(), centerPos.y() * 2  - pos.y());
        newResizeFPoints[3] = QPointF(point3.x(), pos.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }
    return newResizeFPoints;
}

/* point8 in the seventh position */
FourPoints point8Resize7(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() -  PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point2,  point4, pos);
        qDebug() << "point8Resize7:" << pointLineDir(point2, point4, pos);

        if (pointLineDir(point2, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y());
        }
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point8Resize7ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() > point1.x() -  PADDING * 4) {
        return newResizeFPoints;
    }

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        if (std::abs(pos.x() - point1.x()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
            return newResizeFPoints;
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(pos.x(), point1.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/* point8 in the seventh position */
FourPoints point8Resize8(QPointF point1, QPointF point2, QPointF point3,
                         QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);
    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    if (pos.x() < point1.x() + PADDING * 4) {
        return newResizeFPoints;
    }

    if (!isShift) {
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = QPointF(pos.x(), point4.y());
    } else {
        qreal width = getDistance(point1, point3);
        qreal height = getDistance(point1, point2);
        qreal ration = width / std::max(height, 0.01);

        qreal distance = pointToLineDistance(point2,  point4, pos);
        qDebug() << "point8Resize8:" << pointLineDir(point2, point4, pos);

        if (pointLineDir(point2, point4, pos) == -1) {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() - add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() - add.x(), point4.y());
        } else {
            QPointF add = pointSplid(point1, point3, distance * ration);
            point1 = QPointF(point1.x(), point1.y() + add.y());
            add = pointSplid(point3, point4, distance);
            point4 = QPointF(point4.x() + add.x(), point4.y());
        }
        point2 = QPointF(point1.x() + point4.x() - point3.x(), point1.y() + point4.y() - point3.y());
        newResizeFPoints[0] = point1;
        newResizeFPoints[1] = point2;
        newResizeFPoints[2] = point3;
        newResizeFPoints[3] = point4;
    }

    return newResizeFPoints;
}

FourPoints point8Resize8ByAlt(QPointF point1, QPointF point2, QPointF point3,
                              QPointF point4, QPointF pos, bool isShift)
{
    FourPoints newResizeFPoints;
    newResizeFPoints = initFourPoints(newResizeFPoints);

    newResizeFPoints[0] = point1;
    newResizeFPoints[1] = point2;
    newResizeFPoints[2] = point3;
    newResizeFPoints[3] = point4;

    QPointF centerPos = QPointF((point1.x() + point4.x()) / 2,
                                (point1.y() + point4.y()) / 2);

    if (!isShift) {
        if (std::abs(pos.x() - point1.x()) < MIN_PADDING * 10) {
            newResizeFPoints[0] = point1;
            newResizeFPoints[1] = point2;
            newResizeFPoints[2] = point3;
            newResizeFPoints[3] = point4;
            return newResizeFPoints;
        }

        newResizeFPoints[0] = QPointF(centerPos.x() * 2 - pos.x(), point1.y());
        newResizeFPoints[1] = QPointF(pos.x(), point2.y());
        newResizeFPoints[2] = QPointF(centerPos.x() * 2 - pos.x(), point3.y());
        newResizeFPoints[3] = QPointF(pos.x(), point3.y());
    } else {
        newResizeFPoints = resizeByAltNoTiltKeepRation(newResizeFPoints, pos);
    }

    return newResizeFPoints;
}

/************************ micro-adjust  **************************/
FourPoints pointMoveMicro(FourPoints fourPoints, QString dir)
{
    QPointF point1 = fourPoints[0];
    QPointF point2 = fourPoints[1];
    QPointF point3 = fourPoints[2];
    QPointF point4 = fourPoints[3];
    if (dir == "Left") {
        point1 = QPoint(point1.x() - 1, point1.y());
        point2 = QPoint(point2.x() - 1, point2.y());
        point3 = QPoint(point3.x() - 1, point3.y());
        point4 = QPoint(point4.x() - 1, point4.y());
    }
    if (dir == "Right") {
        point1 = QPoint(point1.x() + 1, point1.y());
        point2 = QPoint(point2.x() + 1, point2.y());
        point3 = QPoint(point3.x() + 1, point3.y());
        point4 = QPoint(point4.x() + 1, point4.y());
    }
    if (dir == "Up") {
        point1 = QPoint(point1.x(), point1.y() - 1);
        point2 = QPoint(point2.x(), point2.y() - 1);
        point3 = QPoint(point3.x(), point3.y() - 1);
        point4 = QPoint(point4.x(), point4.y() - 1);
    }
    if (dir == "Down") {
        point1 = QPoint(point1.x(), point1.y() + 1);
        point2 = QPoint(point2.x(), point2.y() + 1);
        point3 = QPoint(point3.x(), point3.y() + 1);
        point4 = QPoint(point4.x(), point4.y() + 1);
    }
    fourPoints[0] = point1;
    fourPoints[1] = point2;
    fourPoints[2] = point3;
    fourPoints[3] = point4;
    return fourPoints;
}

FourPoints pointResizeMicro(FourPoints fourPoints, QString dir, bool isBig)
{
    Q_UNUSED(isBig);
    if (dir == "Ctrl+Left" || dir == "Ctrl+Shift+Left") {
        fourPoints = point5ResizeMicro(fourPoints, isBig);
    }
    if (dir == "Ctrl+Right" || dir == "Ctrl+Shift+Right") {
        fourPoints = point7ResizeMicro(fourPoints, isBig);
    }
    if (dir == "Ctrl+Up" || dir == "Ctrl+Shift+Up") {
        fourPoints = point6ResizeMicro(fourPoints, isBig);
    }
    if (dir == "Ctrl+Down" || dir == "Ctrl+Shift+Down") {
        fourPoints = point8ResizeMicro(fourPoints, isBig);
    }
    return fourPoints;
}
FourPoints point5ResizeMicro(FourPoints fourPoints, bool isBig)
{
    QPointF point1 = fourPoints[0];
    QPointF point2 = fourPoints[1];
    QPointF point3 = fourPoints[2];
//    QPointF point4 = fourPoints[3];
    if (getDistance(point1, point3) < MIN_PADDING && !isBig) {
        return fourPoints;
    } else {
        qreal distance = 1;
        qreal weight = 1;
        if (isBig) {
            weight = 1;
        } else {
            weight = -1;
        }

        if (point1.x() - point2.x() <= 0 && point1.y() - point2.y() <= 0 &&
                point1.x() - point3.x() <= 0 && point1.y() - point3.y() >= 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - weight * add.x(), point1.y() + weight * add.y());
            point2 = QPointF(point2.x() - weight * add.x(), point2.y() + weight * add.y());
            fourPoints[0] = point1;
            fourPoints[1] = point2;
            return fourPoints;
        }
        if (point1.x() - point2.x() < 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() > 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + weight * add.x(), point1.y() + weight * add.y());
            point2 = QPointF(point2.x() + weight * add.x(), point2.y() + weight * add.y());
            fourPoints[0] = point1;
            fourPoints[1] = point2;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() < 0 &&
                point1.x() - point3.x() < 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() - weight * add.x(), point1.y() - weight * add.y());
            point2 = QPointF(point2.x() - weight * add.x(), point2.y() - weight * add.y());
            fourPoints[0] = point1;
            fourPoints[1] = point2;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point1 = QPointF(point1.x() + weight * add.x(), point1.y() - weight * add.y());
            point2 = QPointF(point2.x() + weight * add.x(), point2.y() - weight * add.y());
            fourPoints[0] = point1;
            fourPoints[1] = point2;
            return fourPoints;
        }
    }
    return fourPoints;
}
FourPoints point6ResizeMicro(FourPoints fourPoints,  bool isBig)
{
    QPointF point1 = fourPoints[0];
    QPointF point2 = fourPoints[1];
    QPointF point3 = fourPoints[2];
//    QPointF point4 = fourPoints[3];
    if (getDistance(point1, point2) < MIN_PADDING && !isBig) {
        return fourPoints;
    } else {
        qreal distance = 1;
        qreal weight = 1;
        if (isBig) {
            weight = 1;
        } else {
            weight = -1;
        }

        if (point1.x() - point2.x() <= 0 && point1.y() - point2.y() <= 0 &&
                point1.x() - point3.x() <= 0 && point1.y() - point3.y() >= 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - weight * add.x(), point1.y() - weight * add.y());
            point3 = QPointF(point3.x() - weight * add.x(), point3.y() - weight * add.y());
            fourPoints[0] = point1;
            fourPoints[2] = point3;
            return fourPoints;
        }
        if (point1.x() - point2.x() < 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() > 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() - weight * add.x(), point1.y() + weight * add.y());
            point3 = QPointF(point3.x() - weight * add.x(), point3.y() + weight * add.y());
            fourPoints[0] = point1;
            fourPoints[2] = point3;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() < 0 &&
                point1.x() - point3.x() < 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + weight * add.x(), point1.y() - weight * add.y());
            point3 = QPointF(point3.x() + weight * add.x(), point3.y() - weight * add.y());
            fourPoints[0] = point1;
            fourPoints[2] = point3;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point1 = QPointF(point1.x() + weight * add.x(), point1.y() + weight * add.y());
            point3 = QPointF(point3.x() + weight * add.x(), point3.y() + weight * add.y());
            fourPoints[0] = point1;
            fourPoints[2] = point3;
            return fourPoints;
        }
    }
    return fourPoints;
}
FourPoints point7ResizeMicro(FourPoints fourPoints,  bool isBig)
{
    QPointF point1 = fourPoints[0];
    QPointF point2 = fourPoints[1];
    QPointF point3 = fourPoints[2];
    QPointF point4 = fourPoints[3];
    if (getDistance(point1, point3) < MIN_PADDING && !isBig) {
        return fourPoints;
    } else {
        qreal distance = 1;
        qreal weight = 1;
        if (isBig) {
            weight = 1;
        } else {
            weight = -1;
        }

        if (point1.x() - point2.x() <= 0 && point1.y() - point2.y() <= 0 &&
                point1.x() - point3.x() <= 0 && point1.y() - point3.y() >= 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + weight * add.x(), point3.y() - weight * add.y());
            point4 = QPointF(point4.x() + weight * add.x(), point4.y() - weight * add.y());
            fourPoints[2] = point3;
            fourPoints[3] = point4;
            return fourPoints;
        }
        if (point1.x() - point2.x() < 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() > 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - weight * add.x(), point3.y() - weight * add.y());
            point4 = QPointF(point4.x() - weight * add.x(), point4.y() - weight * add.y());
            fourPoints[2] = point3;
            fourPoints[3] = point4;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() < 0 &&
                point1.x() - point3.x() < 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() + weight * add.x(), point3.y() + weight * add.y());
            point4 = QPointF(point4.x() + weight * add.x(), point4.y() + weight * add.y());
            fourPoints[2] = point3;
            fourPoints[3] = point4;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point3, distance);
            point3 = QPointF(point3.x() - weight * add.x(), point3.y() + weight * add.y());
            point4 = QPointF(point4.x() - weight * add.x(), point4.y() + weight * add.y());
            fourPoints[2] = point3;
            fourPoints[3] = point4;
            return fourPoints;
        }
    }
    return fourPoints;
}

FourPoints point8ResizeMicro(FourPoints fourPoints,  bool isBig)
{
    QPointF point1 = fourPoints[0];
    QPointF point2 = fourPoints[1];
    QPointF point3 = fourPoints[2];
    QPointF point4 = fourPoints[3];
    if (getDistance(point1, point2) < MIN_PADDING && !isBig) {
        return fourPoints;
    } else {
        qreal distance = 1;
        qreal weight = 1;
        if (isBig) {
            weight = 1;
        } else {
            weight = -1;
        }

        if (point1.x() - point2.x() <= 0 && point1.y() - point2.y() <= 0 &&
                point1.x() - point3.x() <= 0 && point1.y() - point3.y() >= 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + weight * add.x(), point2.y() + weight * add.y());
            point4 = QPointF(point4.x() + weight * add.x(), point4.y() + weight * add.y());
            fourPoints[1] = point2;
            fourPoints[3] = point4;
            return fourPoints;
        }
        if (point1.x() - point2.x() < 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() > 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() + weight * add.x(), point2.y() - weight * add.y());
            point4 = QPointF(point4.x() + weight * add.x(), point4.y() - weight * add.y());
            fourPoints[1] = point2;
            fourPoints[3] = point4;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() < 0 &&
                point1.x() - point3.x() < 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - weight * add.x(), point2.y() + weight * add.y());
            point4 = QPointF(point4.x() - weight * add.x(), point4.y() + weight * add.y());
            fourPoints[1] = point2;
            fourPoints[3] = point4;
            return fourPoints;
        }
        if (point1.x() - point2.x() > 0 && point1.y() - point2.y() > 0 &&
                point1.x() - point3.x() > 0 && point1.y() - point3.y() < 0) {
            QPointF add = pointSplid(point1, point2, distance);
            point2 = QPointF(point2.x() - weight * add.x(), point2.y() - weight * add.y());
            point4 = QPointF(point4.x() - weight * add.x(), point4.y() - weight * add.y());
            fourPoints[1] = point2;
            fourPoints[3] = point4;
            return fourPoints;
        }
    }
    return fourPoints;
}

/***********************  special process   ***************************/
bool pointInRect(FourPoints fourPoints, QPointF pos)
{
    QPointF point1 = fourPoints[0];
    QPointF point2 = fourPoints[1];
    QPointF point3 = fourPoints[2];
    QPointF point4 = fourPoints[3];

    qreal sumArea = std::sqrt(std::pow(point1.x() - point2.x(), 2) + std::pow(point1.y() - point2.y(), 2)) * std::sqrt(
                        std::pow(point4.x() - point2.x(), 2) + std::pow(point4.y() - point2.y(), 2));

    qreal sumArea_1 = pointToLineDistance(point1, point2, pos) * std::sqrt(std::pow(point1.x() - point2.x(), 2) +
                                                                           std::pow(point1.y() - point2.y(), 2)) / 2;
    qreal sumArea_2 = pointToLineDistance(point4, point2, pos) * std::sqrt(std::pow(point4.x() - point2.x(), 2) +
                                                                           std::pow(point4.y() - point2.y(), 2)) / 2;
    qreal sumArea_3 = pointToLineDistance(point4, point3, pos) * std::sqrt(std::pow(point4.x() - point3.x(), 2) +
                                                                           std::pow(point4.y() - point3.y(), 2)) / 2;
    qreal sumArea_4 = pointToLineDistance(point1, point3, pos) * std::sqrt(std::pow(point1.x() - point3.x(), 2) +
                                                                           std::pow(point1.y() - point3.y(), 2)) / 2;

    qreal resultArea = sumArea_1 + sumArea_2 + sumArea_3 + sumArea_4;

    if (std::abs(resultArea - sumArea) <= 5) {
        return true;
    }

    if (sumArea_1 >= sumArea) {
        return false;
    }

    if (sumArea_2 >= sumArea || sumArea_2 + sumArea_1 > sumArea) {
        return false;
    }
    if (sumArea_3 >= sumArea || sumArea_3 + sumArea_2 + sumArea_1 > sumArea) {
        return false;
    }
    if (sumArea_4 >= sumArea || sumArea_4 + sumArea_3 + sumArea_2 + sumArea_1 > sumArea) {
        return false;
    }

    return true;
}

FourPoints getMainPoints(QPointF point1, QPointF point2, bool isShift)
{
    FourPoints fourPoints;
    fourPoints = initFourPoints(fourPoints);
    qreal PADDING = 4;

    qreal leftX = std::min(point1.x(), point2.x());
    qreal leftY = std::min(point1.y(), point2.y());

    qreal pWidth = std::max(std::abs(point1.x() - point2.x()), PADDING);
    qreal pHeight = std::max(std::abs(point1.y() - point2.y()), PADDING);

    if (isShift) {
        qreal shiftWidth = std::min(pWidth, pHeight);

        if (point2.x() >= point1.x()) {
            if (point2.y() >= point1.y()) {
                fourPoints[0] = point1;
                fourPoints[1] = QPointF(point1.x(), point1.y() + shiftWidth);
                fourPoints[2] = QPointF(point1.x() + shiftWidth, point1.y());
                fourPoints[3] = QPointF(point1.x() + shiftWidth, point1.y() + shiftWidth);
            } else {
                fourPoints[0] = QPointF(point1.x(), point1.y() - shiftWidth);
                fourPoints[1] = point1;
                fourPoints[2] = QPointF(point1.x() + shiftWidth, point1.y() - shiftWidth);
                fourPoints[3] = QPointF(point1.x() + shiftWidth, point1.y());
            }
        } else {
            if (point2.y() >= point1.y()) {
                fourPoints[0] = QPointF(point1.x() - shiftWidth, point1.y());
                fourPoints[1] = QPointF(point1.x() - shiftWidth, point1.y() + shiftWidth);
                fourPoints[2] = point1;
                fourPoints[3] = QPointF(point1.x(), point1.y() + shiftWidth);
            } else {
                fourPoints[0] = QPointF(point1.x() - shiftWidth, point1.y() - shiftWidth);
                fourPoints[1] = QPointF(point1.x() - shiftWidth, point1.y());
                fourPoints[2] = QPointF(point1.x(), point1.y() - shiftWidth);
                fourPoints[3] = point1;
            }
        }
    } else {
        fourPoints[0] = QPointF(leftX, leftY);
        fourPoints[1] = QPointF(leftX, leftY + pHeight);
        fourPoints[2] = QPointF(leftX + pWidth,  leftY);
        fourPoints[3] = QPointF(leftX + pWidth, leftY + pHeight);
    }

    return fourPoints;
}

FourPoints getMainPointsByAlt(QPointF centerPos, QPointF pos, bool isShift)
{
    FourPoints fourPoints;
    fourPoints = initFourPoints(fourPoints);
    const qreal PADDING = 5;
    qreal shiftWidth = std::max(std::abs(centerPos.x() - pos.x()), PADDING);
    qreal shiftHeight = std::max(std::abs(centerPos.y() - pos.y()), PADDING);

    if (isShift) {
        if (std::min(shiftWidth, shiftHeight) == 0)
            shiftWidth = std::max(std::max(shiftWidth, shiftHeight), PADDING);
        else
            shiftWidth = std::max(std::max(shiftWidth, shiftHeight), PADDING);
        fourPoints[0] = QPointF(centerPos.x() - shiftWidth, centerPos.y() - shiftWidth);
        fourPoints[1] = QPointF(centerPos.x() - shiftWidth, centerPos.y() + shiftWidth);
        fourPoints[2] = QPointF(centerPos.x() + shiftWidth, centerPos.y() - shiftWidth);
        fourPoints[3] = QPointF(centerPos.x() + shiftWidth, centerPos.y() + shiftWidth);
        return fourPoints;
    } else {
        fourPoints[0] = QPointF(centerPos.x() - shiftWidth, centerPos.y() - shiftHeight);
        fourPoints[1] = QPointF(centerPos.x() - shiftWidth, centerPos.y() + shiftHeight);
        fourPoints[2] = QPointF(centerPos.x() + shiftWidth, centerPos.y() - shiftHeight);
        fourPoints[3] = QPointF(centerPos.x() + shiftWidth, centerPos.y() + shiftHeight);
        return fourPoints;
    }
}

FourPoints getRationFPoints(QPointF point1, QPointF point2, QString ration)
{
    FourPoints fourPoints;
    fourPoints = initFourPoints(fourPoints);
    qreal PADDING = 4;

    qreal leftX = std::min(point1.x(), point2.x());
    qreal leftY = std::min(point1.y(), point2.y());

    qreal pWidth = std::max(std::abs(point1.x() - point2.x()), PADDING);
    qreal pHeight = std::max(std::abs(point1.y() - point2.y()), PADDING);

    qreal rationWidth, rationHeight;

    if (ration == "1:1") {
        rationWidth = std::min(pWidth, pHeight);
        rationHeight = rationWidth;
    } else if (ration == "4:3") {
        rationWidth = pWidth;
        rationHeight = 3 * rationWidth / 4;
    } else if (ration == "8:5") {
        rationWidth = pWidth;
        rationHeight = 5 * rationWidth / 8;
    } else if (ration == "16:9") {
        rationWidth = pWidth;
        rationHeight = 9 * rationWidth / 16;
    }

    if (ration != "free") {
        if (point2.x() >= point1.x()) {
            if (point2.y() >= point1.y()) {
                fourPoints[0] = point1;
                fourPoints[1] = QPointF(point1.x(), point1.y() + rationHeight);
                fourPoints[2] = QPointF(point1.x() + rationWidth, point1.y());
                fourPoints[3] = QPointF(point1.x() + rationWidth, point1.y() + rationHeight);
            } else {
                fourPoints[0] = QPointF(point1.x(), point1.y() - rationHeight);
                fourPoints[1] = point1;
                fourPoints[2] = QPointF(point1.x() + rationWidth, point1.y() - rationHeight);
                fourPoints[3] = QPointF(point1.x() + rationWidth, point1.y());
            }
        } else {
            if (point2.y() >= point1.y()) {
                fourPoints[0] = QPointF(point1.x() - rationWidth, point1.y());
                fourPoints[1] = QPointF(point1.x() - rationWidth, point1.y() + rationHeight);
                fourPoints[2] = point1;
                fourPoints[3] = QPointF(point1.x(), point1.y() + rationHeight);
            } else {
                fourPoints[0] = QPointF(point1.x() - rationWidth, point1.y() - rationHeight);
                fourPoints[1] = QPointF(point1.x() - rationWidth, point1.y());
                fourPoints[2] = QPointF(point1.x(), point1.y() - rationHeight);
                fourPoints[3] = point1;
            }
        }
    } else {
        fourPoints[0] = QPointF(leftX, leftY);
        fourPoints[1] = QPointF(leftX, leftY + pHeight);
        fourPoints[2] = QPointF(leftX + pWidth,  leftY);
        fourPoints[3] = QPointF(leftX + pWidth, leftY + pHeight);
    }

    return fourPoints;
}

/**************** divie the rectangle into nine parts ********************/
QList<QPointF> trisectionPoints(FourPoints fourPoints)
{
    QList<QPointF> pointsList;
    QPointF point0 =  QPointF(
                          (fourPoints[0].x() * 2 + fourPoints[1].x()) / 3,
                          (fourPoints[0].y() * 2 + fourPoints[1].y()) / 3
                      );
    QPointF point1 = QPointF(
                         (fourPoints[0].x() + fourPoints[1].x() * 2) / 3,
                         (fourPoints[0].y() + fourPoints[1].y() * 2) / 3
                     );

    QPointF point2 = QPointF(
                         (fourPoints[1].x() * 2 + fourPoints[3].x()) / 3,
                         (fourPoints[1].y() * 2 + fourPoints[3].y()) / 3
                     );
    QPointF point3 = QPointF(
                         (fourPoints[1].x() + fourPoints[3].x() * 2) / 3,
                         (fourPoints[1].y() + fourPoints[3].y() * 2) / 3
                     );

    QPointF point4 = QPointF(
                         (fourPoints[3].x() * 2 + fourPoints[2].x()) / 3,
                         (fourPoints[3].y() * 2 + fourPoints[2].y()) / 3
                     );
    QPointF point5 = QPointF(
                         (fourPoints[3].x() + fourPoints[2].x() * 2) / 3,
                         (fourPoints[3].y() + fourPoints[2].y() * 2) / 3
                     );

    QPointF point6 = QPointF(
                         (fourPoints[2].x() * 2 + fourPoints[0].x()) / 3,
                         (fourPoints[2].y() * 2 + fourPoints[0].y()) / 3
                     );
    QPointF point7 = QPointF(
                         (fourPoints[2].x() + fourPoints[0].x() * 2) / 3,
                         (fourPoints[2].y() + fourPoints[0].y() * 2) / 3
                     );

    pointsList.append(point0);
    pointsList.append(point1);
    pointsList.append(point2);
    pointsList.append(point3);
    pointsList.append(point4);
    pointsList.append(point5);
    pointsList.append(point6);
    pointsList.append(point7);

    return pointsList;
}

/****************  jude a point on rectangle ****************************/
bool pointOnRect(QRect rect, QPoint pos)
{
//    QPoint point1 = QPoint(rect.x(), rect.y());
//    QPoint point2 = QPoint(rect.x(), rect.y() + rect.height());
//    QPoint point3 = QPoint(rect.x() + rect.width(), rect.y());
//    QPoint point4 = QPoint(rect.x() + rect.width(), rect.y() + rect.height());

    if (pos.x() >= rect.x() && pos.x() <= rect.x() + rect.width()
            && pos.y() >= rect.y() && pos.y() <= rect.y() + rect.height()) {
        return true;
    }

    return false;
}

/********** The cut image has a proportional initialization point *******/
FourPoints getInitFourPointByCut(FourPoints mainPoints, const QString &rationName)
{

    QPointF centerPos = QPointF((mainPoints[0].x() + mainPoints[3].x()) / 2,
                                (mainPoints[0].y() + mainPoints[3].y()) / 2);
    qreal width = mainPoints[3].x() - mainPoints[0].x();
    qreal height = mainPoints[3].y() - mainPoints[0].y();
    FourPoints fourPoints;
    fourPoints = initFourPoints(fourPoints);
    if (rationName == "free") {
        return mainPoints;
    } else if (rationName == "1:1") {
        width = std::min(width, height);
        height = width;
    } else if (rationName == "4:3") {
        if (height > width * qreal(3) / qreal(4)) {
            height = width * qreal(3) / qreal(4);
        } else {
            width = height * qreal(4) / qreal(3);
        }
    } else if (rationName == "8:5") {
        if (height > width * qreal(5) / qreal(8)) {
            height = width * qreal(5) / qreal(8);
        } else {
            width = height * qreal(8) / qreal(5);
        }
    } else if (rationName == "16:9") {
        if (height > width * qreal(9) / qreal(16)) {
            height = width * qreal(9) / qreal(16);
        } else {
            width = height * qreal(16) / qreal(9);
        }
    }

    fourPoints[0] = QPointF(centerPos.x() - width / 2, centerPos.y() - height / 2);
    fourPoints[1] = QPointF(centerPos.x() - width / 2, centerPos.y() + height / 2);
    fourPoints[2] = QPointF(centerPos.x() + width / 2, centerPos.y() - height / 2);
    fourPoints[3] = QPointF(centerPos.x() + width / 2, centerPos.y() + height / 2);
    return fourPoints;
}

/*************************  Zoom point  ***********************/
FourPoints zoomPoints(FourPoints mainPoints, qreal zoomFactor)
{
    for (int i = 0; i < mainPoints.length(); i++) {
        mainPoints[i] = QPointF(mainPoints[i].x() * zoomFactor, mainPoints[i].y() * zoomFactor);
    }
    return mainPoints;
}

QList<QPointF> zoomPointList(QList<QPointF> points, qreal zoomFactor)
{
    for (int i = 0; i < points.length(); i++) {
        points[i] = QPointF(points[i].x() * zoomFactor, points[i].y() * zoomFactor);
    }
    return points;
}
