#include "cgraphicstextitem.h"

CGraphicsTextItem::CGraphicsTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    initText();
}

CGraphicsTextItem::CGraphicsTextItem(const QString &text, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent)
{
    initText();
}

CGraphicsTextItem::~CGraphicsTextItem()
{

}

void CGraphicsTextItem::initText()
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
    this->setTextInteractionFlags(Qt::TextEditorInteraction);
}

