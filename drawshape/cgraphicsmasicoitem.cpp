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
#include <QGraphicsView>
#include <QDebug>

DWIDGET_USE_NAMESPACE

static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (path == QPainterPath() || pen == Qt::NoPen)
        return path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;

}

/*CGraphicsMasicoItem::CGraphicsMasicoItem(CGraphicsItem *parent)
    : CGraphicsRectItem(parent)
    , m_pixmap(1362, 790)
    , m_nBlurEffect(MasicoEffect)
{

}

CGraphicsMasicoItem::CGraphicsMasicoItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem(rect, parent)
    , m_pixmap(1362, 790)
{

}*/

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

CGraphicsMasicoItem::CGraphicsMasicoItem(const SGraphicsBlurUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsPenItem(&(data->data), head, parent)
    , m_pixmap(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize())
    , m_nBlurEffect((EBlurEffect)data->effect)
{
    updateBlurPath();
}

//CGraphicsMasicoItem::CGraphicsMasicoItem(const CGraphicsUnit &unit, CGraphicsItem *parent)
//    : CGraphicsPenItem(unit, parent)
//    , m_pixmap(CDrawParamSigleton::GetInstance()->getCutSize())
//    , m_nBlurEffect(CDrawParamSigleton::GetInstance()->getBlurEffect())
//{
//    m_nBlurEffect = static_cast<EBlurEffect>(unit.data.pBlur->effect);
//    setBlurWidth(unit.data.pBlur->blurWidth);
//}

int CGraphicsMasicoItem::type() const
{
    return BlurType;
}

void CGraphicsMasicoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    updateGeometry();
    QGraphicsScene *scene = this->scene();
    //绘制滤镜
    if (scene != nullptr) {
        //计算交叉矩形的区域
        QRectF sceneRect = this->scene()->sceneRect();
        QRectF intersectRect = this->mapRectToScene(this->boundingRect()).intersected(this->scene()->sceneRect());
        QPointF interTopLeft = intersectRect.topLeft() - sceneRect.topLeft();
        QPointF interBottomRight = intersectRect.bottomRight() - sceneRect.topLeft();
        QRectF rectCopy = QRectF(interTopLeft, interBottomRight).normalized();
        QPixmap tmpPixmap = m_pixmap.copy(rectCopy.toRect());
        painter->save();
        painter->setClipPath(m_blurPath, Qt::IntersectClip);
        //判断和他交叉的元素，裁剪出下层的像素
        //下层有图元才显示
        int imgWidth = tmpPixmap.width();
        int imgHeigth = tmpPixmap.height();
        int radius = 5;
        if (m_nBlurEffect == BlurEffect) {
            tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth);
        }
        painter->drawPixmap(mapFromScene(intersectRect.topLeft()), tmpPixmap, QRectF());
        painter->restore();
    }

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPath(getPath());

    if (this->getMutiSelect()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale());
        if ( CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            pen.setColor(QColor(224, 224, 224));
        } else {
            pen.setColor(QColor(69, 69, 69));
        }
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}

