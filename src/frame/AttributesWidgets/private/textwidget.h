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
#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H
#include "drawshape/globaldefine.h"
#include "widgets/cfontcombobox.h"

#include <DWidget>
#include <DComboBox>
#include <DSpinBox>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class TextColorButton;
class SeperatorLine;

class TextWidget : public DWidget
{
    Q_OBJECT
public:
    explicit TextWidget(DWidget *parent = nullptr);
    ~TextWidget() override;

public slots:
    void setTextColor(const QColor &color, bool emitSig = false);
    void setFontSize(int size, bool emitSig = false);
    void setTextFamilyStyle(const QString &family, const QString &style = "",
                            bool emitSig = false,
                            /*bool isPreview = false, bool firstPreview = false*/EChangedPhase phase = EChanged);

    void setVaild(bool color, bool size, bool Family, bool Style);

    void setFamilyNull();

    void updateTheme();


signals:
    void fontSizeChanged(int size, bool divertFocus = false);
    void fontFamilyChanged(const QString &family, bool preview, bool firstPreview);
    void fontFamilyChangedPhase(const QString &family, EChangedPhase phase);
    void fontStyleChanged(const QString &style, EChangedPhase phase);
    void colorChanged(const QColor &color, EChangedPhase phase);

    void fontFamilyChangeFinished();
    void fontStyleChangeFinished();
    void fontSizeChangeFinished();

protected:
    bool eventFilter(QObject *, QEvent *event) override;

private:
    TextColorButton *m_fillBtn;
    CFontComboBox *m_fontComBox;
    SeperatorLine *m_textSeperatorLine;
    DComboBox *m_fontHeavy; // 字体的重量
    DComboBox *m_fontSize; // 字体的大小
    DLabel *m_fontFamilyLabel;
    DLabel *m_fontsizeLabel;

    QString m_oriFamily;
    bool    m_oneItemIsHighlighted;
    bool    m_keyPressed = false;


    bool   activedToPackUp     = false;    //是否是激活某个item引起的收起
    bool   firstHighlight      = false;    //是否是初次高亮

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection 初始化连接
     */
    void initConnection();

    void initDefaultSetting();

    void addFontPointSize();
};

#endif // TEXTWIDGET_H
