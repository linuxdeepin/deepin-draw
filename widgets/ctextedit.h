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

    virtual void setVisible(bool visible) Q_DECL_OVERRIDE;

    void setLastDocumentWidth(qreal width);

    void resizeDocument();

    /*
    * @bref: getAllTextColorIsEqual 返回文本当前点击后是否所有文字颜色一致
    * @return:bool
    */
    bool getAllTextColorIsEqual();

    /*
    * @bref: getAllFontSizeIsEqual 返回文本当前点击后是否所有文字大小一致
    * @return:bool
    */
    bool getAllFontSizeIsEqual();

    /*
    * @bref: getAllFontFamilyIsEqual 返回文本当前点击后是否所有字体大小一致
    * @return:bool
    */
    bool getAllFontFamilyIsEqual();

    /*
    * @bref: getAllFontStyleIsEqual 返回文本当前点击后是否所有自重大小一致
    * @return:bool
    */
    bool getAllFontStyleIsEqual();

    void setFontStyle(QFont ft);

public slots:
    void slot_textChanged();
    void cursorPositionChanged();


private:
    CGraphicsTextItem *m_pItem;
    qreal m_widthF;

    bool m_allColorIsEqual; // 所有选中的颜色
    bool m_allSizeIsEqual;  // 所有选中的字体大小
    bool m_allFamilyIsEqual;// 所有选中的字体类型
    bool m_allFontStyleIsEqual;// 所有选中的字体样式

    /*
     * @bref: checkTextProperty 用于检验文字属性是否一致
    */
    void checkTextProperty(QTextBlock block);
};

#endif // CTEXTEDIT_H
