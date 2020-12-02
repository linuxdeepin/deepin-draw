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
#ifndef CGRAPHICSTEXTITEM_H
#define CGRAPHICSTEXTITEM_H
#include "cgraphicsrectitem.h"

#include <QTextDocument>
#include <QTextCharFormat>
#include <QWidget>
#include <QPainterPath>

class CTextEdit;
class CGraphicsProxyWidget;
class CGraphicsItemHighLight;

class CGraphicsTextItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsTextItem();
    ~CGraphicsTextItem() override;

    CTextEdit *getTextEdit() const;
    int  type() const override;

    void setRect(const QRectF &rect) override;

    void updateWidget();
    void setFont(const QFont &font);

    QString getTextFontStyle();
    void setTextFontStyle(const QString &style);

    void setFontSize(qreal size);
    qreal getFontSize();

    void setFontFamily(const QString &family);
    QString getFontFamily();

    void setTextColor(const QColor &col);
    QColor getTextColor();

    void setTextColorAlpha(const int &alpha);
    int getTextColorAlpha();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    // 设置缓存里面的字体样式
    void updateDefaultPropertyFromCache();

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) override;
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;
    CTextEdit *getTextEdit();

    bool isEditable() const;

    ///右键菜单触发功能
    void doCut();
    void doCopy();
    void doPaste();
    void doSelectAll();
    void setSelectTextBlockAlign(const Qt::Alignment &align);
    void doUndo();
    void doRedo();
    void doDelete();

    //选中后 更改字体和颜色
    bool getManResizeFlag() const;
    void setManResizeFlag(bool flag);
    void setLastDocumentWidth(qreal width);
    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    QPainterPath getHighLightPath() override;

    /*
    * @bref: getSelectedTextColor 返回文本当前点击后是否所有文字颜色一致
    * @return:QColor
    */
    QColor getSelectedTextColor();

    /*
    * @bref: getSelectedFontSize 返回文本当前点击后是否所有文字大小一致
    * @return:int
    */
    int getSelectedFontSize();

    /*
    * @bref: getSelectedFontFamily 返回文本当前点击后是否所有字体一致
    * @return:QString
    */
    QString getSelectedFontFamily();

    /*
    * @bref: getSelectedFontStyle 返回文本当前点击后是否所有自重大小一致
    * @return:QString
    */
    QString getSelectedFontStyle();

    /*
    * @bref: getSelectedTextColorAlpha 返回文本当前点击后是否所有透明度大小一致
    * @return:int
    */
    int getSelectedTextColorAlpha();

    void makeEditabel(bool selectAll = true);


    bool isSelectionEmpty();

    /**
     * @brief isGrabToolEvent 是否独占事件
     */
    bool isGrabToolEvent() override;

    /*
    * @bref: updateSelectAllTextProperty 更新文字图元所有文本的属性,这样才能获取到最新的选中文字属性
    */
    void updateSelectAllTextProperty();


    void beginPreview();
    void endPreview(bool loadOrg = true);
    bool isPreview();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    bool isPosPenetrable(const QPointF &posLocal) override;

    /**
     * @brief move  操作结束
     */
    void operatingEnd(CGraphItemEvent *event) override;

    /**
     * @brief itemChange 图元变更
     * @param change 变更属性
     * @param value 变更的值
     * @return
     */
    QVariant itemChange(GraphicsItemChange doChange, const QVariant &value) override;

private:
    // 绘制QTextDocument中的指定区域
    void drawDocument(QPainter *painter,
                      const QTextDocument *doc,
                      const QRectF &r = QRectF(),
                      const QBrush &brush = Qt::NoBrush/*brush to draw all content.used for shadow draw*/);

    // 绘制文本及其内外边
    void drawText(QPainter *painter,
                  QPointF &p,
                  QString &text,
                  const QTextCharFormat &fmt,
                  const QBrush &brush, qreal offset);

    bool needDrawText(const QTextCharFormat &chf);

    void initTextEditWidget();

    void initHandle() override;
private:
    CTextEdit *m_pTextEdit;
    CGraphicsProxyWidget *m_pProxy;

    QFont m_Font;   //字体
    QColor m_color; //字体颜色

    QMenu *m_menu;
    QAction *m_action;
    bool m_bManResize;//人工调整后的宽度

    //CGraphicsUnit dataBeforePreview;
    bool          _isPreview = false;
};

#endif // CGRAPHICSTEXTITEM_H
