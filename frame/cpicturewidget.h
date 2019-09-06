#ifndef CUTWIDGET_H
#define CUTWIDGET_H

#include <DWidget>

class CClickButton;

DWIDGET_USE_NAMESPACE

class CPictureWidget : public DWidget
{
public:
    enum EButtonType {
        LeftRotate,     //左旋转
        RightRotate,    //右旋转
        FlipHorizontal, //水平翻转
        FlipVertical    //垂直翻转
    };

    Q_OBJECT
public:
    CPictureWidget(DWidget *parent = nullptr);
    ~CPictureWidget();

signals:
    void signalBtnClick(int);

private:
    CClickButton *m_leftRotateBtn;
    CClickButton *m_rightRotateBtn;
    CClickButton *m_flipHBtn;
    CClickButton *m_flipVBtn;

private:
    void initUI();
    void initConnection();

};
#endif // CUTWIDGET_H
