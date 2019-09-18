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
#ifndef CGRAPHICSTEXTITEM_H
#define CGRAPHICSTEXTITEM_H
#include "cgraphicsrectitem.h"

#include <QTextDocument>
#include <QTextCharFormat>
#include <QWidget>

class CTextEdit;
class CGraphicsProxyWidget;


class CGraphicsTextItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsTextItem();
    explicit CGraphicsTextItem(const CGraphicsUnit &unit, CGraphicsItem *parent = nullptr);
    ~CGraphicsTextItem() Q_DECL_OVERRIDE;

    CTextEdit *getTextEdit() const;
    virtual int  type() const Q_DECL_OVERRIDE;

    virtual void setRect(const QRectF &rect) Q_DECL_OVERRIDE;

    void setCGraphicsProxyWidget(CGraphicsProxyWidget *proxy);
    CGraphicsProxyWidget *getCGraphicsProxyWidget() const;
    void updateWidget();
    void setFont(const QFont &font);
    //void setFontSize(qreal size);
    void setTextColor(const QColor &col);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;

    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;
    CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;
    CTextEdit *getTextEdit();

    bool isEditable() const;



public slots:
    //选中后 更改字体和颜色
    void currentCharFormatChanged(const QTextCharFormat &format);

protected:

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;


private slots:
    void slot_textmenu(QPoint);
private:
    // 绘制QTextDocument中的指定区域
    void drawDocument(QPainter *painter,
                      const QTextDocument *doc,
                      const QRectF &r = QRectF(),
                      const QBrush &brush = Qt::NoBrush/*brush to draw all content.used for shadow draw*/);
    // 绘制Block及其内容
    void drawTextLayout(QPainter *painter,
                        const QTextBlock &block,
                        const QSizeF &sizeToFill,
                        const QBrush &brush);
    // 绘制文本及其内外边
    void drawText(QPainter *painter,
                  QPointF &p,
                  QString &text,
                  const QTextCharFormat &fmt,
                  const QBrush &brush);
    qreal alignPos(Qt::Alignment a,
                   const qreal &width,
                   const qreal &textWidth);
    bool needDrawText(const QTextCharFormat &chf);
    void clearLetterSpacing(QTextDocument *doc, int *blockNum = nullptr);
    void adjustAlignJustify(QTextDocument *doc, qreal DocWidth, int *blockNum = nullptr);
    void initTextEditWidget();


private:
    CTextEdit *m_pTextEdit;
    CGraphicsProxyWidget *m_pProxy;

    QFont m_Font;   //字体
    QColor m_color; //字体颜色

    QMenu *m_menu;
    QAction *m_action;
};

#endif // CGRAPHICSTEXTITEM_H
