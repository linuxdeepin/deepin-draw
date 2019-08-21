#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H

#include "utils/global.h"
#include "utils/baseutils.h"
#include "drawshape/drawtool.h"

#include <DFrame>

DWIDGET_USE_NAMESPACE

class PushButton;
class QVBoxLayout;

class LeftToolBar : public DFrame
{
    Q_OBJECT
public:
    explicit LeftToolBar(QWidget *parent = nullptr);
    ~LeftToolBar();

signals:
    void sendPicPath(QStringList picPath);
    void setCurrentDrawTool(int type);

public slots:
    void importImage();
private:
    QList<PushButton *> m_actionButtons;
    PushButton *m_picBtn;
    PushButton *m_rectBtn;
    PushButton *m_roundBtn;
    PushButton *m_triangleBtn;
    PushButton *m_starBtn;
    PushButton *m_polygonBtn;
    PushButton *m_lineBtn;
    PushButton *m_penBtn;
    PushButton *m_textBtn;
    PushButton *m_blurBtn;
    PushButton *m_cutBtn;

    QVBoxLayout *m_layout;
    MiddleWidgetStatus m_middleWidgetStatus;

private:
    void initUI();
    void initConnection();
    void clearOtherSelections(PushButton *clickedButton);
};

#endif // RIGHTTOOLBAR_H
