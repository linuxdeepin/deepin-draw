/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "cdrawparamsigleton.h"
#include <QGuiApplication>
#include <QFontDatabase>

#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cdrawscene.h"
#include "application.h"
#include "citemattriwidget.h"
#include "toptoolbar.h"
#include "cgraphicsitemselectedmgr.h"

CDrawParamSigleton::CDrawParamSigleton(const QString &uuid, bool isModified)
    : m_nlineWidth(2)
    , m_sLineColor(Qt::black)//black
    , m_nFillColor(QColor(Qt::white))
    , m_rectXRedius(5)
    , m_radiusNum(50)
    , m_anchorNum(5)
    , m_sideNum(5)
    , m_textFont()
    , m_textColor(Qt::black)
    , m_currentDrawToolMode(selection)
    , m_bShiftKeyPress(false)
    , m_bAltKeyPress(false)
    , m_bCtlKeyPress(false)
    , m_Scale(1)
    , m_cutAttributeType(ECutAttributeType::NoneAttribute)
    , m_cutType(ECutType::cut_free)
    , m_cutSize(1362, 790)
    , m_cutDefaultSize(1362, 790)
    , m_isModify(isModified)
    , m_saveDDFTriggerAction(ESaveDDFTriggerAction::SaveAction)
    , m_ddfSavePath("")
    , m_effect(MasicoEffect)
    , m_blurWidth(20)
    , m_imageAdjustScence(false)
    , m_imageFlipType(ERotationType::NoRotationType)
    , m_lineStartType(ELineType::noneLine)
    , m_lineEndType(ELineType::noneLine)
    , m_penStartType(ELineType::noneLine)
    , m_penEndType(ELineType::noneLine)
    , m_renderImage(0)
    , m_bSelectAlling(false)
{
    m_nFillColor = Qt::white;
    m_nFillColor.setAlpha(0);//transparent

    // 设置默认的字体类型为思源宋黑体，没有该字体则选择系统第一个默认字体
    QFontDatabase fontbase;
    QString sourceHumFont = QObject::tr("Source Han Sans CN");
    if (!fontbase.families().contains(sourceHumFont)) {
        sourceHumFont = fontbase.families().first();
    }
    this->setTextFontFamily(sourceHumFont);
    this->setTextFontStyle("Regular");
    this->setTextSize(14);

    if (uuid.isEmpty()) {
        m_keyUUID = creatUUID();
    } else {
        m_keyUUID = uuid;
    }
}

void CDrawParamSigleton::setLineWidth(int lineWidth)
{
    m_nlineWidth = lineWidth;
}

int CDrawParamSigleton::getLineWidth() const
{
    return m_nlineWidth;
}

void CDrawParamSigleton::setLineColor(const QColor &lineColor)
{
    m_sLineColor = lineColor;
}

QColor CDrawParamSigleton::getLineColor() const
{
    return m_sLineColor;
}

//void CDrawParamSigleton::setFillColor(const QColor &fillColor)
//{
//    m_nFillColor = fillColor;
//}

QColor CDrawParamSigleton::getFillColor() const
{
    return m_nFillColor;
}

void CDrawParamSigleton::setPen(const QPen &pen)
{
    m_nlineWidth = pen.width();
    m_sLineColor = pen.color();
}

QPen CDrawParamSigleton::getPen() const
{
    QPen pen;
    pen.setWidth(m_nlineWidth);
    pen.setColor(m_sLineColor);
    //SComDefualData data = dApp->topToolbar()->attributWidget()->defualtSceneData();

    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    return pen;
}

void CDrawParamSigleton::setBrush(const QBrush &brush)
{
    m_nFillColor = brush.color();
}

QBrush CDrawParamSigleton::getBrush() const
{
    return QBrush(m_nFillColor);
}

