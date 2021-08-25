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
#ifndef CPENTOOL_H
#define CPENTOOL_H
#include "idrawtool.h"
#include <QUndoCommand>
#include <QPicture>
#include <QPointer>
#include "cgraphicslayer.h"

class CPenTool : public IDrawTool
{
    Q_OBJECT
public:
    enum EPenToolType {ENormalPen = 1, ECalligraphyPen, ECrayonPen, ETempErase};

    CPenTool();

    virtual ~CPenTool() override;

    DrawAttribution::SAttrisList attributions() override;

    QCursor cursor() const override;
protected:

    /**
     * @brief toolButton 定义工具的激活按钮
     */
    QAbstractButton *initToolButton() override;

    void  registerAttributionWidgets() override;

    CGraphicsItem *creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　工具开始事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;


    /**
     * @brief toolStart　判断工具活跃类型
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    int decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　 工具刷新事件
     * @param event       当次事件信息
     * @param pInfo       记录信息
     */
    void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　工具结束事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief allowedMaxTouchPointCount 允许的最大支持实时绘制点数
     */
    int  allowedMaxTouchPointCount() override;

    /**
     * @brief returnToSelectTool 工具执行的结束
     * @param event 事件
     * @param pInfo 额外信息
     */
    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;

    /**
     * @brief minMoveUpdateDistance　创建图元时最小移动距离
     */
    int minMoveUpdateDistance() override;

    /**
     * @brief pictureColorChanged　改变图片颜色(不含透明区域)
     */
    QPixmap pictureColorChanged(const QImage &image, const QColor &color);

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    bool autoSupUndoForCreatItem() override {return false;}
protected:
    QPen     getViewDefualtPen(PageView *view) const;
    QBrush   getViewDefualtBrush(PageView *view) const;

    QPicture paintNormalPen(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo);
    QPicture paintCalligraphyPen(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo);
    QPicture paintCrayonPen(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo, qreal space = 8);
    QPicture paintTempErasePen(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo);

    void paintPictureToView(const QPicture &picture, PageView *view);

protected:
    QMap<PageScene *, JDynamicLayer *> _layers;
    //JDynamicLayer *_layer = nullptr;
    bool _isNewLayer = false;
    QMap<int, JActivedPaintInfo> _activePictures;
    QComboBox *m_pPenStyleComboBox = nullptr;
    QImage m_pRenderImage;
    QPointF m_prePos;
};


#endif // CPENTOOL_H
