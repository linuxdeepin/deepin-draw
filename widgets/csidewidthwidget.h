#ifndef CSIDEWIDTHWIDGET_H
#define CSIDEWIDTHWIDGET_H

#include <DFrame>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE


class CPushButton;

class CSideWidthWidget : public DFrame
{
public:
    enum CLineWidth {
        Finer = 1,
        Fine = 2,
        Medium = 4,
        Bold = 8
    };

    Q_OBJECT
public:
    explicit CSideWidthWidget(QWidget *parent = nullptr);

signals:
    void signalSideWidthChange(int);
public slots:
    void slotSideWidthChange(int);

private:
    CPushButton *m_finerButton;
    CPushButton *m_fineButton;
    CPushButton *m_mediumButton;
    CPushButton *m_boldButton;

    QHBoxLayout *m_layout;
    QMap<CPushButton *, CLineWidth> m_buttonMap;

private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CPushButton *clickedButton);
};

#endif // CSIDEWIDTHWIDGET_H
