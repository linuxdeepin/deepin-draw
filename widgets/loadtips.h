#ifndef LOADTIPS_H
#define LOADTIPS_H

#include <QDialog>
#include <QLabel>
#include <DWaterProgress>

DWIDGET_USE_NAMESPACE

class LoadTips : public QDialog {
    Q_OBJECT
public:
    LoadTips(QWidget* parent = 0);
    ~LoadTips();

public:
    void showTips();

signals:
    void progressValueChanged(int value);
    void finishedPainting();

private:
    DWaterProgress* m_waterProg;
    QLabel* m_messageLabel;
    int m_counts;
};
#endif // LOADTIPS_H
