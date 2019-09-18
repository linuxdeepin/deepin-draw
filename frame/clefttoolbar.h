#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H

#include <DFrame>

class CCheckButton;
class QVBoxLayout;

DWIDGET_USE_NAMESPACE

class CLeftToolBar : public DFrame
{
    Q_OBJECT
public:
    explicit CLeftToolBar(DWidget *parent = nullptr);
    ~CLeftToolBar();

    void shortCutOperation(int type);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);                      //进入QWidget瞬间事件


signals:
    void setCurrentDrawTool(int type);
    void importPic();
    void signalBegainCut();

public slots:

    /**
     * @brief slotChangedStatusToSelect 将工具栏的状态切换为选择状态
     */
    void slotChangedStatusToSelect();
    void slotSetDisableButtons(bool);
    void slotQuitCutMode();

private:
    QList<CCheckButton *> m_actionButtons;
    CCheckButton *m_picBtn;
    CCheckButton *m_rectBtn;
    CCheckButton *m_roundBtn;
    CCheckButton *m_triangleBtn;
    CCheckButton *m_starBtn;
    CCheckButton *m_polygonBtn;
    CCheckButton *m_lineBtn;
    CCheckButton *m_penBtn;
    CCheckButton *m_textBtn;
    CCheckButton *m_blurBtn;
    CCheckButton *m_cutBtn;

    QVBoxLayout *m_layout;


private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CCheckButton *clickedButton);
    void initDrawTools();
};

#endif // RIGHTTOOLBAR_H
