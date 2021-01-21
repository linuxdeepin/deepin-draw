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
#ifndef CDRAWPARAMSIGLETON_H
#define CDRAWPARAMSIGLETON_H
#include "globaldefine.h"
#include <QtGlobal>
#include <QColor>
#include <QPen>
#include <QFont>

class CDrawParamSigleton
{
public:
    CDrawParamSigleton(const QString &uuid = "", bool isModified = false);

    void setLineWidth(int lineWidth);
    void setLineColor(const QColor &lineColor);

    void setPen(const QPen &pen);
    QPen getPen() const;

    void setBrush(const QBrush &brush);
    QBrush getBrush() const;

    void setCurrentDrawToolMode(EDrawToolMode mode, bool que = false);
    EDrawToolMode getCurrentDrawToolMode() const;

    int getRadiusNum() const;
    void setRadiusNum(int radiusNum);

    int getAnchorNum() const;
    void setAnchorNum(int anchorNum);

    int getSideNum() const;
    void setSideNum(int sideNum);

    QFont getTextFont() const;
    void setTextFontFamily(const QString &strFont);

    QString getTextFontStyle() const;
    void setTextFontStyle(const QString &style);

    void setShiftKeyStatus(bool flag);
    static bool getShiftKeyStatus();

    void setAltKeyStatus(bool flag);
//    static bool getAltKeyStatus();

    void setCtlKeyStatus(bool flag);

    void setScale(qreal scale);
    qreal getScale() const;

    void setTextSize(qreal size);
//    qreal getTextSize() const;

    void setTextColor(const QColor &fillColor);
    QColor getTextColor() const;

//    void setTextColorAlpha(const int &alpha);
//    int getTextColorAlpha() const;

    ECutType getCutType() const;
    void setCutType(const ECutType &cutType);

    QSize getCutSize() const;
    void setCutSize(const QSize &cutSize);

    void setImageAdjustScence(const bool &adjust);

    void setImageFlipType(const ERotationType &type);

    QSize getCutDefaultSize() const;
    void setCutDefaultSize(const QSize &cutSize);

    ECutAttributeType getCutAttributeType() const;

    bool isModified() const;
    void setModify(bool isModify);

    EBlurEffect getBlurEffect() const;
    void setBlurEffect(const EBlurEffect &blurEffect);

    int getBlurWidth() const;
    void setBlurWidth(const int width);

    QString getDdfSavePath() const;
    void setDdfSavePath(const QString &ddfSavePath);

    ELineType getLineStartType() const;
    void setLineStartType(const ELineType &lineType);

    ELineType getLineEndType() const;
    void setLineEndType(const ELineType &lineType);

    ELineType getPenStartType() const;
    void setPenStartType(const ELineType &penType);

    ELineType getPenEndType() const;
    void setPenEndType(const ELineType &penType);

    ESaveDDFTriggerAction getSaveDDFTriggerAction() const;
    void setSaveDDFTriggerAction(const ESaveDDFTriggerAction &saveDDFTriggerAction);

    int getRenderImage() const;
    void setRenderImage(int renderImage);

    /**
       * @brief viewName　获取视图名字
       * @return 返回视图名字
       */
    QString viewName() const;
    /**
        * @brief setViewName　设置视图名字
        * @param name 视图名字
        */
    void setViewName(QString name);

    /**
       * @brief getRectXRedius　获取矩形圆角半径
       * @return 矩形圆角半径
       */
    int getRectXRedius() const;
    /**
        * @brief setRectXRedius　设置矩形圆角半径
        * @param redius 矩形圆角半径
        */
    void setRectXRedius(int redius);

    /**
        * @brief getShowViewNameByModifyState　根据当前是否被修改的状态返回显示名字
        */
    QString getShowViewNameByModifyState() const;


    QString uuid() const;

public:
    static QString creatUUID();

private:
    int m_nlineWidth;
    QColor m_sLineColor;
    QColor m_nFillColor;
    int m_rectXRedius;//矩形圆角半径,默认x，y相同，后期有需要再添加y

    int m_radiusNum; //多角星半径
    int m_anchorNum;//多角星锚点数

    int m_sideNum;//多边形边数

    QFont m_textFont; //文本字体
    //qreal m_textSize; //文本大小
    QColor m_textColor;//文本颜色

    EDrawToolMode m_currentDrawToolMode;

    //按键状态
    bool m_bShiftKeyPress;
    bool m_bAltKeyPress;
    bool m_bCtlKeyPress;

    //当前比例尺
    qreal m_Scale;

    //cut
    ECutAttributeType m_cutAttributeType;
    ECutType m_cutType;
    QSize m_cutSize;
    QSize m_cutDefaultSize;

    //图元是否有修改
    bool m_isModify;
    //保存DDF文件的触发者
    ESaveDDFTriggerAction m_saveDDFTriggerAction;
    //DDF文件保存路径
    QString m_ddfSavePath;

    //blur
    EBlurEffect m_effect; //模糊效果
    int m_blurWidth;    //模糊半径

    // 图片自适应场景、翻转(旋转由图元本身控制)
    bool m_imageAdjustScence;
    ERotationType m_imageFlipType;

    //线型
    ELineType m_lineStartType; // 线起点样式
    ELineType m_lineEndType; // 线终点样式

    // 画笔类型
    ELineType m_penStartType; // 画笔起点样式
    ELineType m_penEndType; // 画笔终点样式

    int m_renderImage; //是否是将场景渲染到图片上　0否　1是　2是且渲染为透明

    bool m_bSelectAlling; //是否正在全选

    QString m_viewName;//视图名字

    QString m_keyUUID;      //唯一标识
};


#endif // CDRAWPARAMSIGLETON_H
