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

#include <DFrame>
#include <DMenu>
#include <DLabel>

#include <DWidget>
#include <DStackedWidget>
#include <DPushButton>
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

class TopToolbar : public DFrame
{
    Q_OBJECT

public:
    TopToolbar(DWidget *parent = nullptr);
    ~TopToolbar();

    DMenu *mainMenu();

signals:
    void signalSaveToDDF();
    void signalSaveAs();
    void signalPrint();
    void signalImport();
    void signalAttributeChanged();
    //传递图片的旋转和翻转信号
    void signalPassPictureOperation(int);
    //放大缩小信号
    void signalZoom(qreal);
    void signalShowExportDialog();
    void signalNew();

    void signalForwardCutTypeChanged(int);
    void signalForwardCutSizeChanged(int, int);


public:
//    void showDrawDialog();
    void showSaveDialog();
    void showPrintDialog();
    EDrawToolMode middleWidgetStatus();

public slots:
    void updateMiddleWidget(int type);
    void showColorfulPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void updateColorPanelVisible(QPoint pos);
    /**
     * @brief slotChangeAttributeFromScene 根据选中的图元更新属性栏
     * @param flag
     * @param primitiveType
     */
    void slotChangeAttributeFromScene(bool flag, int primitiveType);
    void slotZoom(const QString &scale);
    void slotSetScale(const qreal scale);
    void slotSetCutSize();

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
    DComboBox  *m_scaleComboBox;
    DMenu *m_mainMenu;


private:
    void initUI();
    void initConnection();
    void initStackWidget();
    void initMenu();
    void initComboBox();
};

#endif // TOPTOOLBAR_H
