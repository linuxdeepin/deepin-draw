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
#ifndef ATTRIBUTEITEMWIDGET_H
#define ATTRIBUTEITEMWIDGET_H

#include <QWidget>
#include <QMap>
#include <QLabel>
#include <QDebug>

#include <QPushButton>
#include <DComboBox>
#include <DSpinBox>
#include <DIconButton>
#include <DBlurEffectWidget>
#include <QMetaType>
#include "globaldefine.h"


class QSpacerItem;
class QHBoxLayout;

class CSpinBox;
class SeperatorLine;
class CColorPickWidget;
DWIDGET_USE_NAMESPACE

Q_DECLARE_METATYPE(QMargins)
namespace DrawAttribution {

//所有图元都可以拥有的属性：画笔颜色、画笔宽度、填充颜色
enum EComAttri {
    ETitle,

    EBrushColor,
    EPenColor,
    EBorderWidth,  //可设置为0px
    EPenStyle,

    EPenWidth,    //不支持0px

    ERectRadius,

    EStarAnchor,
    EStarInnerOuterRadio,

    EPolygonSides,

    EStreakBeginStyle,
    EStreakEndStyle,

    EFontColor,
    EFontFamily,
    EFontWeightStyle,
    EFontSize,

    EImageLeftRot,
    EImageRightRot,
    EImageHorFilp,
    EImageVerFilp,
    EImageAdaptScene,

    ESeparator,
    EGroupWgt,

    EPictureAttri,
    EBlurAttri,
    EFullFontAttri,
    ECutToolAttri,
    EEraserWidth,

    EUserAttri = 1000
};

#define AttriWidgetReWidth "recommendedSize"
#define AttriWidgetVar "variant"

#define WidgetShowInHorBaseWidget "WidgetShowInHorScollor"
#define WidgetShowInVerWindow   "WidgetShowInVerWindow"
#define WidgetMarginInVerWindow "WidgetMarginInVerWindow"
#define WidgetAlignInVerWindow  "WidgetAlignInVerWindow"

struct SAttri {
    int      attri = -1;
    QVariant var;
    SAttri(int atr = -1, const QVariant &vr = QVariant()): attri(atr), var(vr) {}
    bool operator < (const SAttri &other)
    {
        return (attri < other.attri);
    }
    bool operator == (const SAttri &other)
    {
        return (attri == other.attri && var == other.var);
    }

    friend QDebug operator<<(QDebug debug, const SAttri &c)
    {
        QDebugStateSaver saver(debug);
        debug.nospace() << '(' << c.attri << ", " << c.var << ')';

        return debug;
    }
};

class SAttrisList: public QList<SAttri>
{
public:
    SAttrisList(const QList<int> &list = QList<int>());

    SAttrisList insected(const SAttrisList &other);

    bool        haveAttribution(int attri);
};


class CAttributeWgt: public QWidget
{
    Q_OBJECT
public:
    CAttributeWgt(int attri = -1, QWidget *parent = nullptr);

    void setAttribution(int arrti);
    int  attribution();


    virtual void  setVar(const QVariant &var);
    virtual QSize recommendedSize()const;

signals:
    void attriChanged(const QVariant var, int phase = EChanged);

protected:
    bool event(QEvent *e) override;

private:
    int _attri = -1;
};

class CExpButton: public QWidget
{
    Q_OBJECT
public:
    explicit CExpButton(QWidget *parent = nullptr);
signals:
    void clicked();

private:
    DIconButton   *_expBtn  = nullptr;
    QLabel *_sepLine = nullptr;
};

class CSpline: public QFrame
{
    Q_OBJECT
public:
    explicit CSpline(QWidget *parent = nullptr);
};


class CExpWgt: public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit CExpWgt(QWidget *parent = nullptr);

    void addWidget(QWidget *pWidget);

    //void setWidgets(const QList<QWidget *> &widgets);

    void clear();

    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    static bool widgetShowInWindow(const QWidget *w);
    static QMargins widgetMarginInWindow(const QWidget *w);
    static Qt::Alignment widgetAlignInWindow(const QWidget *w);
private:
    void clearChildLayout();
    QHBoxLayout *packageWidget(QWidget *pWidget, const QMargins &margins, Qt::Alignment align = Qt::AlignLeft);

private:
    QLayout *_pCenterLay = nullptr;
    QList<QWidget *> _widgets;
    QList<CSpline *> _splines;
};

