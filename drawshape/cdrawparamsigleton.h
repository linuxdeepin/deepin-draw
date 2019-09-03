#ifndef CDRAWPARAMSIGLETON_H
#define CDRAWPARAMSIGLETON_H
#include "globaldefine.h"
#include <QtGlobal>
#include <QColor>
#include <QPen>
#include <QFont>

class CDrawParamSigleton
{
private :
    static CDrawParamSigleton *m_pInstance;
    CDrawParamSigleton();

public :
    static CDrawParamSigleton *GetInstance();

public:
    void setLineWidth(int lineWidth);
    int getLineWidth() const;

    void setLineColor(const QColor &lineColor);
    QColor getLineColor() const;

    void setFillColor(const QColor &fillColor);
    QColor getFillColor() const;

    void setPen(const QPen &pen);
    QPen getPen() const;

    void setBrush(const QBrush &brush);
    QBrush getBrush() const;


    void setCurrentDrawToolMode(EDrawToolMode mode);
    EDrawToolMode getCurrentDrawToolMode() const;

    int getRadiusNum() const;
    void setRadiusNum(int radiusNum);

    int getAnchorNum() const;
    void setAnchorNum(int anchorNum);

    int getSideNum() const;
    void setSideNum(int sideNum);

    EPenType getPenType() const;
    void setPenType(const EPenType &penType);

    EPenType getCurrentPenType() const;
    void setCurrentPenType(const EPenType &currentPenType);

    QFont getTextFont() const;
    void setTextFont(const QFont &textFont);

    void setShiftKeyStatus(bool flag);
    bool getShiftKeyStatus();

    void setAltKeyStatus(bool flag);
    bool getAltKeyStatus();

private:
    int m_nlineWidth;
    QColor m_sLineColor;
    QColor m_nFillColor;

    int m_radiusNum; //多角星半径
    int m_anchorNum;//多角星锚点数

    int m_sideNum;//多边形边数

    EPenType m_currentPenType; //当前画笔类型

    QFont m_textFont; //文本字体

    EDrawToolMode m_currentDrawToolMode;

    //按键状态
    bool m_bShiftKeyPress;
    bool m_bAltKeyPress;
};


#endif // CDRAWPARAMSIGLETON_H
