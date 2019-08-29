#ifndef POLYGONALSTARATTRIBUTEWIDGET_H
#define POLYGONALSTARATTRIBUTEWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLineEdit>
#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class BigColorButton;
class BorderColorButton;
class CSideWidthWidget;

class PolygonalStarAttributeWidget : public DWidget
{
    Q_OBJECT
public:
    PolygonalStarAttributeWidget(QWidget *parent = nullptr);
    ~PolygonalStarAttributeWidget();

public slots:
    void updatePolygonalStarWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalPolygonalStarAttributeChanged();

private:
    BigColorButton *m_fillBtn;
    BorderColorButton *m_strokeBtn;
    CSideWidthWidget *m_sideWidthWidget;
    DSlider *m_anchorNumSlider; //锚点数滑块
    DLineEdit *m_anchorNumEdit;//锚点数编辑框
    DSlider *m_radiusNumSlider;//半径滑块
    DLineEdit *m_radiusNumEdit;//半径编辑框

    bool m_isUsrDragSlider; //是否是用户拖拽滑块

private:
    void initUI();
    void initConnection();
};

#endif // POLYGONALSTARATTRIBUTEWIDGET_H
