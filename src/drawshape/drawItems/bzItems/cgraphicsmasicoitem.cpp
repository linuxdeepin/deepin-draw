#include "cgraphicsmasicoitem.h"
#include "cdrawparamsigleton.h"
#include "sitemdata.h"
#include "cgraphicstextitem.h"
#include "widgets/ctextedit.h"
#include "cgraphicsproxywidget.h"
#include "cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cgraphicsitemselectedmgr.h"

#include <DApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsView>
#include <QDebug>

DWIDGET_USE_NAMESPACE

CGraphicsMasicoItem::CGraphicsMasicoItem(QGraphicsItem *parent)
    : CGraphicsPenItem(parent)
    , m_pixmap(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize())
    , m_nBlurEffect(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurEffect())
{
    this->setSizeHandleRectFlag(CSizeHandleRect::Rotation, false);
}

CGraphicsMasicoItem::CGraphicsMasicoItem(const QPointF &startPoint, QGraphicsItem *parent)
    : CGraphicsPenItem(startPoint, parent)
    , m_pixmap(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize())
    , m_nBlurEffect(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurEffect())
{
    this->setSizeHandleRectFlag(CSizeHandleRect::Rotation, false);
}

int CGraphicsMasicoItem::type() const
{
    return BlurType;
}

void CGraphicsMasicoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateHandlesGeometry();
    QGraphicsScene *scene = this->scene();
    //绘制滤镜
    if (scene != nullptr) {
        //计算交叉矩形的区域
        QPixmap tmpPixmap = m_pixmap;
        painter->save();
        painter->setClipPath(m_blurPath, Qt::IntersectClip);
        //判断和他交叉的元素，裁剪出下层的像素
        //下层有图元才显示
        int imgWidth = tmpPixmap.width();
        int imgHeigth = tmpPixmap.height();
        const int radius = 10;
        if (!tmpPixmap.isNull()) {
            tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if (m_nBlurEffect == BlurEffect) {
                tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            } else {
                tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth);
            }
        }
        painter->drawPixmap(boundingRect().topLeft(), tmpPixmap);
        painter->restore();
    }
    paintMutBoundingLine(painter, option);
}

bool CGraphicsMasicoItem::isPosPenetrable(const QPointF &posLocal)
{
    Q_UNUSED(posLocal)
    return true;
}

void CGraphicsMasicoItem::updateMasicPixmap()
{
    updateBlurPixmap(Q3Alp);
}

void CGraphicsMasicoItem::updateMasicPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QPainterPath CGraphicsMasicoItem::getSelfOrgShape() const
{
    QPainterPath path = getPath();
    if (m_isShiftPress) {
        path.lineTo(m_straightLine.p2());
    }
    return path;
}

void CGraphicsMasicoItem::updateBlurPath()
{
    QPainterPathStroker t_stroker;
    t_stroker.setWidth(pen().widthF());
    QPainterPath path = getPath();
    if (m_isShiftPress) {
        path.lineTo(m_straightLine.p2());
    }
    QPainterPath t_painterPath = t_stroker.createStroke(path);
    m_blurPath = t_painterPath/*.simplified()*/;
}

QPainterPath CGraphicsMasicoItem::blurPath()
{
    return m_blurPath;
}

EBlurEffect CGraphicsMasicoItem::getBlurEffect() const
{
    return m_nBlurEffect;
}

void CGraphicsMasicoItem::setBlurEffect(EBlurEffect effect)
{
    prepareGeometryChange();
    m_nBlurEffect = effect;
    updateBlurPath();
}

int CGraphicsMasicoItem::getBlurWidth() const
{
    return this->pen().width();
}

void CGraphicsMasicoItem::setBlurWidth(int width)
{
    prepareGeometryChange();
    QPen pen = this->pen();
    pen.setWidth(width);
    this->setPen(pen);
    updateBlurPath();
    if (this->drawScene() != nullptr) {
        this->drawScene()->selectGroup()->updateBoundingRect();
    }
    updateMasicPixmap();
    CGraphicsItem::updateShape();
}

CGraphicsUnit CGraphicsMasicoItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;

    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsBlurUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = /*this->rotation()*/this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    unit.data.pBlur = new SGraphicsBlurUnitData();
    unit.data.pBlur->data.path = this->getPath();

    unit.data.pBlur->effect = m_nBlurEffect;

    return unit;
}

void CGraphicsMasicoItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pBlur != nullptr) {
        setPath(data.data.pPen->path);
        m_nBlurEffect = EBlurEffect(data.data.pBlur->effect);
    }
    loadHeadData(data.head);

    calcVertexes();

    updateBlurPath();

    updateHandlesGeometry();
}

