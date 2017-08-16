#ifndef CUTIMAGETIPS_H
#define CUTIMAGETIPS_H

#include <QDialog>
#include <QLabel>

class CutImageTips : public QDialog
{
    Q_OBJECT
public:
    CutImageTips(QWidget *parent = 0);
    ~CutImageTips();

    void showTips(QPoint pos);

protected:
    void paintEvent(QPaintEvent *e);

private:
    QLabel* m_settingTips;
};

#endif // CUTIMAGETIPS_H