void CGraphicsMasicoItem::setPixmap()
{
    if (this->scene() != nullptr) {
        bool flag = this->isSelected();
        QList<QGraphicsItem * > items = this->scene()->items();
        QList<QGraphicsItem * > filterItems = this->filterItems(items);
        QList<bool> filterItemsSelectFlags;
        int textItemIndex = -1;
        QTextCursor textCursor;

        auto curScene = static_cast<CDrawScene *>(scene());
        auto itemsMgr = curScene->getItemsMgr();
        auto itemsMgrFlag = itemsMgr->isVisible();
        if (itemsMgrFlag) {
            itemsMgr->setVisible(false);
        }

        for (int i = 0; i != filterItems.size(); i++) {
            filterItemsSelectFlags.push_back(filterItems[i]->isSelected());

            if (filterItems[i]->type() == TextType) {
                if (static_cast<CGraphicsTextItem *>(filterItems[i])->isEditable()) {
                    textItemIndex = i;
                    textCursor = static_cast<CGraphicsTextItem *>(filterItems[i])->getTextEdit()->textCursor();
                }
            }
            filterItems[i]->setVisible(false);
        }
        this->hide();
        QRect rect = this->scene()->sceneRect().toRect();
        m_pixmap = QPixmap(rect.width(), rect.height());
        m_pixmap.fill(QColor(255, 255, 255, 0));
        QPainter painterd(&m_pixmap);
        painterd.setRenderHint(QPainter::Antialiasing);
        painterd.setRenderHint(QPainter::SmoothPixmapTransform);

        curScene->getDrawParam()->setRenderImage(2);

        this->scene()->setBackgroundBrush(Qt::transparent);

        this->scene()->render(&painterd);

        curScene->getDrawParam()->setRenderImage(0);

        curScene->resetSceneBackgroundBrush();
        if (itemsMgrFlag) {
            itemsMgr->setVisible(true);
        }

//        this->scene()->setBackgroundBrush(Qt::transparent);

        //m_pixmap.save("/home/wang/Desktop/wang.png", "PNG");

        this->show();
        this->setSelected(flag);

        for (int i = 0; i != filterItems.size(); i++) {
            filterItems[i]->setVisible(true);
            filterItems[i]->setSelected(filterItemsSelectFlags[i]);
            if (textItemIndex == i) {
                static_cast<CGraphicsTextItem *>(filterItems[i])->getTextEdit()->show();
                static_cast<CGraphicsTextItem *>(filterItems[i])->getTextEdit()->setTextCursor(textCursor);
                static_cast<CGraphicsTextItem *>(filterItems[i])->getTextEdit()->setFocus(Qt::MouseFocusReason);
//                static_cast<CGraphicsTextItem *>(filterItems[i])->getTextEdit()->activateWindow();
                //static_cast<CGraphicsTextItem *>(filterItems[i])->getTextEdit()->grabKeyboard();
            }
        }

//        qDebug() << "textItemIndex = " << textItemIndex << endl;


    }
    //this->scene()->views()[0]->setFocus();
}

void CGraphicsMasicoItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QRectF CGraphicsMasicoItem::boundingRect() const
{
    QRectF rect = this->shape().boundingRect();;
    return rect;
}

QPainterPath CGraphicsMasicoItem::shape() const
{
    return qt_graphicsItem_shapeFromPath(getPath(), pen());
}

void CGraphicsMasicoItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    CGraphicsPenItem::resizeTo(dir, point);
    updateBlurPath();
}

void CGraphicsMasicoItem::updateBlurPath()
{
    QPainterPathStroker t_stroker;
    t_stroker.setWidth(pen().widthF());
    QPainterPath t_painterPath = t_stroker.createStroke(getPath());
    m_blurPath = t_painterPath.simplified();
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
}

CGraphicsUnit CGraphicsMasicoItem::getGraphicsUnit() const
{

    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsBlurUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pBlur = new SGraphicsBlurUnitData();
    unit.data.pBlur->data.penType = this->currentType();
    unit.data.pBlur->data.path = this->getPath();
    unit.data.pBlur->data.arrow = this->getArrow();

    unit.data.pBlur->effect = m_nBlurEffect;

    return unit;
}

void CGraphicsMasicoItem::duplicate(CGraphicsItem *item)
{
    CGraphicsPenItem::duplicate(item);
    static_cast<CGraphicsMasicoItem *>(item)->setBlurEffect(m_nBlurEffect);
    static_cast<CGraphicsMasicoItem *>(item)->updateBlurPath();
    static_cast<CGraphicsMasicoItem *>(item)->setPixmap(m_pixmap);
}

QList<QGraphicsItem *> CGraphicsMasicoItem::filterItems(QList<QGraphicsItem *> items)
{
    int index = 0;
    qreal thisZValue = this->zValue();
    QList<QGraphicsItem *> retList;
    if (this->scene() != nullptr) {
        QList<QGraphicsItem *> allitems = this->scene()->items();
        index = allitems.indexOf(this);

        foreach (QGraphicsItem *item, items) {
            //只对自定义的图元生效
            if (item->type() > QGraphicsItem::UserType && item->type() < MgrType ) {

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

