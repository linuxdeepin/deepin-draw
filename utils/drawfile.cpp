#include "drawfile.h"

#include "configsettings.h"
#include "tempfile.h"

#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QProcess>
#include <QDebug>
#include <QMetaType>

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
    QString saveFilename = QFileInfo(path).fileName();
    QString savePath = TempFile::instance()->getRandomFile(saveFilename, "ddf");
    QSettings* ddf = new QSettings(savePath, QSettings::NativeFormat, this);
    setItem(ddf, "windowSize", "size", windowSize);
    setItem(ddf, "canvasSize", "size", canvasSize);
    setItem(ddf, "artboardSize", "size", artboardSize);
    for(int i = 0; i < shapes.length(); i++)
    {
        setItem(ddf, QString("shape_%1").arg(i), "type", shapes[i].type);
        for(int j = 0; j < shapes[i].mainPoints.length(); j++)
        {
            setItem(ddf, QString("shape_%1").arg(i), QString("mainPoints%1").arg(j),
                     shapes[i].mainPoints[j]);
        }
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
        qDebug() << "ghkl:" << shapes[i].points.length() << shapes[i].portion.length();
        for(int k = 0; k < shapes[i].points.length(); k++)
        {
            setItem(ddf, QString("shape_%1").arg(i), QString("points%1").arg(k),
                    shapes[i].points[k]);
            if (shapes[i].portion.length() == 0 || k > shapes[i].portion.length() - 1)
                continue;
            else
                setItem(ddf, QString("shape_%1").arg(i), QString("portion%1").arg(k),
                    shapes[i].portion[k]);

        }
    }
    QProcess proc;
    qDebug() << "VBNM:" << savePath << path;
    proc.execute(QString("tar -P -cvf %1 %2").arg(path).arg(savePath));
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
    QString drawPath = path;
    QString dir = QDir(QFileInfo(drawPath).dir()).absolutePath();
    QString filename = QFileInfo(drawPath).fileName();
    QString suffix = QFileInfo(drawPath).suffix();
    filename = filename.remove(QString(".%1").arg(suffix));
    QString newName = QString(dir) + "/"+filename + "x" + ".ddf";
    QString newExtraName = QString(dir) + "/"+ filename + "x" + ".tar";
    QProcess proc;
    proc.execute(QString("cp %1 %2").arg(path).arg(newName));
    qDebug() << "cmd:" <<  QString("tar -xvf %1").arg(newExtraName);
    proc.execute(QString("mv %1 %2").arg(newName).arg(newExtraName));
    proc.start(QString("tar -P -xvf %1").arg(newExtraName));

    proc.waitForFinished(100);
    QString resultFile =  QString(proc.readAllStandardOutput().data());
    qDebug() << "resultFile:" << resultFile;
    QSettings* parseSettings = new QSettings(QFileInfo(resultFile.remove("\n")
                                                       ).absoluteFilePath(), QSettings::NativeFormat, this);
    m_windowSize = value(parseSettings, "windowSize", "size").toSize();
    m_canvasSize = value(parseSettings, "canvasSize", "size").toSize();
    m_artboardSize = value(parseSettings, "artboardSize", "size").toSize();

    QStringList allGroups = groups(parseSettings);
    qDebug() << "groups:" << resultFile<< allGroups.length()
             << m_windowSize << m_canvasSize << m_artboardSize;
    for(int i = 0; i < allGroups.length(); i++)
    {
        Toolshape shape;
        QString val = value(parseSettings, QString("shape_%1").arg(i), "type").toString();
        if (val.isEmpty())
            break;
        else
            shape.type = val;

        for(int j = 0; j < 4; j++)
        {
            shape.mainPoints[j] = value(parseSettings, QString("shape_%1").arg(i),
                                        QString("mainPoints%1").arg(j)).toPointF();
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

        bool loadPoints = true;
        for(int k = 0;loadPoints; k++)
        {
            QPointF point = value(parseSettings, QString("shape_%1").arg(i),
                                        QString("points%1").arg(k)).toPointF();
            QPointF portion = value(parseSettings, QString("shape_%1").arg(i),
                                   QString("portion%1").arg(k)).toPointF();
            if (point == QPointF(0, 0))
            {
                loadPoints = false;
                break;
            } else {
                shape.points.append(point);
                if (portion == QPointF(0, 0))
                    continue;
                    shape.portion.append(portion);
            }
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













