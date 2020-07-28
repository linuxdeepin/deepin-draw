/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author: Ji Xianglong<jixianglong@uniontech.com>
*
* Maintainer: Ji Xianglong <jixianglong@uniontech.com>
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
#ifndef CITEMATTRIWIDGET_H
#define CITEMATTRIWIDGET_H

#include "sitemdata.h"
#include "bigcolorbutton.h"
#include "bordercolorbutton.h"
#include "csidewidthwidget.h"
#include "seperatorline.h"
#include "cspinbox.h"
#include "sitemdata.h"

#include <QWidget>
#include <QMap>

class CDrawScene;
class CGraphicsItem;

using CSceneDefaultData = QMap<CDrawScene *, CGraphicsUnit>;

class CItemAttriWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CItemAttriWidget(QWidget *parent = nullptr);

    /**
     * @brief setGraphicItem　设置当前图元
     * @return
     */
    void setGraphicItem(CGraphicsItem *pItem);

    /**
     * @brief graphicItem　获取当前图元
     * @return
     */
    CGraphicsItem *graphicItem();

    /**
     * @brief refresh　刷新
     * @return
     */
    virtual void refresh();

protected:
    Q_SLOT virtual void initUiWhenShow() {}

    class CBlockObjectSig
    {
    public:
        CBlockObjectSig(QObject *b)
            : _b(b)
        {
            b->blockSignals(true);
        }
        ~CBlockObjectSig() { _b->blockSignals(false); }

    private:
        QObject *_b = nullptr;
    };

    class CCmdBlock
    {
    public:
        CCmdBlock(CGraphicsItem *pItem);
        ~CCmdBlock();

    private:
        CGraphicsItem *_pItem = nullptr;
    };

private:
    CGraphicsItem *_pItem = nullptr;
};

using CPenColorBtn = BorderColorButton;
using CBrushColorBtn = BigColorButton;

class CComAttrWidget : public CItemAttriWidget
{
    Q_OBJECT
public:
    CComAttrWidget(QWidget *parent = nullptr);

    enum EAttriSourceItemType { Rect = 0x00000001,
                                Ellipse = Rect << 1,
                                Triangle = Rect << 2,
                                Star = Rect << 3,
                                Polygon = Rect << 4,
                                Pen = Rect << 5,
                                Line = Rect << 6,
                                MasicPen = Rect << 7,
                                Text = Rect << 8,
                                Image = Rect << 9,
                                Cut = Rect << 10,
                                ShowTitle = Rect << 11
    };

    void showByType(EAttriSourceItemType tp);

    CSceneDefaultData &defualtData();

protected:
    void refresh();

private:
    void clearUi();
    int getSourceTpByItem(CGraphicsItem *pItem);
    int getSourceTpByItemType(int itemType);

    bool isNeededNothing(int tp);
    bool isBrushColorNeeded(int tp);
    bool isBorderNeeded(int tp);
    bool isSpecialItem(int tp);
    void refreshHelper(int tp);
    void refreshDataHelper(int tp);

    /* -----公有的属性控件----- */
    QHBoxLayout *getLayout();
    DLabel *getTitleLabel();
    CPenColorBtn *getPenColorBtn();
    CBrushColorBtn *getBrushColorBtn();
    CSideWidthWidget *getBorderWidthWidget();
    SeperatorLine *getSpLine();

    /* -----rect图元的属性控件----- */
    CSpinBox *getSpinBoxForRectRadius();
    DLabel *getSpinBoxForRectRadiusLabel();

    /* -----star图元的属性控件----- */
    CSpinBox *getSpinBoxForStarAnchor();
    CSpinBox *getSpinBoxForStarinterRadius();
    DLabel *getSpinBoxForStarAnchorLabel();
    DLabel *getSpinBoxForStarinterRadiusLabel();

    /* -----polgon图元的属性控件----- */
    CSpinBox *getSpinBoxForPolgonSideNum();
    DLabel *getSpinBoxForPolgonSideNumLabel();

    /* -----线条类图元属性控件----- */
    DComboBox *getComboxForLineStartStyle();
    DComboBox *getComboxForLineEndStyle();
    DLabel *getLabelForLineStartStyle();
    DLabel *getLabelForLineEndStyle();
    DLabel *getMaskLabForLineStartStyle();
    DLabel *getMaskLabForLineEndStyle();

    /* -----  特殊的文字图元属性控件 ----- */

    /* -----  特殊的模糊图元属性控件 ----- */

    /* -----  特殊的裁剪图元属性控件 ----- */

    /* -----  特殊的图片图元属性控件 ----- */

private:
    void updateDefualData(bool isComProperty = false);

protected:
    QHBoxLayout *m_contrlLay = nullptr;

    //标题label
    DLabel *m_title = nullptr;

    /* 填充色显示 */
    CBrushColorBtn *m_fillBtn = nullptr;

    /* 边线色显示 */
    CPenColorBtn *m_strokeBtn = nullptr;

    /* 边线宽度控件 */
    CSideWidthWidget *m_sideWidthWidget = nullptr;

    SeperatorLine *m_sepLine = nullptr;

    /* 矩形圆角值 */
    CSpinBox *m_rediusSpinbox = nullptr;
    DLabel *m_rediusLable = nullptr;

    //线需要的控件
    DComboBox *m_lineStartComboBox = nullptr; // 线段起点样式
    DComboBox *m_lineEndComboBox = nullptr; // 线段终点样式
    DLabel *m_startLabel = nullptr;
    DLabel *m_endLabel = nullptr;
    DLabel *m_maskLableStart = nullptr;
    DLabel *m_maskLableEnd = nullptr;

    //星型图元需要的控件
    CSpinBox *m_anchorNumber = nullptr; //锚点数
    CSpinBox *m_radiusNumber = nullptr; //半径
    DLabel *m_anchorNumLabel = nullptr;
    DLabel *m_radiusLabel = nullptr;

    //多边形对应的侧边控件
    CSpinBox *m_sideNumSpinBox = nullptr; //侧边数
    DLabel *m_sideNumLabel = nullptr;

    //文字图元的属性控件

private:
    CSceneDefaultData m_defualDatas;
};

#endif // CITEMATTRIWIDGET_H
