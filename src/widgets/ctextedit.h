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

#include <QInputMethodEvent>

DWIDGET_USE_NAMESPACE

class CGraphicsTextItem;

class CTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit CTextEdit(CGraphicsTextItem *item, QWidget *parent = nullptr);
    ~CTextEdit() override;

//    virtual void setVisible(bool visible) override;

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

    /*
    * @bref: getSelectedTextColorAlpha 返回文本当前点击后是否所有透明度大小一致
    * @return:int
    */
    int getSelectedTextColorAlpha();

    /*
     * @bref: checkTextProperty 用于检验文字属性是否一致
    */
    void checkTextProperty();

    /* 刷新背景色 (使用场景:因为不再随主题的变化而变化就要固定背景色)*/
    void updateBgColorTo(const QColor c = QColor(255, 255, 255), bool laterDo = false);

    /*
    * @bref: getFontWeigthByStyleName 根据字体的名字获取字重
    * 这样做的原因是QTextCharFormat直接设置style没有用(巨坑)
    */
    quint8 getFontWeigthByStyleName(const QString &styleName);

public slots:
    void slot_textChanged();
    void cursorPositionChanged();

protected:
    void insertFromMimeData(const QMimeData *source) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    CGraphicsTextItem *m_pItem;
    qreal m_widthF;

    QColor m_selectedColor; // 所有选中的颜色
    int m_selectedSize;  // 所有选中的字体大小
    QString m_selectedFamily;// 所有选中的字体类型
    int m_selectedFontweight;// 所有选中的字体样式
    int m_selectedColorAlpha; //所有选中的文字颜色透明度

    QInputMethodEvent m_e;//输入中文的预览文本

    /*
    * @bref: updateCurrentCursorProperty 更新鼠标处属性到变量中
    */
    void updateCurrentCursorProperty();

    QString getFontStyleByWeight(const int &weight);
    QTextCharFormat getCacheCharFormat();
};

#endif // CTEXTEDIT_H
