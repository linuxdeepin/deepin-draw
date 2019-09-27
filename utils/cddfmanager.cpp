#include "cddfmanager.h"
#include "drawshape/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cgraphicsrectitem.h"
#include "drawshape/cpictureitem.h"
#include "drawshape/cgraphicsellipseitem.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicstriangleitem.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>


CDDFManager::CDDFManager(QObject *parent) : QObject(parent)
{

}

void CDDFManager::saveToDDF(const QString &path, const QGraphicsScene *scene)
{

    QList<QGraphicsItem *> itemList = scene->items(Qt::AscendingOrder);

    if (itemList.isEmpty()) {
        return;
    }

    int primitiveCount = 0;
    CGraphics graphics;

    foreach (QGraphicsItem *item, itemList) {
        CGraphicsItem *tempItem =  static_cast<CGraphicsItem *>(item);
        //暂时先不能保存画笔
        if (tempItem->type() >= RectType && CutType != item->type() && PenType != item->type()) {
            CGraphicsUnit graphicsUnit = tempItem->getGraphicsUnit();
            graphics.vecGraphicsUnit.push_back(graphicsUnit);
            primitiveCount ++;
        }
    }

    graphics.unitCount = primitiveCount;
    graphics.rect = scene->sceneRect();

    QFile writeFile(path);

    if (writeFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&writeFile);
        out << graphics;
        writeFile.close();
    }

    foreach (CGraphicsUnit unit, graphics.vecGraphicsUnit) {
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
        }
    }
}

bool CDDFManager::loadDDF(const QString &path, QGraphicsScene *scene, CGraphicsView *view)
{
    QFile readFile(path);

    if (!readFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    CGraphics graphics;

    QDataStream in(&readFile);

    in >> graphics;

    readFile.close();

    if (graphics.unitCount <= 0) {
        return false;
    }

    view->clearScene();
    scene->setSceneRect(graphics.rect);

    foreach (CGraphicsUnit unit, graphics.vecGraphicsUnit) {
        if (RectType == unit.head.dataType) {
            CGraphicsRectItem *item = new CGraphicsRectItem(unit);
            scene->addItem(item);

            if (unit.data.pRect) {
                delete unit.data.pRect;
                unit.data.pRect = nullptr;
            }
        } else if (EllipseType == unit.head.dataType) {
            CGraphicsEllipseItem *item = new CGraphicsEllipseItem(unit);
            scene->addItem(item);

            if (unit.data.pCircle) {
                delete unit.data.pCircle;
                unit.data.pCircle = nullptr;
            }
        } else if (TriangleType == unit.head.dataType) {
            CGraphicsTriangleItem *item = new CGraphicsTriangleItem(unit);
            scene->addItem(item);

            if (unit.data.pTriangle) {
                delete unit.data.pTriangle;
                unit.data.pTriangle = nullptr;
            }
        } else if (PolygonType == unit.head.dataType) {
            CGraphicsPolygonItem *item = new CGraphicsPolygonItem(unit);
            scene->addItem(item);

            if (unit.data.pPolygon) {
                delete unit.data.pPolygon;
                unit.data.pPolygon = nullptr;
            }
        } else if (PolygonalStarType == unit.head.dataType) {
            CGraphicsPolygonalStarItem *item = new CGraphicsPolygonalStarItem(unit);
            scene->addItem(item);

            if (unit.data.pPolygonStar) {
                delete unit.data.pPolygonStar;
                unit.data.pPolygonStar = nullptr;
            }
        } else if (LineType == unit.head.dataType) {
            CGraphicsLineItem *item = new CGraphicsLineItem(unit);
            scene->addItem(item);

            if (unit.data.pLine) {
                delete unit.data.pLine;
                unit.data.pLine = nullptr;
            }
        } else if (TextType == unit.head.dataType) {
            CGraphicsTextItem *item = new CGraphicsTextItem(unit);
            scene->addItem(item);

            if (unit.data.pText) {
                delete unit.data.pText;
                unit.data.pText = nullptr;
            }
        } else if (PictureType == unit.head.dataType) {
            CPictureItem *item = new CPictureItem(unit);
            scene->addItem(item);

            if (unit.data.pPic) {
                delete unit.data.pPic;
                unit.data.pPic = nullptr;
            }

        }
    }

    return true;
}



