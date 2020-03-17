/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "cddfmanager.h"
#include "drawshape/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cgraphicsrectitem.h"
#include "drawshape/cpictureitem.h"
#include "drawshape/cgraphicsellipseitem.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicstriangleitem.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cgraphicspenitem.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include "drawshape/cgraphicsmasicoitem.h"
#include "frame/cgraphicsview.h"
#include "frame/cviewmanagement.h"
#include "widgets/dialog/cprogressdialog.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QtConcurrent>


CDDFManager::CDDFManager(CGraphicsView *view)
    : QObject(view)
    , m_CProgressDialog(new CProgressDialog(view))
    , m_view(view)
{
    connect(this, SIGNAL(signalUpdateProcessBar(int)), m_CProgressDialog, SLOT(slotupDateProcessBar(int)));
    connect(this, SIGNAL(signalSaveDDFComplete()), this, SLOT(slotSaveDDFComplete()));
    connect(this, SIGNAL(signalLoadDDFComplete()), this, SLOT(slotLoadDDFComplete()));
}


void CDDFManager::saveToDDF(const QString &path, const QGraphicsScene *scene)
{

    QList<QGraphicsItem *> itemList = scene->items(Qt::AscendingOrder);

    //即使无图元也可以进行保存
//    if (itemList.count() <= 0) {
//        return;
//    }


    int primitiveCount = 0;
    m_path = path;
    m_CProgressDialog->showProgressDialog(CProgressDialog::SaveDDF);

    foreach (QGraphicsItem *item, itemList) {
        CGraphicsItem *tempItem =  static_cast<CGraphicsItem *>(item);

        if (tempItem->type() >= RectType && CutType != item->type() ) {
            CGraphicsUnit graphicsUnit = tempItem->getGraphicsUnit();
            m_graphics.vecGraphicsUnit.push_back(graphicsUnit);
            primitiveCount ++;
        }
    }

    m_graphics.unitCount = primitiveCount;
    m_graphics.rect = scene->sceneRect();

    QtConcurrent::run([ = ] {
        QFile writeFile(path);

        if (writeFile.open(QIODevice::WriteOnly))
        {
            QDataStream out(&writeFile);
            out << (quint32)0xA0B0C0D0;
            out << RoundRect;
            out << m_graphics.unitCount;
            out << m_graphics.rect;

            int count = 0;
            int totalCount = m_graphics.vecGraphicsUnit.count();
            int process = 0;

            for (CGraphicsUnit &unit : m_graphics.vecGraphicsUnit) {
                out << unit;

                ///进度条处理
                count ++;
                process = (float)count / totalCount * 100;
                emit signalUpdateProcessBar(process);

                ///释放内存
                if (RectType == unit.head.dataType && nullptr != unit.data.pRect) {
                    delete unit.data.pRect;
                    unit.data.pRect = nullptr;
                } else if (EllipseType == unit.head.dataType && nullptr != unit.data.pCircle) {
                    delete unit.data.pCircle;
                    unit.data.pCircle = nullptr;
                } else if (TriangleType == unit.head.dataType && nullptr != unit.data.pTriangle) {
                    delete unit.data.pTriangle;
                    unit.data.pTriangle = nullptr;
                } else if (PolygonType == unit.head.dataType && nullptr != unit.data.pPolygon) {
                    delete unit.data.pPolygon;
                    unit.data.pPolygon = nullptr;
                } else if (polygonalStar == unit.head.dataType && nullptr != unit.data.pPolygonStar) {
                    delete unit.data.pPolygonStar;
                    unit.data.pPolygonStar = nullptr;
                } else if (LineType == unit.head.dataType && nullptr != unit.data.pLine) {
                    delete unit.data.pLine;
                    unit.data.pLine = nullptr;
                } else if (TextType == unit.head.dataType && nullptr != unit.data.pText) {
                    delete unit.data.pText;
                    unit.data.pText = nullptr;
                } else if (PictureType == unit.head.dataType && nullptr != unit.data.pPic) {
                    delete unit.data.pPic;
                    unit.data.pPic = nullptr;
                } else if (PenType == unit.head.dataType && nullptr != unit.data.pPen) {
                    delete unit.data.pPen;
                    unit.data.pPen = nullptr;
                } else if (BlurType == unit.head.dataType && nullptr != unit.data.pBlur) {
                    delete unit.data.pBlur;
                    unit.data.pBlur = nullptr;
                }
            }
            writeFile.close();
            m_graphics.vecGraphicsUnit.clear();
        }
        emit signalSaveDDFComplete();
    });
}

