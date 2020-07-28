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

struct SComDefualData {
    QPen pen; //0
    QBrush bursh = QColor(0, 0, 0, 0); //1
    int rectRadius = 5; //2
    int starAnCount = 5; //3
    int starInRadiusRadio = 50; //4
    int polySideCount = 5; //5
    ELineType lineStartType = noneLine; //6
    ELineType lineEndType = noneLine; //7

    QColor textColor = QColor(0, 0, 0); //8
    QFont textFont; //9

    int masicType = 0; //10
    int masicWidth = 20; //11

    void save(int index, const QVariant &var)
    {
        switch (index) {
        case 0:
            pen = var.value<QPen>();
            break;
        case 1:
            bursh = var.value<QBrush>();
            break;
        case 2:
            rectRadius = var.toInt();
            break;
        case 3:
            starAnCount = var.toInt();
            break;
        case 4:
            starInRadiusRadio = var.toInt();
            break;
        case 5:
            polySideCount = var.toInt();
            break;
        case 6:
            lineStartType = ELineType(var.toInt());
            break;
        case 7:
            lineEndType = ELineType(var.toInt());
            break;
        case 8:
            textColor = var.value<QColor>();
            break;
        case 9:
            textFont = var.value<QFont>();
            break;
        case 10:
            masicType = var.toInt();
            break;
        case 11:
            masicWidth = var.toInt();
            break;
        }
    }

    SComDefualData()
    {
        pen.setWidth(2);
        pen.setColor(QColor(0, 0, 0));
    }
};

using CSceneDefaultData = QMap<CDrawScene *, SComDefualData>;

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
        CCmdBlock(CGraphicsItem *pItem, EChangedPhase phase = EChanged);
        ~CCmdBlock();

    private:
        CGraphicsItem *_pItem = nullptr;
        EChangedPhase _phase = EChangedUpdate;
    };

protected:
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

    QList<CGraphicsItem *> graphicItems();

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
    template<class T>
    void updateDefualData(int id, const T &var);

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
