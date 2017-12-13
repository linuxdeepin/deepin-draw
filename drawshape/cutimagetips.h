#ifndef CUTIMAGETIPS_H
#define CUTIMAGETIPS_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include "utils/baseutils.h"

class RationButton : public QPushButton
{
    Q_OBJECT
public:
    RationButton(QWidget* parent = 0)
    {
        Q_UNUSED(parent);
        setFixedSize(30, 24);
        setCheckable(true);
    }
    ~RationButton(){}
};

class CutImageTips : public QDialog
{
    Q_OBJECT
public:
    CutImageTips(QWidget *parent = 0);
    ~CutImageTips();

    void showTips(QPoint pos);
    void setCutRation(CutRation ration);

signals:
    void canceled();
    void cutAction();
    void cutRationChanged(CutRation cutRation);

protected:
    void paintEvent(QPaintEvent *e);

private:
    RationButton* m_cancelBtn;
    RationButton* m_okBtn;
    QButtonGroup* m_rationBtnGroup;
};

#endif // CUTIMAGETIPS_H