void CDDFManager::loadDDF(const QString &path, bool isOpenByDDF)
{
    emit signalClearSceneBeforLoadDDF();

    m_CProgressDialog->showProgressDialog(CProgressDialog::LoadDDF, isOpenByDDF);
    m_path = path;

    QtConcurrent::run([ = ] {
        QFile readFile(path);

        if (readFile.open(QIODevice::ReadOnly))
        {
            QDataStream in(&readFile);
            quint32 type;
            in >> type;
            int version;
            in >> version;
//            qDebug() << "loadDDF type = " << type << " version = " << version << endl;
//            qDebug() << "loadDDF type = " << (quint32)0xA0B0C0D0 << " version = " << RoundRect << endl;
            if (type != (quint32)0xA0B0C0D0) {
                in.device()->seek(0);
            }
            in >> m_graphics.unitCount;
            in >> m_graphics.rect;

            emit signalStartLoadDDF(m_graphics.rect);

            int count = 0;
            int process = 0;

            for (int i = 0; i < m_graphics.unitCount; i++) {
                CGraphicsUnit unit;
                in >> unit;

                if (RectType == unit.head.dataType) {
                    CGraphicsRectItem *item = new CGraphicsRectItem(*(unit.data.pRect), unit.head);
                    item->setXYRedius(unit.data.pRect->xRedius, unit.data.pRect->yRedius);
                    emit signalAddItem(item);

                    if (unit.data.pRect) {
                        delete unit.data.pRect;
                        unit.data.pRect = nullptr;
                    }
                } else if (EllipseType == unit.head.dataType) {
                    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(unit.data.pCircle, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pCircle) {
                        delete unit.data.pCircle;
                        unit.data.pCircle = nullptr;
                    }
                } else if (TriangleType == unit.head.dataType) {
                    CGraphicsTriangleItem *item = new CGraphicsTriangleItem(unit.data.pTriangle, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pTriangle) {
                        delete unit.data.pTriangle;
                        unit.data.pTriangle = nullptr;
                    }
                } else if (PolygonType == unit.head.dataType) {
                    CGraphicsPolygonItem *item = new CGraphicsPolygonItem(unit.data.pPolygon, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPolygon) {
                        delete unit.data.pPolygon;
                        unit.data.pPolygon = nullptr;
                    }
                } else if (PolygonalStarType == unit.head.dataType) {
                    CGraphicsPolygonalStarItem *item = new CGraphicsPolygonalStarItem(unit.data.pPolygonStar, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPolygonStar) {
                        delete unit.data.pPolygonStar;
                        unit.data.pPolygonStar = nullptr;
                    }
                } else if (LineType == unit.head.dataType) {
                    CGraphicsLineItem *item = new CGraphicsLineItem(unit.data.pLine, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pLine) {
                        delete unit.data.pLine;
                        unit.data.pLine = nullptr;
                    }
                } else if (TextType == unit.head.dataType) {
                    CGraphicsTextItem *item = new CGraphicsTextItem(unit.data.pText, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pText) {
                        delete unit.data.pText;
                        unit.data.pText = nullptr;
                    }
                } else if (PictureType == unit.head.dataType) {
                    CPictureItem *item = new CPictureItem(unit.data.pPic, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPic) {
                        delete unit.data.pPic;
                        unit.data.pPic = nullptr;
                    }
                } else if (PenType == unit.head.dataType) {
                    CGraphicsPenItem *item = new CGraphicsPenItem(unit.data.pPen, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPen) {
                        delete unit.data.pPen;
                        unit.data.pPen = nullptr;
                    }

                } else if (BlurType == unit.head.dataType) {
                    CGraphicsMasicoItem *item = new CGraphicsMasicoItem(unit.data.pBlur, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pBlur) {
                        delete unit.data.pBlur;
                        unit.data.pBlur = nullptr;
                    }

                }

                ///进度条处理
                count ++;
                process = (float)count / m_graphics.unitCount * 100;
                emit signalUpdateProcessBar(process);


            }
            in >> m_graphics.version;
            qDebug() << "loadDDF m_graphics.version = " << m_graphics.version << endl;
            readFile.close();
            emit signalLoadDDFComplete();
        }
    });

}

void CDDFManager::slotLoadDDFComplete()
{
    m_CProgressDialog->hide();
    m_view->getDrawParam()->setDdfSavePath(m_path);
    m_view->setModify(false);
    emit singalEndLoadDDF();
}

void CDDFManager::slotSaveDDFComplete()
{
    m_CProgressDialog->hide();
    m_view->getDrawParam()->setDdfSavePath(m_path);
    m_view->setModify(false);
    if (ESaveDDFTriggerAction::SaveAction != m_view->getDrawParam()->getSaveDDFTriggerAction()) {
        emit signalContinueDoOtherThing();
    }
}
