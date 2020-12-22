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
#include <QTextLayout>

DWIDGET_USE_NAMESPACE

class CGraphicsTextItem;

class CTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit CTextEdit(CGraphicsTextItem *item, QWidget *parent = nullptr);
    ~CTextEdit() override;

    /**
     * @brief currentFormat 当前文字的格式
     */
    QTextCharFormat currentFormat(bool considerSelection = true);

    /**
     * @brief setCurrentFormat 设置当前文字的格式
     */
    void setCurrentFormat(const QTextCharFormat &format, bool merge = false);

    /**
     * @brief currentColor 当前文字的颜色
     */
    QColor   currentColor();

    /**
     * @brief setCurrentColor 设置当前文字的格式
     */
    void setCurrentColor(const QColor &color);

    /**
     * @brief currentColor 当前文字的字体
     */
    QFont    currentFont(bool considerSelection = true);

    /**
     * @brief setCurrentColor 设置当前文字的格式
     */
    void setCurrentFont(const QFont &ft);

    /**
     * @brief currentColor 当前文字字体的字号(pt)
     */
    int  currentFontSize();

    /**
     * @brief setCurrentColor 设置当前文字字体的字号(pt)
     */
    void setCurrentFontSize(const int sz);

    /**
     * @brief currentFontFamily 当前文字字体的字体族
     */
    QString  currentFontFamily();

    /**
     * @brief setCurrentFontFamily 设置当前文字字体的字体族
     */
    void setCurrentFontFamily(const QString &family);

    /**
     * @brief currentFontStyle 当前文字字体的样式
     */
    QString  currentFontStyle();

    /**
     * @brief setCurrentFontStyle 设置当前文字字体的样式
     */
    void setCurrentFontStyle(const QString &style);


    /* 刷新背景色 (使用场景:因为不再随主题的变化而变化就要固定背景色)*/
    void updateBgColorTo(const QColor c = QColor(255, 255, 255), bool laterDo = false);

    /**
    * @brief toWeight 将style字符串转成int的字体粗细
    */
    static int  toWeight(const QString &styleName);

    /**
    * @brief toStyle 将字重int值转成style字符串
    */
    static QString toStyle(const int &weight);

public slots:
    /**
    * @brief onTextChanged 当doc中内容变化时进行响应(主要是实现自动调整大小)
    */
    void onTextChanged();

    /**
    * @brief onCursorPositionChanged 当光标位置变化时响应(刷新属性界面展示)
    */
    void onCursorPositionChanged();

    /**
    * @brief onSelectionChanged 选中区域变化时响应(刷新属性界面展示)
    */
    void onSelectionChanged();


    void onCurrentCharFormatChanged(const QTextCharFormat &format);

    /**
    * @brief updatePropertyWidget 刷新属性界面展示
    */
    void updatePropertyWidget();

protected:
    void insertFromMimeData(const QMimeData *source) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    QVector<QTextLayout::FormatRange> getCharFormats(int posBegin, int posEnd);

    void updateSelectionFormat();

private:
    CGraphicsTextItem *m_pItem = nullptr;
    QTextCharFormat _selectionFmt;

    QSet<QTextCharFormat::Property> _blockedProperties;


    bool _sflag = false;
};

#endif // CTEXTEDIT_H
