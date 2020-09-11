/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef CCUTWIDGET_H
#define CCUTWIDGET_H

#include "widgets/seperatorline.h"
#include "globaldefine.h"

#include <DWidget>
#include <DLineEdit>
#include <QAction>

DWIDGET_USE_NAMESPACE

class QButtonGroup;
class CCutWidget : public DWidget
{
    Q_OBJECT
public:
public:
    explicit CCutWidget(DWidget *parent = nullptr);
    ~CCutWidget();

    /**
     * @brief updateCutSize 更新裁剪尺寸
     */
    void setCutSize(const QSize &sz, bool emitSig = true);
    QSize cutSize();

    /**
     * @brief setCutType　设置裁剪比例的类型
     */
    void setCutType(ECutType current, bool emitSig = true, bool adjustSz = true);
    ECutType cutType();

    /**
     * @brief setCutType　设置裁剪比例的类型
     */
    void  setDefualtRaidoBaseSize(const QSize &sz);
    QSize defualtRaidoBaseSize();


    void  setAutoCalSizeIfRadioChanged(bool b);
    bool  autoCalSizeIfRadioChanged();

signals:
    void cutSizeChanged(const QSize &sz);
    void cutTypeChanged(ECutType tp);
    void finshed(bool accept);

public slots:
    /**
     * @brief changeButtonTheme 根据主题改变按钮主题
     */
    void changeButtonTheme();

    /**
     * @brief clearAllChecked　清除所有按钮选中
     */
    void clearAllChecked();

private:
    void adjustSize(bool emitSig = true);

private:
    DPushButton *m_scaleBtn1_1;
    DPushButton *m_scaleBtn2_3;
    DPushButton *m_scaleBtn8_5;
    DPushButton *m_scaleBtn16_9;
    DPushButton *m_freeBtn;
    DPushButton *m_originalBtn;
    QButtonGroup *m_scaleBtnGroup = nullptr;

    bool          m_autoCal = true;
    const qreal Radio[cut_count] {1.0, 2.0 / 3.0, 8.0 / 5.0, 16.0 / 9.0, -1, -2};

    ECutType m_curCutType;
    QSize m_cutCutSize;

    QSize m_defultRadioSize;

    DLineEdit *m_widthEdit;
    DLineEdit *m_heightEdit;

    DPushButton *m_doneBtn;
    DPushButton *m_cancelBtn;
    SeperatorLine *m_sepLine;

    QAction *m_SizeAddAction;
    QAction *m_SizeReduceAction;

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection 初始化连接
     */
    void initConnection();
//    /**
//     * @brief activeFreeMode 激活自由按钮
//     */
//    void activeFreeMode();
};

#endif // CCUTWIDGET_H
