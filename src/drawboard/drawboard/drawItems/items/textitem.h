/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef CGRAPHICSTEXTITEM_H
#define CGRAPHICSTEXTITEM_H
#include "rectitem.h"

#include <QTextDocument>
#include <QTextCharFormat>
#include <QWidget>
#include <QPainterPath>

class TextEdit;
class ProxyTextWidgetItem;
class CGraphicsItemHighLight;

class DRAWLIB_EXPORT TextItem : public RectBaseItem
{
public:
    explicit TextItem(const QString &text = "", PageItem *parent = nullptr);

    ~TextItem() override;

    SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase) override;

    TextEdit *textEditor() const;

    int  type() const override;

    void setText(const QString &text);
    QString text()const;

    QFont font() const;
    void setFont(const QFont &font);

    void setFontStyle(const QString &style);
    QString fontStyle();

    void setFontSize(int size);
    QVariant fontSize();

    void setFontFamily(const QString &family);
    QString fontFamily()const;

    void setTextColor(const QColor &col);
    QColor textColor() const;

    void setCurrentFormat(const QTextCharFormat &format, bool merge = false);
    QTextCharFormat currentCharFormat() const;

    void setWrap(bool b);
    bool isWrap() const;

    void setEditing(bool state, bool selectAllText = true);
    bool isEditing() const;

    bool isModified()const;

    void cut();
    void copy();
    void paste();
    void selectAll();
    void setAlignment(const Qt::Alignment &align);
    void undo();
    void redo();
    void deleteChar();

    void setInnerUndoEnable(bool b);
    bool isInnerUndoEnable() const;
    void clearInnerUndoStack();

protected:
    void loadUnit(const Unit &data) override;
    Unit getUnit(int reson) const override;

    QPainterPath calOrgShapeBaseRect(const QRectF &rect) const override;
    QPainterPath highLightPath()const override;
    QVariant pageItemChange(int doChange, const QVariant &value) override;

    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;
    bool isPosPenetrable(const QPointF &posLocal) override;
    void doScalBegin(PageItemScalEvent *event) override;

    PRIVATECLASS(TextItem)
};
class DRAWLIB_EXPORT TextHandleNode: public HandleNode
{
    Q_OBJECT
public:
    using HandleNode::HandleNode;
    TextItem *parentTextItem()const;
protected:
    bool isVisbleCondition()const override;
};
#endif // CGRAPHICSTEXTITEM_H
