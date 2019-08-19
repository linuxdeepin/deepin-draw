#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H


//#include "widgets/arrowrectangle.h"
#include "utils/global.h"
#include "utils/baseutils.h"
#include <dtkwidget_global.h>
#include "drawshape/drawtool.h"

#include <QFrame>

DWIDGET_USE_NAMESPACE

class PushButton;
class QVBoxLayout;

class LeftToolBar : public QFrame
{
    Q_OBJECT
public:
    explicit LeftToolBar(QWidget *parent = nullptr);
    ~LeftToolBar();

signals:
    void setCurrentDrawTool(int type);

public slots:

private:
    QMap<PushButton*,bool> m_actionPushButtons;
    PushButton* m_rectBtn;
    PushButton* m_roundBtn;
    PushButton* m_triangleBtn;
    PushButton* m_starBtn;
    PushButton* m_polygonBtn;
    PushButton* m_lineBtn;
    PushButton* m_penBtn;
    PushButton* m_textBtn;
    PushButton* m_blurBtn;
    PushButton* m_cutBtn;

    QVBoxLayout* m_layout;
    MiddleWidgetStatus m_middleWidgetStatus;

private:
    void initUI();
    void initConnection();
    void clearOtherSelection();
};

#endif // RIGHTTOOLBAR_H