QPixmap CGraphicsMasicoItem::blurPix()
{
    return m_pixmap;
}

QList<QGraphicsItem *> CGraphicsMasicoItem::filterItems(QList<QGraphicsItem *> items)
{
    qreal thisZValue = this->zValue();
    QList<QGraphicsItem *> retList;
    if (this->scene() != nullptr) {
        QList<QGraphicsItem *> allitems = this->scene()->items();

        foreach (QGraphicsItem *item, items) {
            //只对自定义的图元生效
            if ((item->type() > QGraphicsItem::UserType && item->type() < MgrType)) {
                if (item->type() == BlurType && item != this) {
                    retList.push_back(item);
                    continue;
                }

                qreal itemZValue = item->zValue();
                if (thisZValue < itemZValue) {
                    retList.push_back(item);
                }
            }
        }
    }

    return retList;
}

void CGraphicsMasicoItem::updateBlurPixmap(CGraphicsMasicoItem::EBlurAlp tp)
{
    if (tp == EFreeBlur) {
        freeBlurUpdate();
    } else if (tp == EOnlyImag) {
        onlyImageBlurUpdate();
    }
}

void CGraphicsMasicoItem::freeBlurUpdate()
{
    if (this->scene() != nullptr) {
        bool flag = this->isSelected();
        QList<QGraphicsItem * > items = this->scene()->items();
        QList<QGraphicsItem * > filterItems = this->filterItems(items);
        QList<bool> filterItemsSelectFlags;

        auto curScene = static_cast<CDrawScene *>(scene());
        auto itemsMgr = curScene->selectGroup();
        auto itemsMgrFlag = itemsMgr->isVisible();
        if (itemsMgrFlag) {
            itemsMgr->setFlag(ItemHasNoContents, true);
        }

        for (int i = 0; i != filterItems.size(); i++) {
            QGraphicsItem *pItem = filterItems[i];
            filterItemsSelectFlags.push_back(pItem->isSelected());
            pItem->setFlag(ItemHasNoContents, true);
            if (pItem->type() == TextType) {
                if (!pItem->childItems().isEmpty()) {
                    QGraphicsItem *pChild = pItem->childItems().first();
                    pChild->setFlag(ItemHasNoContents, true);
                }
            }
        }

        drawScene()->setDrawForeground(false);

        this->hide();
        QRect rect = this->sceneBoundingRect().toRect()/*this->scene()->sceneRect().toRect()*/;
        m_pixmap = QPixmap(rect.width(), rect.height());
        m_pixmap.fill(QColor(255, 255, 255, 0));
        QPainter painterd(&m_pixmap);
        painterd.setRenderHint(QPainter::Antialiasing);
        painterd.setRenderHint(QPainter::SmoothPixmapTransform);

        curScene->getDrawParam()->setRenderImage(2);

        this->scene()->setBackgroundBrush(Qt::transparent);

        this->scene()->render(&painterd, QRectF(0, 0, m_pixmap.width(), m_pixmap.height()),
                              rect);

        curScene->getDrawParam()->setRenderImage(0);

        curScene->resetSceneBackgroundBrush();
        if (itemsMgrFlag) {
            itemsMgr->setFlag(ItemHasNoContents, false);
        }

        this->show();
        this->setSelected(flag);

        for (int i = 0; i != filterItems.size(); i++) {
            QGraphicsItem *pItem = filterItems[i];
            pItem->setFlag(ItemHasNoContents, false);
            pItem->setSelected(filterItemsSelectFlags[i]);

            if (pItem->type() == TextType) {
                if (!pItem->childItems().isEmpty()) {
                    QGraphicsItem *pChild = pItem->childItems().first();
                    pChild->setFlag(ItemHasNoContents, false);
                }
            }
        }

        drawScene()->setDrawForeground(true);
    }
}

void CGraphicsMasicoItem::onlyImageBlurUpdate()
{
    if (parentItem() != nullptr) {
        //仅针对父对象进行模糊
        blurPath();
    }
}

//ELineType CGraphicsMasicoItem::getPenStartType() const
//{
//    return m_penStartType;
//}

//void CGraphicsMasicoItem::setPenStartType(const ELineType &penType)
//{
//    m_penStartType = penType;
//}

//ELineType CGraphicsMasicoItem::getPenEndType() const
//{
//    return m_penEndType;
//}

//void CGraphicsMasicoItem::setPenEndType(const ELineType &penType)
//{
//    m_penEndType = penType;
//}
