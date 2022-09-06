// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QWidget>
#include <DPushButton>

DWIDGET_USE_NAMESPACE

enum ToolButonStyle {
    MENU_STYLE,
    BUTTON_STYLE
};

class ToolButton : public QPushButton
{
    Q_OBJECT
public:

    /**
     * @description: ToolButton 构造函数
    */
    explicit ToolButton(QWidget *parent = nullptr, ToolButonStyle style = MENU_STYLE);

    void setBtnStyle(ToolButonStyle style);
    void setShowText(bool bShowText);

    void setAlignment(Qt::Alignment alignment = Qt::AlignLeft);
protected:
    /**
     * @description: paintEvent 重新绘制控件的样式
    */
    void paintEvent(QPaintEvent *e) override;

    QPen getPen(const QStyleOptionButton option);
    QSize sizeHint() const override;

private:
    bool m_bShowText;
    ToolButonStyle m_style;
    Qt::Alignment m_alignment;
};

#endif // TOOLBUTTON_H
