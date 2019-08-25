#ifndef CDRAWPARAMSIGLETON_H
#define CDRAWPARAMSIGLETON_H
#include "globaldefine.h"
#include <QtGlobal>
#include <QColor>
#include <QPen>
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

    void setFillFlag(bool flag);
    bool getFillFlag();

    void setFillColor(const QColor &fillColor);
    QColor getFillColor() const;

    QPen getPen() const;
    QBrush getBrush() const;

    void setCurrentDrawToolMode(EDrawToolMode mode);
    EDrawToolMode getCurrentDrawToolMode() const;

private:
    int m_nlineWidth;
    QColor m_sLineColor;

    bool m_bFillFlag;
    QColor m_nFillColor;

    EDrawToolMode m_currentDrawToolMode;
};


#endif // CDRAWPARAMSIGLETON_H
