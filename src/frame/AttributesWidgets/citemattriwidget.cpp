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
#include "citemattriwidget.h"
#include "cgraphicsitem.h"
#include "cgraphicsitemselectedmgr.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicsitemselectedmgr.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicscutitem.h"
#include "cgraphicstriangleitem.h"
#include "cpictureitem.h"
#include "cdrawscene.h"

#include <DComboBox>

#include <QLineEdit>

CItemAttriWidget::CItemAttriWidget(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, "initUiWhenShow", Qt::QueuedConnection);
}

void CItemAttriWidget::setGraphicItem(CGraphicsItem *pItem)
{
    _pItem = pItem;
    refresh();
}

CGraphicsItem *CItemAttriWidget::graphicItem()
{
    return _pItem;
}

void CItemAttriWidget::refresh()
{
}

CItemAttriWidget::CCmdBlock::CCmdBlock(CGraphicsItem *pItem)
    : _pItem(pItem)
{
    if (_pItem == nullptr)
        return;
    if (_pItem->drawScene() != nullptr) {
        _pItem->drawScene()->recordItemsInfoToCmd(QList<CGraphicsItem *>() << pItem, true);
    }
}

CItemAttriWidget::CCmdBlock::~CCmdBlock()
{
    if (_pItem == nullptr)
        return;
    if (_pItem->drawScene() != nullptr) {
        _pItem->drawScene()->finishRecord(false);
    }
}

CComAttrWidget::CComAttrWidget(QWidget *parent)
    : CItemAttriWidget(parent)
{
    setAttribute(Qt::WA_NoMousePropagation, true);
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addStretch();
    lay->setMargin(0);
    lay->addLayout(getLayout());
    lay->addStretch();
    setLayout(lay);
}

void CComAttrWidget::showByType(CComAttrWidget::EAttriSourceItemType tp)
{
    setGraphicItem(nullptr);
    refreshHelper(tp);
}

CSceneDefaultData &CComAttrWidget::defualtData()
{
    return m_defualDatas;
}

void CComAttrWidget::refresh()
{
    clearUi();

    int tp = getSourceTpByItem(this->graphicItem());
    if (tp < 0)
        return;

    refreshHelper(tp);

    refreshDataHelper(tp);
}

void CComAttrWidget::clearUi()
{
    //    for (QObject *o : this->children()) {
    //        o->deleteLater();
    //    }
    //    setLayout(nullptr);

    //    m_sepLine = nullptr;
    //    m_fillBtn = nullptr;
    //    m_strokeBtn = nullptr;
    //    m_sideWidthWidget = nullptr;
    //    m_rediusSpinbox = nullptr;
    //    m_rediusLable = nullptr;
    //    m_lineStartComboBox = nullptr;

    //    m_lineEndComboBox = nullptr;   // 线段终点样式
    //    m_startLabel = nullptr;
    //    m_endLabel = nullptr;
    //    m_maskLableStart = nullptr;
    //    m_maskLableEnd = nullptr;

    //    //星型图元需要的控件
    //    m_anchorNumber = nullptr; //锚点数
    //    m_radiusNumber = nullptr; //半径
    //    m_anchorNumLabel = nullptr;
    //    m_radiusLabel = nullptr;
    //    m_sideNumSpinBox = nullptr; //侧边数

    //1.隐藏所有控件
    getTitleLabel()->hide();
    getPenColorBtn()->hide();
    getBrushColorBtn()->hide();
    getBorderWidthWidget()->hide();
    getSpLine()->hide();
    getSpinBoxForRectRadius()->hide();
    getSpinBoxForRectRadiusLabel()->hide();
    getSpinBoxForStarAnchor()->hide();
    getSpinBoxForStarinterRadius()->hide();
    getSpinBoxForStarAnchorLabel()->hide();
    getSpinBoxForStarinterRadiusLabel()->hide();
    getSpinBoxForPolgonSideNum()->hide();
    getSpinBoxForPolgonSideNumLabel()->hide();
    getComboxForLineStartStyle()->hide();
    getComboxForLineEndStyle()->hide();
    getLabelForLineStartStyle()->hide();
    getLabelForLineEndStyle()->hide();
    getMaskLabForLineStartStyle()->hide();
    getMaskLabForLineEndStyle()->hide();
    getPenColorBtn()->hide();

    //2.清理原先的布局内的控件
    QHBoxLayout *pLay = getLayout();
    for (int i = 0; i < pLay->count();) {
        pLay->takeAt(i);
    }
}

