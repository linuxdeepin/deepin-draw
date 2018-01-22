#ifndef CUTIMAGETIPS_H
#define CUTIMAGETIPS_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>

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

class CutImageTips : public QLabel
{
    Q_OBJECT
public:
    CutImageTips(QWidget *parent = 0);
    ~CutImageTips();

    void showTips(QPointF pos, int newWidth);
    void setCutRation(CutRation ration);

signals:
    void canceled();
    void cutAction();
    void cutRationChanged(CutRation cutRation);

protected:
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent* e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* e) Q_DECL_OVERRIDE;

private:
    RationButton* m_cancelBtn;
    RationButton* m_okBtn;
    QButtonGroup* m_rationBtnGroup;
};

#endif // CUTIMAGETIPS_H
