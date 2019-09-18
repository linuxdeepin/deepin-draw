/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     JiangChangli
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
