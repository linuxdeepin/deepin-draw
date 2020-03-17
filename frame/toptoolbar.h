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

class TopToolbar : public DFrame
{
    Q_OBJECT

public:
    TopToolbar(DWidget *parent = nullptr);
    ~TopToolbar() Q_DECL_OVERRIDE;

    DMenu *mainMenu();



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
     * @brief signalAttributeChanged　图元属性改变信号
     */
    void signalAttributeChanged();
    /**
     * @brief signalPassPictureOperation   传递图片的旋转和翻转信号
     */
    void signalPassPictureOperation(int);
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
     * @brief signalForwardCutTypeChanged　传递裁剪类型信号
     */
    void signalForwardCutTypeChanged(int);
    /**
     * @brief signalForwardCutSizeChanged　传递裁剪尺寸信号
     */
    void signalForwardCutSizeChanged(int, int);
    /**
     * @brief signalTextFontFamilyChanged　字体改变信号
     */
    void signalTextFontFamilyChanged();
    /**
     * @brief signalTextFontSizeChanged　字体大小改变信号
     */
    void signalTextFontSizeChanged();
    /**
     * @brief signalQuitCutModeFromTopBarMenu　点击菜单栏退出裁剪模式信号
     */
    void signalQuitCutModeFromTopBarMenu();
    /**
     * @brief signalCutLineEditIsfocus　裁剪尺寸输入框获取焦点信号
     */
    void signalCutLineEditIsfocus(bool);


public:
    /**
     * @brief changeTopButtonsTheme　改变属性栏所有按钮的主题
     */
    void changeTopButtonsTheme();

public slots:
    /**
     * @brief updateMiddleWidget　按类型更新工具栏
     * @param type　图元类型
     */
    void updateMiddleWidget(int type);
    /**
     * @brief showColorfulPanel　显示调色板
     * @param drawstatus　颜色类型
     * @param pos　显示位置
     * @param visible　是否显示
     */
    void showColorfulPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    /**
     * @brief updateColorPanelVisible　更新调色板
     * @param pos　位置
     */
    void updateColorPanelVisible(QPoint pos);
    /**
     * @brief slotChangeAttributeFromScene 根据选中的图元更新属性栏
     * @param flag
     * @param primitiveType　图元类型
     */
    void slotChangeAttributeFromScene(bool flag, int primitiveType);
    /**
     * @brief slotZoom　缩放
     * @param scale　缩放因子
     */
    void slotZoom(const QString &scale);
    /**
     * @brief slotSetScale　显示缩放比例
     * @param scale　缩放因子
     */
    void slotSetScale(const qreal scale);
    /**
     * @brief slotSetCutSize　更新裁剪尺寸属性栏
     */
    void slotSetCutSize();
    /**
     * @brief slotSetTextFont　更新文字属性栏
     */
    void slotSetTextFont();
    /**
     * @brief slotHideColorPanel　隐藏调色板
     */
    void slotHideColorPanel();
    /**
     * @brief slotRectRediusChanged　圆角矩形半径
     */
    void slotRectRediusChanged(int value);
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
    /**
     * @brief slotUpdateCurrentAttributeBar　更新属性栏
     */
    void slotUpdateCurrentAttributeBar();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;                      //进入QWidget瞬间事件

private:
    QString m_path;
    QStringList m_paths;
    DStackedWidget *m_stackWidget;
    int m_textFontsize = 12;

    CTitleWidget *m_titleWidget;
    CommonshapeWidget *m_commonShapeWidget;
    PolygonalStarAttributeWidget *m_polygonalStarWidget;
    PolygonAttributeWidget *m_PolygonWidget;
    CCutWidget *m_cutWidget;
    CPictureWidget *m_picWidget;
    LineWidget *m_drawLineWidget;
    TextWidget *m_drawTextWidget;
    BlurWidget *m_drawBlurWidget;
    CPenWidget *m_penWidget;


    ArrowRectangle *m_colorARect;
    ColorPanel *m_colorPanel;
    DrawStatus  m_drawStatus;
    //DComboBox  *m_scaleComboBox;
    CPushButton  *m_scaleComboBox;
    CMenu *m_mainMenu;

    QAction *m_saveAction;
    QAction *m_newAction;

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
     * @brief initStackWidget　初始化属性栏
     */
    void initStackWidget();
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
