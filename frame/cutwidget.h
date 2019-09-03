#ifndef CUTWIDGET_H
#define CUTWIDGET_H

#include <QWidget>

#include "widgets/pushbutton.h"

class CutWidget : public QWidget
{
    Q_OBJECT
public:
    CutWidget(QWidget *parent = 0);
    ~CutWidget();

signals:
    void rotateImage(int degree);
    void cutImage();
    void mirroredImage(bool hor, bool ver);
    void rotateLeftOrRight(bool leftOrRight);

public slots:
    void updateBtns(const QString &path);
    void cutImageBtnReset();
    bool cuttingStatus();
    void mirrorHor();
    void mirrorVer();
    void rotateLeft();
    void rotateRight();

private:
    void initConnect();

    PushButton *m_leftRotateBtn;
    PushButton *m_rightRotateBtn;
    PushButton *m_cutBtn;
    PushButton *m_flipHBtn;
    PushButton *m_flipVBtn;
};
#endif // CUTWIDGET_H
