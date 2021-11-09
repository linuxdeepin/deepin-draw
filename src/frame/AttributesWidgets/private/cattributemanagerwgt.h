/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: ZhangYong <zhangyong@uniontech.com>
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
    explicit CAttributeManagerWgt(QWidget *parent = nullptr);

    SAttrisList      attributions() const;
    void             setAttributions(const SAttrisList &attribution);
    void             changeAttribution(const SAttri &attri, bool update = true);

    QList<QWidget *> attributeWgts();

    QList<QVariant>  attributeVars();

    bool            isLogicAncestorOf(QWidget *w);

    void installComAttributeWgt(int attri, QWidget *pWgt, const QVariant &defaultVar = QVariant());
    void removeComAttributeWgt(int attri);

    void setDefaultAttributionVar(int attri, const QVariant &defaultVar);
    int  attrOfWidget(QWidget *pWgt);
    QWidget *widgetOfAttr(int attri);


    QVariant defaultAttriVar(int attri);


    void setWidgetRecommedSize(QWidget *pWgt, const QSize &sz);

signals:
    void attributionChanged(int attris, const QVariant &var, int phase = EChanged, bool autoCmdStack = true);
    void updateWgt(QWidget *pWgt, const QVariant &var);

private:
    void     ensureAttributions();
    QSize    attriWidgetRecommendedSize(QWidget *pWgt) override;
    void     setWidgetAttribution(QWidget *pWgt, const QVariant &var);
    void     onAttriWidgetValueChanged(const QVariant var, int phase);

protected:
    void paintEvent(QPaintEvent *event) override;

private:

    SAttrisList            _sAttributions;

    QMap<int, QWidget *> s_allInstalledAttriWgts;
    QMap<int, QVariant>  s_allInstalledDefaultVar;

    bool _dirty = false;
};
}

#endif // CATTRIBUTEMANAGERWGT_H