int CComAttrWidget::getSourceTpByItem(CGraphicsItem *pItem)
{
    int retTp = 0;

    if (pItem == nullptr) {
        retTp = ShowTitle;
        return retTp;
    }

    if (MgrType == pItem->type()) {
        QList<CGraphicsItem *> bzItems = dynamic_cast<CGraphicsItemSelectedMgr *>(pItem)->getItems();
        for (CGraphicsItem *p : bzItems) {
            if (p != nullptr)
                retTp |= getSourceTpByItem(p);
        }

        if (this->isNeededNothing(retTp)) {
            retTp = ShowTitle;
        }
        return retTp;
    }

    retTp = getSourceTpByItemType(pItem->type());

    return retTp;
}

int CComAttrWidget::getSourceTpByItemType(int itemType)
{
    int retTp = 0;

    switch (itemType) {
    case RectType: {
        retTp = Rect;
        break;
    }
    case EllipseType: {
        retTp = Ellipse;
        break;
    }
    case TriangleType: {
        retTp = Triangle;
        break;
    }
    case PolygonalStarType: {
        retTp = Star;
        break;
    }
    case PolygonType: {
        retTp = Polygon;
        break;
    }
    case LineType: {
        retTp = Line;
        break;
    }
    case PenType: {
        retTp = Pen;
        break;
    }
    case TextType: {
        retTp = Text;
        break;
    }
    case PictureType: {
        retTp = Image;
        break;
    }
    case CutType: {
        retTp = Cut;
        break;
    }
    case BlurType: {
        retTp = MasicPen;
        break;
    }
    default:
        retTp = -100;
    }
    return retTp;
}

bool CComAttrWidget::isNeededNothing(int tp)
{
    if (graphicItem()->type() == MgrType) {
        if (dynamic_cast<CGraphicsItemSelectedMgr *>(graphicItem())->count() > 1)
            if (tp & Text || tp & MasicPen || tp & Image) {
                return true;
            }
    }
    return false;
}

bool CComAttrWidget::isBrushColorNeeded(int tp)
{
    if (isNeededNothing(tp)) {
        return false;
    }

    if (tp & Rect || tp & Ellipse || tp & Triangle || tp & Polygon) {
        return true;
    }
    return false;
}

bool CComAttrWidget::isBorderNeeded(int tp)
{
    if (isNeededNothing(tp)) {
        return false;
    }

    return true;
}

bool CComAttrWidget::isSpecialItem(int tp)
{
    if (tp == Text || tp == MasicPen || tp == Image || tp == Cut) {
        return true;
    }
    return false;
}

