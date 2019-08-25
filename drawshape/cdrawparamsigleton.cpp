#include "cdrawparamsigleton.h"

CDrawParamSigleton *CDrawParamSigleton::m_pInstance = nullptr;

CDrawParamSigleton::CDrawParamSigleton()
    : m_nlineWidth(2)
    , m_sLineColor(Qt::black)
    , m_bFillFlag(false)
    , m_nFillColor(Qt::black)
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

void CDrawParamSigleton::setFillFlag(bool flag)
{
    m_bFillFlag = flag;
}

bool CDrawParamSigleton::getFillFlag()
{
    return m_bFillFlag;
}

void CDrawParamSigleton::setFillColor(const QColor &fillColor)
{
    m_nFillColor = fillColor;
}

QColor CDrawParamSigleton::getFillColor() const
{
    return m_nFillColor;
}

QPen CDrawParamSigleton::getPen() const
{
    QPen pen;
    pen.setWidth(m_nlineWidth);
    pen.setColor(m_sLineColor);

    return pen;
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