void CDrawParamSigleton::setCurrentDrawToolMode(EDrawToolMode mode)
{
    m_currentDrawToolMode = mode;

    if (mode != selection)
        CManageViewSigleton::GetInstance()->getCurView()->drawScene()->clearMrSelection();

    CGraphicsItem *pItem = nullptr;
    CComAttrWidget::EAttriSourceItemType tp = CComAttrWidget::ShowTitle;
    switch (mode) {
    case selection: {
        if (CManageViewSigleton::GetInstance()->getCurView()->drawScene()->getItemsMgr()->count() > 0) {
            return;
        }
        break;
    }
    case rectangle: {
        tp = CComAttrWidget::Rect;
        break;
    }
    case ellipse: {
        tp = CComAttrWidget::Ellipse;
        break;
    }
    case triangle: {
        tp = CComAttrWidget::Triangle;
        break;
    }
    case polygonalStar: {
        tp = CComAttrWidget::Star;
        break;
    }
    case polygon: {
        tp = CComAttrWidget::Polygon;
        break;
    }
    case line: {
        tp = CComAttrWidget::Line;
        break;
    }
    case pen: {
        tp = CComAttrWidget::Pen;
        break;
    }
    case text: {
        tp = CComAttrWidget::Text;
        break;
    }
    case blur: {
        tp = CComAttrWidget::MasicPen;
        break;
    }
    case cut: {
        tp = CComAttrWidget::Cut;
        break;
    }
    default:
        break;
    }

    // [0] 刷新顶部菜单栏属性显示
    if (dApp->topToolbar() != nullptr && dApp->topToolbar()->attributWidget() != nullptr)
        dApp->topToolbar()->attributWidget()->showByType(tp, pItem);

    // [1] 刷新点击工具栏后改变鼠标样式
    dApp->currentDrawScence()->changeMouseShape(mode);

}

EDrawToolMode CDrawParamSigleton::getCurrentDrawToolMode() const
{
    return m_currentDrawToolMode;
}

int CDrawParamSigleton::getRadiusNum() const
{
    return m_radiusNum;
}

void CDrawParamSigleton::setRadiusNum(int radiusNum)
{
    m_radiusNum = radiusNum;
}

int CDrawParamSigleton::getAnchorNum() const
{
    return m_anchorNum;
}

void CDrawParamSigleton::setAnchorNum(int anchorNum)
{
    m_anchorNum = anchorNum;
}

int CDrawParamSigleton::getSideNum() const
{
    return m_sideNum;
}

void CDrawParamSigleton::setSideNum(int sideNum)
{
    m_sideNum = sideNum;
}

QFont CDrawParamSigleton::getTextFont() const
{
    return m_textFont;
}

void CDrawParamSigleton::setTextFontFamily(const QString &strFont)
{
    m_textFont.setFamily(strFont);
}

QString CDrawParamSigleton::getTextFontStyle() const
{
    return m_textFont.styleName();
}

void CDrawParamSigleton::setTextFontStyle(const QString &style)
{
    m_textFont.setStyleName(style);
}

void CDrawParamSigleton::setShiftKeyStatus(bool flag)
{
    m_bShiftKeyPress = flag;
}

bool CDrawParamSigleton::getShiftKeyStatus()
{
    bool bRet = false;
    if (QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier) {
        bRet = true;
    }

    return bRet;
}

void CDrawParamSigleton::setAltKeyStatus(bool flag)
{
    m_bAltKeyPress = flag;
}

bool CDrawParamSigleton::getAltKeyStatus()
{
    bool bRet = false;
    if (QGuiApplication::queryKeyboardModifiers() & Qt::AltModifier) {
        bRet = true;
    }

    return bRet;
}

void CDrawParamSigleton::setCtlKeyStatus(bool flag)
{
    m_bCtlKeyPress = flag;
}

//bool CDrawParamSigleton::getCtlKeyStatus()
//{
//    bool bRet = false;
//    if (QGuiApplication::queryKeyboardModifiers() & Qt::ControlModifier) {
//        bRet = true;
//    }

//    return bRet;
//}

void CDrawParamSigleton::setScale(qreal scale)
{
    m_Scale = scale;
}

qreal CDrawParamSigleton::getScale() const
{
    return m_Scale;
}

void CDrawParamSigleton::setTextSize(qreal size)
{
    m_textFont.setPointSizeF(size);
}

qreal CDrawParamSigleton::getTextSize() const
{
    return m_textFont.pointSizeF();
}

void CDrawParamSigleton::setTextColor(const QColor &fillColor)
{
    m_textColor = fillColor;
}

QColor CDrawParamSigleton::getTextColor() const
{
    return m_textColor;
}

void CDrawParamSigleton::setTextColorAlpha(const int &alpha)
{
    m_textColor.setAlpha(alpha);
}

int CDrawParamSigleton::getTextColorAlpha() const
{
    return m_textColor.alpha();
}

QString CDrawParamSigleton::getDdfSavePath() const
{
    return m_ddfSavePath;
}

void CDrawParamSigleton::setDdfSavePath(const QString &ddfSavePath)
{
    m_ddfSavePath = ddfSavePath;
    CManageViewSigleton::GetInstance()->wacthFile(m_ddfSavePath);
}

ELineType CDrawParamSigleton::getLineStartType() const
{
    return m_lineStartType;
}

