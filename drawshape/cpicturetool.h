#ifndef CPICTURETOOL_H
#define CPICTURETOOL_H

#include "idrawtool.h"
#include "cpictureitem.h"
#include "cdrawscene.h"
#include"widgets/progresslayout.h"
#include <DFileDialog>

#include <QWidget>

DWIDGET_USE_NAMESPACE

class CPictureTool: public QWidget
{
    Q_OBJECT
public:
    CPictureTool(QWidget *parent = nullptr);
    ~CPictureTool();
signals:
    void addImageSignal(QPixmap pixmap, int itemNumber, CDrawScene *scene, QWidget *centralWindow);

public slots:
    void addImages(QPixmap pixmap, int itemNumber, CDrawScene *scene, QWidget *centralWindow);

public:

    void drawPicture(CDrawScene *scene, QWidget *centralWindow);
private:
    //QList<CPictureItem *> m_picturetItems;
    ProgressLayout *m_progressLayout;
    int m_picNum;

};


#endif // CPICTURETOOL_H