void CComAttrWidget::refreshHelper(int tp)
{
    QHBoxLayout *layout = getLayout();

    if (isSpecialItem(tp) || tp == ShowTitle) {
        layout->addWidget(getTitleLabel());
        getTitleLabel()->setText(tr("Draw"));
        getTitleLabel()->show();
        return;
    }
    if (isBrushColorNeeded(tp)) {
        layout->addWidget(getBrushColorBtn());
        getBrushColorBtn()->show();
    }

    if (isBorderNeeded(tp)) {
        layout->addWidget(getPenColorBtn());
        layout->addWidget(getBorderWidthWidget());
        getPenColorBtn()->show();
        getBorderWidthWidget()->show();
    }

    switch (tp) {
    case Rect: {
        layout->addWidget(getSpLine());
        layout->addWidget(getSpinBoxForRectRadiusLabel());
        layout->addWidget(getSpinBoxForRectRadius());
        getSpLine()->show();
        getSpinBoxForRectRadiusLabel()->show();
        getSpinBoxForRectRadius()->show();
        break;
    }
    case Polygon: {
        layout->addWidget(getSpLine());
        layout->addWidget(getSpinBoxForPolgonSideNumLabel());
        layout->addWidget(getSpinBoxForPolgonSideNum());
        getSpLine()->show();
        getSpinBoxForPolgonSideNumLabel()->show();
        getSpinBoxForPolgonSideNum()->show();
        break;
    }
    case Star: {
        layout->addWidget(getSpLine());
        layout->addWidget(getSpinBoxForStarAnchorLabel());
        layout->addWidget(getSpinBoxForStarAnchor());
        getSpLine()->show();
        getSpinBoxForStarAnchorLabel()->show();
        getSpinBoxForStarAnchor()->show();

        layout->addWidget(getSpinBoxForStarinterRadiusLabel());
        layout->addWidget(getSpinBoxForStarinterRadius());
        getSpinBoxForStarinterRadiusLabel()->show();
        getSpinBoxForStarinterRadius()->show();
        break;
    }
    case Pen:
    case Line: {
        layout->addWidget(getSpLine());
        layout->addWidget(getLabelForLineStartStyle());
        layout->addWidget(getComboxForLineStartStyle());
        getSpLine()->show();
        getLabelForLineStartStyle()->show();
        getComboxForLineStartStyle()->show();

        layout->addSpacing(12);
        layout->addWidget(getLabelForLineEndStyle());
        layout->addWidget(getComboxForLineEndStyle());
        getLabelForLineEndStyle()->show();
        getComboxForLineEndStyle()->show();
        break;
    }
    default:
        break;
    }
}

void CComAttrWidget::refreshDataHelper(int tp)
{
    Q_UNUSED(tp);
}

QHBoxLayout *CComAttrWidget::getLayout()
{
    if (m_contrlLay == nullptr) {
        m_contrlLay = new QHBoxLayout;
        m_contrlLay->setMargin(0);
        m_contrlLay->setContentsMargins(0, 0, 0, 0);
    }
    return m_contrlLay;
}

DLabel *CComAttrWidget::getTitleLabel()
{
    if (m_title == nullptr) {
        m_title = new DLabel(this);
    }
    return m_title;
}

CPenColorBtn *CComAttrWidget::getPenColorBtn()
{
    if (m_strokeBtn == nullptr) {
        m_strokeBtn = new CPenColorBtn(this);
        connect(m_strokeBtn, &CPenColorBtn::colorChanged, this, [=](const QColor &color, bool preview) {
            if (this->graphicItem() != nullptr) {
                CCmdBlock block(preview ? nullptr : this->graphicItem());
                QPen p = this->graphicItem()->pen();
                p.setColor(color);
                this->graphicItem()->setPen(p);
                this->updateDefualData(true);
                qDebug() << "CPenColorBtn::colorChanged ------- " << p.color() << " item pen color = " << graphicItem()->pen().color() << "preview = " << preview;
            }
        });
    }
    return m_strokeBtn;
}

CBrushColorBtn *CComAttrWidget::getBrushColorBtn()
{
    if (m_fillBtn == nullptr) {
        m_fillBtn = new CBrushColorBtn(this);
        connect(m_fillBtn, &CBrushColorBtn::colorChanged, this, [=](const QColor &color, bool preview) {
            if (this->graphicItem() != nullptr) {
                CCmdBlock block(preview ? nullptr : this->graphicItem());
                QBrush br = this->graphicItem()->brush();
                br.setColor(color);
                this->graphicItem()->setBrush(br);
                this->updateDefualData(true);
                //qDebug() << "CBrushColorBtn::colorChanged ------- " << br.color() << " item br color = " << graphicItem()->brush().color() << "preview = " << preview;
            }
        });
    }
    return m_fillBtn;
}