void CDrawParamSigleton::setLineStartType(const ELineType &lineType)
{
    m_lineStartType = lineType;
}

ELineType CDrawParamSigleton::getLineEndType() const
{
    return m_lineEndType;
}

void CDrawParamSigleton::setLineEndType(const ELineType &lineType)
{
    m_lineEndType = lineType;
}

ELineType CDrawParamSigleton::getPenStartType() const
{
    return m_penStartType;
}

void CDrawParamSigleton::setPenStartType(const ELineType &penType)
{
    m_penStartType = penType;
}

ELineType CDrawParamSigleton::getPenEndType() const
{
    return m_penEndType;
}

void CDrawParamSigleton::setPenEndType(const ELineType &penType)
{
    m_penEndType = penType;
}

ESaveDDFTriggerAction CDrawParamSigleton::getSaveDDFTriggerAction() const
{
    return m_saveDDFTriggerAction;
}

void CDrawParamSigleton::setSaveDDFTriggerAction(const ESaveDDFTriggerAction &saveDDFTriggerAction)
{
    m_saveDDFTriggerAction = saveDDFTriggerAction;
}

int CDrawParamSigleton::getRenderImage() const
{
    return m_renderImage;
}

void CDrawParamSigleton::setRenderImage(int renderImage)
{
    m_renderImage = renderImage;
}

//bool CDrawParamSigleton::getSelectAllFlag() const
//{
//    return m_bSelectAlling;
//}

//void CDrawParamSigleton::setSelectAllFlag(bool flag)
//{
//    m_bSelectAlling = flag;
//}

ECutType CDrawParamSigleton::getCutType() const
{
    return m_cutType;
}

void CDrawParamSigleton::setCutType(const ECutType &cutType)
{
    m_cutType = cutType;
}

QSize CDrawParamSigleton::getCutSize() const
{
    return m_cutSize;
}

void CDrawParamSigleton::setCutSize(const QSize &cutSize)
{
    m_cutSize = cutSize;
}

//bool CDrawParamSigleton::getImageAdjustScence() const
//{
//    return m_imageAdjustScence;
//}

void CDrawParamSigleton::setImageAdjustScence(const bool &adjust)
{
    m_imageAdjustScence = adjust;
}

//ERotationType CDrawParamSigleton::getImageFlipType() const
//{
//    return m_imageFlipType;
//}

void CDrawParamSigleton::setImageFlipType(const ERotationType &type)
{
    m_imageFlipType = type;
}

QSize CDrawParamSigleton::getCutDefaultSize() const
{
    return m_cutDefaultSize;
}

void CDrawParamSigleton::setCutDefaultSize(const QSize &cutSize)
{
    m_cutDefaultSize = cutSize;
}

ECutAttributeType CDrawParamSigleton::getCutAttributeType() const
{
    return m_cutAttributeType;
}

//void CDrawParamSigleton::setCutAttributeType(const ECutAttributeType &cutAttributeType)
//{
//    m_cutAttributeType = cutAttributeType;
//}


bool CDrawParamSigleton::getModify() const
{
    return m_isModify;
}

void CDrawParamSigleton::setModify(bool isModify)
{
    m_isModify = isModify;
}


EBlurEffect CDrawParamSigleton::getBlurEffect() const
{
    return m_effect;
}

void CDrawParamSigleton::setBlurEffect(const EBlurEffect &blurEffect)
{
    m_effect = blurEffect;
}

int CDrawParamSigleton::getBlurWidth() const
{
    return m_blurWidth;
}

void CDrawParamSigleton::setBlurWidth(const int width)
{
    m_blurWidth = width;
}

QString CDrawParamSigleton::viewName() const
{
    return m_viewName;
}

void CDrawParamSigleton::setViewName(QString name)
{
    m_viewName = name;
}

int CDrawParamSigleton::getRectXRedius() const
{
    return m_rectXRedius;
}

void CDrawParamSigleton::setRectXRedius(int redius)
{
    m_rectXRedius = redius;
}

QString CDrawParamSigleton::getShowViewNameByModifyState()
{
    //只有保存成文件了的，且和文件内容一致才显示原名 否则都加*
    if (!getModify() /*&& !getDdfSavePath().isEmpty()*/) {
        return viewName();
    }
    QString vName = "* " + viewName();
    return vName;
}

QString CDrawParamSigleton::uuid()
{
    return m_keyUUID;
}

QString CDrawParamSigleton::creatUUID()
{
    static int uuidKey = 0;
    QString uuid = QString("uuid_%1").arg(uuidKey);
    ++uuidKey;
    return uuid;
}

