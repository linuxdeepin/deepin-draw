#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H

#include "utils/baseutils.h"
#include "drawshape/drawtool.h"

#include <DFrame>

#include <QButtonGroup>

DWIDGET_USE_NAMESPACE

class CPushButton;
class QVBoxLayout;

class LeftToolBar : public DFrame
{
    Q_OBJECT
public:
    explicit LeftToolBar(QWidget *parent = nullptr);
    ~LeftToolBar();

signals:
    void setCurrentDrawTool(int type);

public slots:

private:
    QList<CPushButton *> m_actionButtons;
    CPushButton *m_picBtn;
    CPushButton *m_rectBtn;
    CPushButton *m_roundBtn;
    CPushButton *m_triangleBtn;
    CPushButton *m_starBtn;
    CPushButton *m_polygonBtn;
    CPushButton *m_lineBtn;
    CPushButton *m_penBtn;
    CPushButton *m_textBtn;
    CPushButton *m_blurBtn;
    CPushButton *m_cutBtn;

    QVBoxLayout *m_layout;
    MiddleWidgetStatus m_middleWidgetStatus;

private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CPushButton *clickedButton);
};

#endif // RIGHTTOOLBAR_H