CSideWidthWidget *CComAttrWidget::getBorderWidthWidget()
{
    if (m_sideWidthWidget == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_sideWidthWidget = new CSideWidthWidget(this);
        m_sideWidthWidget->setFixedWidth(100);
        m_sideWidthWidget->setFont(ft);

        connect(m_sideWidthWidget, &CSideWidthWidget::sideWidthChanged, this,
                [=](int lineWidth, bool preview) {
                    Q_UNUSED(preview)
                    if (this->graphicItem() != nullptr) {
                        CCmdBlock block(preview ? nullptr : this->graphicItem());
                        QPen p = this->graphicItem()->pen();
                        p.setWidthF(lineWidth);
                        this->graphicItem()->setPen(p);
                        this->updateDefualData(true);
                        this->graphicItem()->updateShape();
                        //qDebug() << "CSideWidthWidget::SideWidthChange -------  = " << lineWidth;
                    }
                });
    }
    return m_sideWidthWidget;
}

SeperatorLine *CComAttrWidget::getSpLine()
{
    if (m_sepLine == nullptr) {
        m_sepLine = new SeperatorLine(this);
    }

    return m_sepLine;
}

CSpinBox *CComAttrWidget::getSpinBoxForRectRadius()
{
    if (m_rediusSpinbox == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_rediusSpinbox = new CSpinBox(this);
        m_rediusSpinbox->setKeyboardTracking(false);
        m_rediusSpinbox->setRange(-1, INT_MAX);
        m_rediusSpinbox->setFixedSize(QSize(85, 36));
        m_rediusSpinbox->setFont(ft);
        m_rediusSpinbox->setSpecialValueText("— —");
        m_rediusSpinbox->setEnabledEmbedStyle(true);
        m_rediusSpinbox->lineEdit()->setClearButtonEnabled(false);
        m_rediusSpinbox->setProperty("preValue", 5);
        m_rediusSpinbox->setValue(5);

        connect(m_rediusSpinbox, &CSpinBox::valueChanged, this, [=](int value, bool preview) {
            //qDebug() << "value = " << value << "preview = " << preview;
            if (this->graphicItem() != nullptr) {
                //要知道这个控件是针对Rect图元的
                if (this->graphicItem()->type() == RectType) {
                    //记录undo
                    CCmdBlock block(preview ? nullptr : this->graphicItem());

                    CGraphicsRectItem *pItem = dynamic_cast<CGraphicsRectItem *>(this->graphicItem());
                    pItem->setXYRedius(value, value);
                    pItem->updateShape();
                    this->updateDefualData();
                }
            }
        });
    }
    return m_rediusSpinbox;
}

DLabel *CComAttrWidget::getSpinBoxForRectRadiusLabel()
{
    if (m_rediusLable == nullptr) {
        const int TEXT_SIZE = 13;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_rediusLable = new DLabel(this);
        m_rediusLable->setText(tr("Radius"));
        m_rediusLable->setFont(ft);
    }
    return m_rediusLable;
}

CSpinBox *CComAttrWidget::getSpinBoxForStarAnchor()
{
    if (m_anchorNumber == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_anchorNumber = new CSpinBox(this);
        m_anchorNumber->setKeyboardTracking(false);
        m_anchorNumber->setFixedSize(QSize(85, 36));
        m_anchorNumber->setRange(0, INT_MAX);
        m_anchorNumber->setFont(ft);
        m_anchorNumber->setSpecialValueText("— —");
        m_anchorNumber->setEnabledEmbedStyle(true);
        m_anchorNumber->lineEdit()->setClearButtonEnabled(false);
    }
    return m_anchorNumber;
}

CSpinBox *CComAttrWidget::getSpinBoxForStarinterRadius()
{
    if (m_radiusNumber == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_radiusNumber = new CSpinBox(this);
        m_radiusNumber->setKeyboardTracking(false);
        m_radiusNumber->setRange(-1, 1000);
        m_radiusNumber->setFixedSize(QSize(85, 36));
        m_radiusNumber->setSuffix("%");
        m_radiusNumber->setFont(ft);
        m_radiusNumber->setSpecialValueText("— —");
        m_radiusNumber->setEnabledEmbedStyle(true);
        m_radiusNumber->lineEdit()->setClearButtonEnabled(false);
    }
    return m_radiusNumber;
}

DLabel *CComAttrWidget::getSpinBoxForStarAnchorLabel()
{
    if (m_anchorNumLabel == nullptr) {
        const int TEXT_SIZE = 13;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_anchorNumLabel = new DLabel(this);
        m_anchorNumLabel->setText(tr("Points"));
        m_anchorNumLabel->setFont(ft);
    }
    return m_anchorNumLabel;
}

