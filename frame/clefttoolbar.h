#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H

#include <DFrame>



class CPushButton;
class QVBoxLayout;

DWIDGET_USE_NAMESPACE

class CLeftToolBar : public DFrame
{
    Q_OBJECT
public:
    explicit CLeftToolBar(QWidget *parent = nullptr);
    ~CLeftToolBar();

signals:
    void sendPicPath(QStringList picPath);
    void setCurrentDrawTool(int type);

public slots:
    void importImage();

private:
    QList<CPushButton *> m_actionButtons;
    CPushButton *m_picBtn;
    CPushButton *m_selectBtn;
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


private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CPushButton *clickedButton);
    void initDrawTools();
};

#endif // RIGHTTOOLBAR_H
