/*
* Copyright (C) 2020 ~ 2021 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DZOOMMENUCOMBOBOX_H
#define DZOOMMENUCOMBOBOX_H

#include <DFrame>
#include <DWidget>
#include <DFloatingButton>

#include <QMenu>
#include <QWidgetAction>

DWIDGET_USE_NAMESPACE

class QHBoxLayout;

/*
* @bref: DZoomMenuComboBox 该组件效果类似DComboBox下拉框效果，左右两侧各有一个伸缩比例按钮
*/

class DZoomMenuComboBox : public DWidget
{
    Q_OBJECT
public:
    explicit DZoomMenuComboBox(DWidget *parent = nullptr);

    ~DZoomMenuComboBox();
    /*
    * @bref: addItem 添加子选项
    */
    void addItem(const QString &itemText);
    void addItem(QString itemText, QIcon icon);
    void addItem(QAction *action);

    /*
    * @bref: removeItem 删除子选项
    */
    void removeItem(const QString &itemText);
    void removeItem(int index);
    void removeItem(QAction *action);

    /*
    * @bref: setCurrentIndex 设置选中项
    */
    void setCurrentIndex(int index);
//    int getCurrentIndex() const;

    /*
    * @bref: setCurrentText 设置选中项
    */
    void setCurrentText(const QString &text);
//    QString getCurrentText() const;

    /*
    * @bref: setMenuFlat 设置flat
    */
    void setMenuFlat(bool flat);

//    /*
//    * @bref: setArrowDirction 设置菜单箭头显示位置
//    */
//    void setArrowDirction(Qt::LayoutDirection dir = Qt::LayoutDirection::RightToLeft);

    /*
    * @bref: setArrowICon 设置菜单图标
    */
    void setArrowICon(QIcon icon);

    /*
    * @bref: setItemICon 设置子菜单图标
    * @parma: text 子菜单选项文本名字
    * @parma: index 子菜单选项索引
    */
    void setItemICon(const QString &text, const QIcon icon);
    void setItemICon(int index, QIcon icon);

    /*
    * @bref: setMenuButtonTextAndIcon 设置菜单按钮的文字和图标
    */
    void setMenuButtonTextAndIcon(QString text, QIcon ico);

signals:
    /*
    * @bref: signalCurrentIndexChanged 下标索引改变信号
    */
    void signalCurrentIndexChanged(int index);
    /*
    * @bref: signalCurrentTextChanged 文本改变信号
    */
    void signalCurrentTextChanged(QString text);
    /*
    * @bref: signalLeftBtnClicked 左侧按钮点击信号
    */
    void signalLeftBtnClicked();
    /*
    * @bref: signalRightBtnClicked 右侧按钮点击信号
    */
    void signalRightBtnClicked();

protected slots:
    /*
    * @bref: slotActionToggled 处理子选项点击事件
    */
    void slotActionToggled(QAction *action);


protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    DIconButton *m_reduceBtn; // 减少按钮
    DIconButton *m_increaseBtn; // 增加按钮
    int m_floatingSize;

    DPushButton *m_btn; // 菜单按钮
    QMenu *m_menu; // 下拉菜单
    QList<QAction *> m_actions; // 子菜单项
    int m_currentIndex; // 选中的子菜单索引

    QHBoxLayout *_btnLay = nullptr;

    void initUI();
    void initConnection();
};

#endif // DZOOMMENUCOMBOBOX_H
