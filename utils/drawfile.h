#ifndef DRAWFILE_H
#define DRAWFILE_H

#include <QObject>
#include "utils/shapesutils.h"

class DrawFile : public QObject
{
    Q_OBJECT
public:
    DrawFile(QObject* parent = 0);
    ~DrawFile();
    void createddf(QSize windowSize, QSize canvasSize,
                   QSize artboardSize, QString path,
                   QList<Toolshape> shapes);
    void parseddf(const QString &path);
    void setItem(QSettings* settings, const QString &group,
                           const QString &key, QVariant val);
    QVariant value(QSettings* settings, const QString &group,
                            const QString &key);
    QStringList groups(QSettings* settings);

    QList<Toolshape> toolshapes();
    QSize windowSize();
    QSize canvasSize();
    QSize artboardSize();

private:
    QSize m_windowSize;
    QSize m_canvasSize;
    QSize m_artboardSize;
    QList<Toolshape> m_allshapes;
};

#endif // DRAWFILE_H
