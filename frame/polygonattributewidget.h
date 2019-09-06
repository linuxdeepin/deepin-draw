#ifndef POLYGONATTRIBUTEWIDGET_H
#define POLYGONATTRIBUTEWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLineEdit>

#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class BigColorButton;
class BorderColorButton;
class CSideWidthWidget;

class PolygonAttributeWidget : public DWidget
{
    Q_OBJECT
public:
    PolygonAttributeWidget(DWidget *parent = nullptr);
    ~PolygonAttributeWidget();

public slots:
    void updatePolygonWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalPolygonAttributeChanged();

private:
    BigColorButton *m_fillBtn;
    BorderColorButton *m_strokeBtn;
    CSideWidthWidget *m_sideWidthWidget;
    DSlider *m_sideNumSlider;
    DLineEdit *m_sideNumEdit;

    bool m_isUsrDragSlider; //是否是用户拖拽滑块

private:
    void initUI();
    void initConnection();
    QPoint getBtnPosition(const DPushButton *btn);
};


#endif // POLYGONATTRIBUTEWIDGET_H
