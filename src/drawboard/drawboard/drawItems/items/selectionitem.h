/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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

#ifndef SELECTIONITEM_H
#define SELECTIONITEM_H
#include <rectitem.h>

class PageScene;
class DRAWLIB_EXPORT SelectionItem: public QObject, public RectBaseItem
{
    Q_OBJECT
public:
    explicit SelectionItem(PageScene *scene);
    ~SelectionItem();

    SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase) override;

    int type()const override;

    void select(PageItem *p);
    void select(const QList<PageItem *> &items);

    void notSelect(PageItem *p);
    void notSelect(const QList<PageItem *> &items);
    void clear();

    QList<PageItem *> selectedItems() const;
    int selectedCount() const;

    PageItem *singleSelectedItem()const;
    void setBlockUpdateRect(bool bBlock);
    void operatingBegin(PageItemEvent *event) override;
    void operating(PageItemEvent *event) override;
    bool testOpetating(PageItemEvent *event) override;
    void operatingEnd(PageItemEvent *event) override;

signals:
    void selectionChanged(const QList<PageItem * > &selectedItems);

protected:
    bool contains(const QPointF &point) const override;

    void loadUnit(const Unit &data) override;
    Unit getUnit(int reson) const override;

    void updateRect();



    bool pageItemChangeFilter(PageItem *item, int changeType, QVariant &value) override;

    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;
    QPainterPath calOrgShapeBaseRect(const QRectF &rect) const override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    PRIVATECLASS(SelectionItem)
};

class DRAWLIB_EXPORT SelectionItemNode: public HandleNode
{
    Q_OBJECT
public:
    using HandleNode::HandleNode;

protected:
    bool isVisbleCondition()const override;
};
#endif // SELECTIONITEM_H