class CAttriBaseOverallWgt: public CAttributeWgt
{
    Q_OBJECT
public:
    explicit CAttriBaseOverallWgt(QWidget *parent = nullptr);

    void     hideExpWindow();
    QLayout *centerLayout();

    void addWidget(QWidget *w);

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *e) override;

protected:
    QSize recommendedSize()const override;
    static void setWidgetAllPosterityFocus(QWidget *pW);
    //static bool widgetShowInBaseOverallWgt(const QWidget *w);
protected:
    virtual QSize attriWidgetRecommendedSize(QWidget *pw);


    CExpWgt     *getExpsWidget();
    CExpButton  *getExpButton();

    void     autoResizeUpdate();
    virtual int  totalNeedWidth();

    QLayout *_pCenterLay = nullptr;
    QSpacerItem *_leftSpacer = nullptr;
    QSpacerItem *_rightSpacer = nullptr;
    CExpWgt     *_pExpWidget = nullptr;
    CExpButton  *_expBtn = nullptr;

    QList<QWidget *>       _allWgts;

    friend class CAttributeManagerWgt;
};


//可用于颜色相关的控件如EPenColor，EBrushColor，EFontColor
class CColorSettingButton: public CAttributeWgt
{
    Q_OBJECT

public:
    enum EColorFill {EFillArea, EFillBorder};

    CColorSettingButton(const QString &text = "color",
                        QWidget *parent = nullptr, bool autoConnect = true);


    void  setVar(const QVariant &var) override;

    CColorPickWidget *colorPick();

signals:
    void colorChanged(const QColor &color, int phase = EChanged);

public slots:
    void setColor(const QColor &color, int phase = EChanged);
    void setDefaultButtonSize(const QSize &sz);
    void setColorFill(EColorFill fillPos);

protected:
    void  paintEvent(QPaintEvent *event) override;
    void  mousePressEvent(QMouseEvent *event) override;
public:

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    void paintFillArea(QPainter *painter);
    void paintFillBorder(QPainter *painter);

private:
    QString     _text;
    QColor      _color;
    EColorFill  _fillPos = EFillArea;
    QSize       _defaultButtonSize = QSize(20, 20);
    const int   _space = 6;

    bool        _connectedColorPicker = false;
};

class CComBoxSettingWgt: public CAttributeWgt
{
    Q_OBJECT
public:
    CComBoxSettingWgt(const QString &text = "", QWidget *parent = nullptr);

    void setText(const QString &text);

    QComboBox *comboBox();

    void setComboBox(QComboBox *pBox);

//private:
//    Q_SLOT void onCurrentChanged(int index);

private:
    QComboBox *_comBox = nullptr;
    QLabel    *_lab    = nullptr;
};

class CSpinBoxSettingWgt: public CAttributeWgt
{
    Q_OBJECT

public:
    CSpinBoxSettingWgt(const QString &text = "", QWidget *parent = nullptr);

    void  setVar(const QVariant &var) override;

    CSpinBox *spinBox();

    QSize recommendedSize()const override;

signals:
    void widthChanged(int width);

private:
    CSpinBox *_spinBox = nullptr;
    QLabel   *_lab     = nullptr;
};

class CAttributeManagerWgt;
class CGroupButtonWgt: public CAttributeWgt
{
    Q_OBJECT
public:
    explicit CGroupButtonWgt(QWidget *parent = nullptr);

    QSize recommendedSize() const override;

    //CAttributeManagerWgt *managerParent();

    //int parentChildCount();

    void setGroupFlag(bool canGroup, bool canUngroup);

signals:
    void buttonClicked(bool doGroup, bool doUngroup);

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    DIconButton *groupButton;
    DIconButton *unGroupButton;

    QPushButton *expGroupBtn;
    QPushButton *expUnGroupBtn;

    QLabel *_labelGroup;
    QLabel *_labelUngroup;

    //SeperatorLine *sepLine;
};

}
#endif // ATTRIBUTEITEMWIDGET_H
