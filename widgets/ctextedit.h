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
#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H
//#include "cgraphicstextitem.h"
#include <DTextEdit>

#include <DMenu>
#include <DGraphicsView>

DWIDGET_USE_NAMESPACE

class CGraphicsTextItem;

class CTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CTextEdit(CGraphicsTextItem *item, QWidget *parent = nullptr);
    ~CTextEdit() override;

    virtual void setVisible(bool visible) Q_DECL_OVERRIDE;

    void setLastDocumentWidth(qreal width);

    void resizeDocument();

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
    int getSelectedFontWeight();

    /*
    * @bref: getSelectedTextColorAlpha 返回文本当前点击后是否所有透明度大小一致
    * @return:int
    */
    int getSelectedTextColorAlpha();

    /*
     * @bref: checkTextProperty 用于检验文字属性是否一致
    */
    void checkTextProperty(QTextBlock block);

public slots:
    void slot_textChanged();
    void cursorPositionChanged();

private:
    CGraphicsTextItem *m_pItem;
    qreal m_widthF;

    QColor m_selectedColor; // 所有选中的颜色
    int m_selectedSize;  // 所有选中的字体大小
    QString m_selectedFamily;// 所有选中的字体类型
    QString m_selectedFontStyle;// 所有选中的字体样式
    int m_selectedFontWeight;          // 所有选中字体的自重
    int m_selectedColorAlpha; //所有选中的文字颜色透明度
};

#endif // CTEXTEDIT_H
