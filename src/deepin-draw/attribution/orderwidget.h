#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H

#include "attributewidget.h"

class Page;
class DrawBoard;
class PageScene;

class OrderWidget : public AttributeWgt
{
    Q_OBJECT
public:
    OrderWidget(DrawBoard *drawBoard, QWidget *parent = nullptr);
private:
    void initUi();
    void initConnect();
    DIconButton *createIconButton(const QString &icon, const QString &toolTip, const QString &accesibleName);
    void doAlignment(Qt::AlignmentFlag align);
    void updateAlignButtonStatus();
    void updateLayerButtonStatus();
    void updateButtonsStatus();
private:
    DIconButton *m_oneLayerUp = nullptr;      //向上一层
    DIconButton *m_oneLayerDown = nullptr;    //向下一层
    DIconButton *m_bringToFront = nullptr;    //置于最顶层
    DIconButton *m_sendToback = nullptr;      //置于最底层

    DIconButton *m_LeftAlign = nullptr;     //左对齐
    DIconButton *m_HCenterAlign = nullptr;  //水平居中对齐
    DIconButton *m_RightAlign = nullptr;    //右对齐
    DIconButton *m_TopAlign = nullptr;      //顶对齐
    DIconButton *m_VCenterAlign = nullptr;  //垂直居中对齐
    DIconButton *m_BottomAlign = nullptr;   //底对齐

    DIconButton *m_HEqulSpaceAlign = nullptr; //水平等间距对齐
    DIconButton *m_VEqulSpaceAlign = nullptr; //垂直等间距对齐
    DrawBoard   *m_drawBoard = nullptr;
    Page        *m_currentPage = nullptr;
    PageScene   *m_currentScene = nullptr;
};

#endif // ORDERWIDGET_H
