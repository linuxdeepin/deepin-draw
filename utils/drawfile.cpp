#include "drawfile.h"

#include "configsettings.h"

#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

DrawFile::DrawFile(QObject *parent)
    : QObject(parent)
{
}

DrawFile::~DrawFile()
{
}

void DrawFile::createddf(QSize windowSize, QSize canvasSize,
               QSize artboardSize, QString path,
               QList<Toolshape> shapes)
{
    QSettings* ddf = new QSettings(path, QSettings::NativeFormat, this);
    setItem(ddf, "windowSize", "size", windowSize);
    setItem(ddf, "canvasSize", "size", canvasSize);
    setItem(ddf, "artboardSize", "size", artboardSize);
    for(int i = 0; i < shapes.length(); i++)
    {
        setItem(ddf, QString("shape_%1").arg(i), "type", shapes[i].type);
        setItem(ddf, QString("shape_%1").arg(i), "mainPoints", QVariant::fromValue(shapes[i].mainPoints));
        setItem(ddf, QString("shape_%1").arg(i), "index", QVariant(shapes[i].index));
        setItem(ddf, QString("shape_%1").arg(i), "fillColor", QVariant(shapes[i].fillColor));
        setItem(ddf, QString("shape_%1").arg(i), "strokeColor", QVariant(shapes[i].strokeColor));
        setItem(ddf, QString("shape_%1").arg(i), "isBlur", shapes[i].isBlur);
        setItem(ddf, QString("shape_%1").arg(i), "isMosaic", shapes[i].isMosaic);
        setItem(ddf, QString("shape_%1").arg(i), "isStraight", shapes[i].isStraight);
        setItem(ddf, QString("shape_%1").arg(i), "isShiftPressed", shapes[i].isShiftPressed);
        setItem(ddf, QString("shape_%1").arg(i), "isHorFlip", shapes[i].isHorFlip);
        setItem(ddf, QString("shape_%1").arg(i), "isVerFlip", shapes[i].isVerFlip);
        setItem(ddf, QString("shape_%1").arg(i), "imagePath", shapes[i].imagePath);
        setItem(ddf, QString("shape_%1").arg(i), "editImagePath", shapes[i].editImagePath);
        setItem(ddf, QString("shape_%1").arg(i), "rotate", shapes[i].rotate);
        setItem(ddf, QString("shape_%1").arg(i), "imageSize", shapes[i].imageSize);
        setItem(ddf, QString("shape_%1").arg(i), "fontSize", shapes[i].fontSize);
        setItem(ddf, QString("shape_%1").arg(i), "points", QVariant::fromValue(shapes[i].points));
        setItem(ddf, QString("shape_%1").arg(i), "portion", QVariant::fromValue(shapes[i].portion));
    }
}

void DrawFile::setItem(QSettings *settings, const QString &group,
    const QString &key, QVariant val)
{
    settings->beginGroup(group);
    settings->setValue(key, val);
    settings->endGroup();
    settings->sync();

}

QVariant DrawFile::value(QSettings* settings, const QString &group,
                        const QString &key)
{
    settings->beginGroup(group);
    QVariant val = settings->value(key);
    settings->endGroup();

    return val;
}

QStringList DrawFile::groups(QSettings* settings)
{
    return settings->childGroups();
}

void DrawFile::parseddf(const QString &path)
{
    QSettings* parseSettings = new QSettings(path, QSettings::NativeFormat, this);
    m_windowSize = value(parseSettings, "windowSize", "size").toSize();
    m_canvasSize = value(parseSettings, "canvasSize", "size").toSize();
    m_artboardSize = value(parseSettings, "artboardSize", "size").toSize();
    QStringList allGroups = groups(parseSettings);
    for(int i = 0; i < allGroups.length(); i++)
    {
        Toolshape shape;
        QString val = value(parseSettings, QString("shape_%1").arg(i), "type").toString();
        if (val.isEmpty())
            break;
        else
            shape.type = val;
        QVariantList fps = value(parseSettings, QString("shape_%1").arg(i),"mainPoints").toList();
        for(int j = 0; j < fps.length(); j++)
        {
            shape.mainPoints[j] = fps[j].toPointF();
        }
        shape.index = value(parseSettings, QString("shape_%1").arg(i),
                            "lineWidth").toInt();
        shape.fillColor = value(parseSettings, QString("shape_%1").arg(i),
                            "fillColor").value<QColor>();
        shape.strokeColor = value(parseSettings, QString("shape_%1").arg(i),
                             "strokeColor").value<QColor>();
        shape.isBlur = value(parseSettings, QString("shape_%1").arg(i),
                             "isBlur").toBool();
        shape.isMosaic = value(parseSettings, QString("shape_%1").arg(i),
                               "isMosaic").toBool();
        shape.isStraight = value(parseSettings, QString("shape_%1").arg(i),
                                 "isStraight").toBool();
        shape.isShiftPressed = value(parseSettings, QString("shape_%1").arg(i),
                                 "isShiftPressed").toBool();
        shape.isHorFlip = value(parseSettings, QString("shape_%1").arg(i),
                                "isHorFlip").toBool();
        shape.isVerFlip = value(parseSettings, QString("shape_%1").arg(i),
                                "isVerFlip").toBool();
        shape.imagePath = value(parseSettings, QString("shape_%1").arg(i),
                                "imagePath").toString();
        shape.editImagePath = value(parseSettings, QString("shape_%1").arg(i),
                                    "editImagePath").toString();
        shape.rotate = value(parseSettings, QString("shape_%1").arg(i),
                             "rotate").toReal();
        shape.imageSize = value(parseSettings, QString("shape_%1").arg(i),
                                "imageSize").toSize();
        shape.fontSize = value(parseSettings, QString("shape_%1").arg(i),
                               "fontSize").toInt();
        QVariantList pointList = value(parseSettings, QString("shape_%1").arg(i),
                                       "points").toList();
        QVariantList portionList = value(parseSettings, QString("shape_%1").arg(i),
                                         "portion").toList();
        for(int k = 0; k < pointList.length(); k++)
        {
            shape.points.append(pointList[k].toPointF());
            shape.portion.append(portionList[k].toPointF());
        }

        m_allshapes.append(shape);
    }
}

QList<Toolshape> DrawFile::toolshapes()
{
    return m_allshapes;
}

QSize DrawFile::windowSize()
{
    return m_windowSize;
}

QSize DrawFile::canvasSize()
{
    return m_canvasSize;
}

QSize DrawFile::artboardSize()
{
    return m_artboardSize;
}













