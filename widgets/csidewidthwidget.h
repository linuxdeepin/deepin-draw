#ifndef CSIDEWIDTHWIDGET_H
#define CSIDEWIDTHWIDGET_H

#include <DFrame>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE


class CCheckButton;

class CSideWidthWidget : public DFrame
{
public:
    enum CLineWidth {
        Finer = 2,
        Fine = 4,
        Medium = 6,
        Bold = 8
    };

    Q_OBJECT
public:
    explicit CSideWidthWidget(DWidget *parent = nullptr);

signals:
    void signalSideWidthChange();
public:
    void updateSideWidth();

private:
    CCheckButton *m_finerButton;
    CCheckButton *m_fineButton;
    CCheckButton *m_mediumButton;
    CCheckButton *m_boldButton;

    QHBoxLayout *m_layout;
    QMap<CCheckButton *, CLineWidth> m_buttonMap;

private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CCheckButton *clickedButton);
};

#endif // CSIDEWIDTHWIDGET_H
