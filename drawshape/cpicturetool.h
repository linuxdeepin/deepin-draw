#ifndef CPICTURETOOL_H
#define CPICTURETOOL_H

#include "idrawtool.h"
#include "cpictureitem.h"
#include "cdrawscene.h"
#include"widgets/progresslayout.h"
#include "frame/ccentralwidget.h"

#include <DFileDialog>


#include <DWidget>

DWIDGET_USE_NAMESPACE

class CPictureTool: public DWidget
{
    Q_OBJECT
public:
    CPictureTool(DWidget *parent = nullptr);
    ~CPictureTool();
signals:
    void addImageSignal(QPixmap pixmap, int itemNumber, CDrawScene *scene, CCentralwidget *centralWindow);

public slots:
    void addImages(QPixmap pixmap, int itemNumber, CDrawScene *scene, CCentralwidget *centralWindow);

public:

    void drawPicture(CDrawScene *scene, CCentralwidget *centralWindow);
private:
    //QList<CPictureItem *> m_picturetItems;
    ProgressLayout *m_progressLayout;
    int m_picNum;

};


#endif // CPICTURETOOL_H