DLabel *CComAttrWidget::getSpinBoxForStarinterRadiusLabel()
{
    if (m_radiusLabel == nullptr) {
        const int TEXT_SIZE = 13;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_radiusLabel = new DLabel(this);
        m_radiusLabel->setText(tr("Diameter"));
        m_radiusLabel->setFont(ft);
    }
    return m_radiusLabel;
}

CSpinBox *CComAttrWidget::getSpinBoxForPolgonSideNum()
{
    if (m_sideNumSpinBox == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_sideNumSpinBox = new CSpinBox(this);
        m_sideNumSpinBox->setKeyboardTracking(false);
        m_sideNumSpinBox->setFixedSize(QSize(85, 36));
        m_sideNumSpinBox->setRange(0, INT_MAX);
        //m_sideNumSlider->setRange(4, 10);//此注释不删，记录边数范围
        m_sideNumSpinBox->setFont(ft);
        m_sideNumSpinBox->setSpecialValueText("— —");
        m_sideNumSpinBox->setEnabledEmbedStyle(true);
        m_sideNumSpinBox->lineEdit()->setClearButtonEnabled(false);
    }
    return m_sideNumSpinBox;
}

DLabel *CComAttrWidget::getSpinBoxForPolgonSideNumLabel()
{
    if (m_sideNumLabel == nullptr) {
        const int TEXT_SIZE = 13;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_sideNumLabel = new DLabel(this);
        m_sideNumLabel->setText(tr("Sides"));
        m_sideNumLabel->setFont(ft);
    }
    return m_sideNumLabel;
}

DComboBox *CComAttrWidget::getComboxForLineStartStyle()
{
    if (m_lineStartComboBox == nullptr) {
        m_lineStartComboBox = new DComboBox(this);
        m_lineStartComboBox->setFixedSize(QSize(90, 36));
        m_lineStartComboBox->setIconSize(QSize(34, 20));

        m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_none_arrow"), "");
        m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_circle"), "");
        m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_fill_circle"), "");
        m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_arrow"), "");
        m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_fill_arrow"), "");
    }
    return m_lineStartComboBox;
}

DComboBox *CComAttrWidget::getComboxForLineEndStyle()
{
    if (m_lineEndComboBox == nullptr) {
        m_lineEndComboBox = new DComboBox(this);
        m_lineEndComboBox->setFixedSize(QSize(90, 36));
        m_lineEndComboBox->setIconSize(QSize(34, 20));

        m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_none_arrow"), "");
        m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_right_circle"), "");
        m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_right_fill_circle"), "");
        m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_right_arrow"), "");
        m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_right_fill_arrow"), "");
    }
    return m_lineEndComboBox;
}

DLabel *CComAttrWidget::getLabelForLineStartStyle()
{
    if (m_startLabel == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);

        m_startLabel = new DLabel(this);
        m_startLabel->setText(tr("Start"));
        m_startLabel->setFont(ft);
    }
    return m_startLabel;
}

DLabel *CComAttrWidget::getLabelForLineEndStyle()
{
    if (m_endLabel == nullptr) {
        const int TEXT_SIZE = 14;
        QFont ft;
        ft.setPixelSize(TEXT_SIZE);
        m_endLabel = new DLabel(this);
        m_endLabel->setText(tr("End"));
        m_endLabel->setFont(ft);
    }
    return m_endLabel;
}

DLabel *CComAttrWidget::getMaskLabForLineStartStyle()
{
    if (m_maskLableStart == nullptr) {
        m_maskLableStart = new DLabel(getComboxForLineStartStyle());
        m_maskLableStart->setText("— —");
        m_maskLableStart->move(6, 6);
        m_maskLableStart->setFixedSize(35, 20);
        m_maskLableStart->setVisible(false);
        m_maskLableStart->setFont(m_maskLableStart->font());
    }
    return m_maskLableStart;
}

