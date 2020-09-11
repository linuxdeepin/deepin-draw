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
#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include "drawshape/globaldefine.h"
#include "widgets/cpushbutton.h"

#include <DFrame>
#include <DMenu>
#include <DLabel>

#include <DWidget>
#include <DStackedWidget>
#include <QKeyEvent>


DWIDGET_USE_NAMESPACE

class CommonshapeWidget;
class PolygonalStarAttributeWidget;
class PolygonAttributeWidget;
class CCutWidget;
class CPenWidget;
class LineWidget;
class CPictureWidget;
class TextWidget;
class BlurWidget;
class CTitleWidget;

class ArrowRectangle;
class ColorPanel;
class CMenu;
class DZoomMenuComboBox;

class CComAttrWidget;

class TopToolbar : public DFrame
{
    Q_OBJECT

public:
    TopToolbar(DWidget *parent = nullptr);
    ~TopToolbar() Q_DECL_OVERRIDE;

    /* 主菜单 */
    DMenu *mainMenu();

    /* 属性widget */
    CComAttrWidget *attributWidget();

signals:
    /**
     * @brief signalSaveToDDF 保存ＤＤＦ文件信号
     */
    void signalSaveToDDF();
    /**
     * @brief signalSaveAs 另存为信号
     */
    void signalSaveAs();
    /**
     * @brief signalPrint　打印信号
     */
    void signalPrint();
    /**
     * @brief signalImport　导入信号
     */
    void signalImport();
    /**
     * @brief signalZoom　放大缩小信号
     */
    void signalZoom(qreal);
    /**
     * @brief signalShowExportDialog　显示导出窗口信号
     */
    void signalShowExportDialog();
    /**
     * @brief signalNew　新建信号
     */
    void signalNew();

    /**
     * @brief signalQuitCutModeFromTopBarMenu　点击菜单栏退出裁剪模式信号
     */
    void signalQuitCutModeFromTopBarMenu();

public:
    /**
     * @brief changeTopButtonsTheme　改变属性栏所有按钮的主题
     */
    void changeTopButtonsTheme();

public slots:

    /**
     * @brief slotZoom　缩放
     * @param scale　缩放因子
     */
    void slotZoom(const QString &scale);
    void slotZoom(const qreal &scale);
    /**
     * @brief slotSetScale　显示缩放比例
     * @param scale　缩放因子
     */
    void slotSetScale(const qreal scale);
    //    /**
    //     * @brief slotSetCutSize　更新裁剪尺寸属性栏
    //     */
    //    void slotSetCutSize();
    //    /**
    //     * @brief slotSetTextFont　更新文字属性栏
    //     */
    //    void slotSetTextFont();

    /**
     * @brief signalIsAllPictureItem　选中图元图元是否都为图片
     * @param isEnable　自适应按钮是否可用
     * @param single 是否选中单个图片
     */
    //void slotIsAllPictureItem(bool isEnable, bool single);

private slots:
    /**
     * @brief slotIsCutMode　判断是否为裁剪模式
     * @param action
     */
    void slotIsCutMode(QAction *action);
    /**
     * @brief slotOnImportAction　导入按钮触发函数
     */
    void slotOnImportAction();
    /**
     * @brief slotOnNewConstructAction　新建按钮触发函数
     */
    void slotOnNewConstructAction();
    /**
     * @brief slotOnSaveAction　保存按钮触发函数
     */
    void slotOnSaveAction();
    /**
     * @brief slotOnSaveAsAction　另存为按钮触发函数
     */
    void slotOnSaveAsAction();
    /**
     * @brief slotMenuShow　显示主菜单触发函数
     */
    void slotMenuShow();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;                      //进入QWidget瞬间事件

private:
    QString m_path;
    QStringList m_paths;
    int m_textFontsize = 12;

    CMenu *m_mainMenu;
    DZoomMenuComboBox *m_zoomMenuComboBox; // 缩放菜单组件
    QFont ft; // 全局默认字体

    QAction *m_saveAction;
    QAction *m_newAction;

    CComAttrWidget *m_pAtrriWidget = nullptr;

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();

    /**
     * @brief initMenu　初始化主菜单
     */
    void initMenu();

    /**
     * @brief initComboBox　初始化缩放下拉列表
     */
    void initComboBox();
};

#endif // TOPTOOLBAR_H
