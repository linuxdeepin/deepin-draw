/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef CMASICOTOOL_H
#define CMASICOTOOL_H
#include "idrawtool.h"

class JDynamicLayer;
class IBlurTool: public IDrawTool
{
    Q_OBJECT
public:
    IBlurTool(QObject *parent = nullptr);
    DrawAttribution::SAttrisList attributions() override;
    JDynamicLayer *desLayer(PageScene *pScene);

    QCursor cursor() const override;

protected:
    QAbstractButton *initToolButton() override;
    void  registerAttributionWidgets() override;

    void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    int  decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;
    bool isEnable(PageView *pView) override;

    int  allowedMaxTouchPointCount() override;
    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    static JDynamicLayer *sceneCurrentLayer(PageScene *scene);

private:
    QMap<PageScene *, JDynamicLayer *> _layers;
    QMap<JDynamicLayer *, qreal>        _saveZs;
};

#endif // CMASICOTOOL_H
