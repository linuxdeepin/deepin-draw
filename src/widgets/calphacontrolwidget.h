/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
#ifndef CALPHACONTROLWIDGET_H
#define CALPHACONTROLWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLabel>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

class CAlphaControlWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CAlphaControlWidget(DWidget *parent = nullptr);
    /**
     * @brief updateAlphaControlWidget　通过颜色Ａｌｐｈａ值更新控件
     * @param alpha　颜色ａｌｐｈａ值
     */
    void updateAlphaControlWidget(int alpha);

    /*
    * @bref:getCurrentAlphaValue 获取当前alpha值
    * @return: int alpha值
    */
    int getCurrentAlphaValue();

signals:
    /**
     * @brief signalAlphaChanged　alpha值改变信号
     */
    void signalAlphaChanged(int);
    /**
     * @brief signalFinishChanged　改变结束信号
     */
    void signalFinishChanged();

private:
    DSlider *m_alphaSlider;
    DLineEdit *m_alphaLabel;


private:
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();
};

#endif // CALPHACONTROLWIDGET_H
