#include "cdrawparamsigleton.h"

CDrawParamSigleton *CDrawParamSigleton::m_pInstance = nullptr;

CDrawParamSigleton::CDrawParamSigleton()
    : m_nlineWidth(2)
    , m_sLineColor(Qt::black)//black
    , m_nFillColor(Qt::transparent)//transparent
    , m_radiusNum(50)
    , m_anchorNum(5)
    , m_sideNum(5)
    , m_currentPenType(EPenType::straight)
    , m_textFont(QFont())
    , m_textColor(Qt::black)
    , m_currentDrawToolMode(selection)
    , m_bShiftKeyPress(false)
    , m_bAltKeyPress(false)
    , m_bCtlKeyPress(false)
    , m_Scale(1)
    , m_cutAttributeType(ECutAttributeType::NoneAttribute)
    , m_cutType(ECutType::cut_original)
{
    m_textFont.setPointSizeF(8);
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

void CDrawParamSigleton::setTextFont(const QString &strFont)
{
    m_textFont.setFamily(strFont);
}

void CDrawParamSigleton::setShiftKeyStatus(bool flag)
{
    m_bShiftKeyPress = flag;
}

bool CDrawParamSigleton::getShiftKeyStatus()
{
    return m_bShiftKeyPress;
}

void CDrawParamSigleton::setAltKeyStatus(bool flag)
{
    m_bAltKeyPress = flag;
}

bool CDrawParamSigleton::getAltKeyStatus()
{
    return m_bAltKeyPress;
}

void CDrawParamSigleton::setCtlKeyStatus(bool flag)
{
    m_bCtlKeyPress = flag;
}

bool CDrawParamSigleton::getCtlKeyStatus()
{
    return m_bCtlKeyPress;
}

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

void CDrawParamSigleton::setCutAttributeType(const ECutAttributeType &cutAttributeType)
{
    m_cutAttributeType = cutAttributeType;
}