DLabel *CComAttrWidget::getMaskLabForLineEndStyle()
{
    if (m_maskLableEnd == nullptr) {
        m_maskLableEnd = new DLabel(getComboxForLineEndStyle());
        m_maskLableEnd->setText("— —");
        m_maskLableEnd->move(6, 6);
        m_maskLableEnd->setFixedSize(35, 20);
        m_maskLableEnd->setVisible(false);
        m_maskLableEnd->setFont(m_maskLableEnd->font());
    }
    return m_maskLableEnd;
}

void CComAttrWidget::updateDefualData(bool isComProperty)
{
    int ret = getSourceTpByItem(graphicItem());
    if (ret == ShowTitle) {
        return;
    }
    CGraphicsUnit &scDefual = m_defualDatas[graphicItem()->drawScene()];
    CGraphicsUnit itemsData = graphicItem()->getGraphicsUnit();
    if (isComProperty) {
        scDefual.head = itemsData.head;
        itemsData.data.release();
    } else {
        switch (graphicItem()->type()) {
        case RectType: {
            if (scDefual.data.pRect != nullptr) {
                delete scDefual.data.pRect;
                scDefual.data.pRect = nullptr;
            }
            scDefual.data.pRect = graphicItem()->getGraphicsUnit().data.pRect;
            break;
        }
        case EllipseType: {
            if (scDefual.data.pCircle != nullptr) {
                delete scDefual.data.pCircle;
                scDefual.data.pCircle = nullptr;
            }
            scDefual.data.pCircle = graphicItem()->getGraphicsUnit().data.pCircle;
            break;
        }
        case TriangleType: {
            if (scDefual.data.pTriangle != nullptr) {
                delete scDefual.data.pTriangle;
                scDefual.data.pTriangle = nullptr;
            }
            scDefual.data.pTriangle = graphicItem()->getGraphicsUnit().data.pTriangle;
            break;
        }
        case PolygonalStarType: {
            if (scDefual.data.pPolygonStar != nullptr) {
                delete scDefual.data.pPolygonStar;
                scDefual.data.pPolygonStar = nullptr;
            }
            scDefual.data.pPolygonStar = graphicItem()->getGraphicsUnit().data.pPolygonStar;
            break;
        }
        case PolygonType: {
            if (scDefual.data.pPolygon != nullptr) {
                delete scDefual.data.pPolygon;
                scDefual.data.pPolygon = nullptr;
            }
            scDefual.data.pPolygon = graphicItem()->getGraphicsUnit().data.pPolygon;
            break;
        }
        case LineType: {
            if (scDefual.data.pLine != nullptr) {
                delete scDefual.data.pLine;
                scDefual.data.pLine = nullptr;
            }
            scDefual.data.pLine = graphicItem()->getGraphicsUnit().data.pLine;
            break;
        }
        case PenType: {
            if (scDefual.data.pPen != nullptr) {
                delete scDefual.data.pPen;
                scDefual.data.pPen = nullptr;
            }
            scDefual.data.pPen = graphicItem()->getGraphicsUnit().data.pPen;
            break;
        }
        case TextType: {
            if (scDefual.data.pText != nullptr) {
                delete scDefual.data.pText;
                scDefual.data.pText = nullptr;
            }
            scDefual.data.pText = graphicItem()->getGraphicsUnit().data.pText;
            break;
        }
        case PictureType: {
            if (scDefual.data.pPic != nullptr) {
                delete scDefual.data.pPic;
                scDefual.data.pPic = nullptr;
            }
            scDefual.data.pPic = graphicItem()->getGraphicsUnit().data.pPic;
            break;
        }
            //        case CutType: {
            //            if (scDefual.data.pRect != nullptr) {
            //                delete scDefual.data.pRect;
            //                scDefual.data.pRect = nullptr;
            //            }
            //            scDefual.data.pRect = graphicItem()->getGraphicsUnit().data.pRect;
            //            break;
            //        }
        case BlurType: {
            if (scDefual.data.pBlur != nullptr) {
                delete scDefual.data.pBlur;
                scDefual.data.pBlur = nullptr;
            }
            scDefual.data.pBlur = graphicItem()->getGraphicsUnit().data.pBlur;
            break;
        }
        default:
            break;
        }
    }
}
