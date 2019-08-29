#include "cdrawparamsigleton.h"

CDrawParamSigleton *CDrawParamSigleton::m_pInstance = nullptr;

CDrawParamSigleton::CDrawParamSigleton()
    : m_nlineWidth(2)
    , m_sLineColor(Qt::black)
    , m_nFillColor(Qt::transparent)
    , m_radiusNum(100)
    , m_anchorNum(5)
    , m_sideNum(4)
    , m_currentPenType(EPenType::straight)
    , m_currentDrawToolMode(selection)
{

}

CDrawParamSigleton *CDrawParamSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new CDrawParamSigleton();
    }
    return m_pInstance;
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

void CDrawParamSigleton::setFillColor(const QColor &fillColor)
{
    m_nFillColor = fillColor;
}

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

EPenType CDrawParamSigleton::getCurrentPenType() const
{
    return m_currentPenType;
}

void CDrawParamSigleton::setCurrentPenType(const EPenType &currentPenType)
{
    m_currentPenType = currentPenType;
}

QFont CDrawParamSigleton::getTextFont() const
{
    return m_textFont;
}

void CDrawParamSigleton::setTextFont(const QFont &textFont)
{
    m_textFont = textFont;
}


