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
#ifndef CDDFMANAGER_H
#define CDDFMANAGER_H

#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"

#include <QObject>
#include <DWidget>

class QGraphicsItem;
class QGraphicsScene;
class CGraphicsView;
class CProgressDialog;

DWIDGET_USE_NAMESPACE

class CDDFManager : public QObject
{
    Q_OBJECT

public:
    explicit CDDFManager(QObject *parent = nullptr, DWidget *widget = nullptr);

    void saveToDDF(const QString &path, const QGraphicsScene *scene);

    void loadDDF(const QString &path);
signals:
    void signalSaveDDFComplete();
    void signalLoadDDFComplete();
    void signalSaveDDFFaild();
    void singalLoadDDFFaild();
    void signalUpdateProcessBar(int);
    void signalStartLoadDDF(QRectF rect);
    void signalAddItem(QGraphicsItem *item);
    void signalContinueDoOtherThing();
    void singalEndLoadDDF();

private slots:
    void slotSaveDDFComplete();
    void slotLoadDDFComplete();


private:
    CGraphics m_graphics;
    CProgressDialog *m_CProgressDialog;
    QString m_path;


};

#endif // CDDFMANAGER_H
