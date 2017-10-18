#ifndef CUTWIDGET_H
#define CUTWIDGET_H

#include <QWidget>

#include "widgets/pushbutton.h"

class CutWidget : public QWidget
{
    Q_OBJECT
public:
    CutWidget(QWidget* parent = 0);
    ~CutWidget();

signals:
    void rotateImage(int degree);
    void cutImage();
    void mirroredImage(bool hor, bool ver);

public slots:
    void  updateBtns(const QString &path);
    void  cutImageBtnReset();

private:
    PushButton* m_leftRotateBtn;
    PushButton* m_rightRotateBtn;
    PushButton* m_cutBtn;
    PushButton* m_flipHBtn;
    PushButton* m_flipVBtn;
};
#endif // CUTWIDGET_H
