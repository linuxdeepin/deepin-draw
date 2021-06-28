#ifndef CATTRIBUTEMANAGERWGT_H
#define CATTRIBUTEMANAGERWGT_H
#include "cattributeitemwidget.h"


#include <DIconButton>
#include <DBlurEffectWidget>


#include <QWidget>
#include <QVariant>

#include <QWidget>
#include <QMap>
#include <QLabel>
class QSpacerItem;
DWIDGET_USE_NAMESPACE

using namespace DrawAttribution;

namespace DrawAttribution {

class CAttributeManagerWgt: public CAttriBaseOverallWgt
{
    Q_OBJECT
public:
    CAttributeManagerWgt(QWidget *parent = nullptr);

    SAttrisList      attributions() const;
    void             setAttributions(const SAttrisList &attribution);
    void             changeAttribution(const SAttri &attri, bool update = true);

    QList<QWidget *> attributeWgts();

    QList<QVariant>  attributeVars();

    bool            isLogicAncestorOf(QWidget *w);

    static void installComAttributeWgt(int attri, QWidget *pWgt, const QVariant &defaultVar = QVariant());
    static void removeComAttributeWgt(int attri);

    static void setDefaultAttributionVar(int attri, const QVariant &defaultVar);
    static int  attrOfWidget(QWidget *pWgt);


    static QVariant defaultAttriVar(int attri);


    static void setWidgetRecommedSize(QWidget *pWgt, const QSize &sz);

signals:
    void attributionChanged(int attris, const QVariant &var, int phase = EChanged, bool autoCmdStack = true);
    void updateWgt(QWidget *pWgt, const QVariant &var);

private:
    void     ensureAttributions();
    QSize    attriWidgetRecommendedSize(QWidget *pWgt) override;
    void     setWidgetAttribution(QWidget *pWgt, const QVariant &var);

protected:
    void paintEvent(QPaintEvent *event) override;
//    void showEvent(QShowEvent *event) override;
//    void resizeEvent(QResizeEvent *event) override;

private:

    SAttrisList            _sAttributions;

    static QMap<int, QWidget *> s_allInstalledAttriWgts;
    static QMap<int, QVariant>  s_allInstalledDefaultVar;

    bool _dirty = false;
};
}

#endif // CATTRIBUTEMANAGERWGT_H
