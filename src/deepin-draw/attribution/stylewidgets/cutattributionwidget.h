#ifndef CUTATTRIBUTIONREGISTER_H
#define CUTATTRIBUTIONREGISTER_H
#include <QAbstractItemDelegate>
#include "iattributionregister.h"
#include <DToolButton>
class QLabel;
class CutAttributionWidget: public AttributeWgt
{
    Q_OBJECT
public:
    explicit CutAttributionWidget(DrawBoard *drawBoard, QWidget *parent = nullptr);
    void initUi();
    void initConnect();
private:
    QSize getCutSzie();
private:
    DrawBoard *m_drawBoard = nullptr;
    QList<QToolButton * > m_buttonList;
    CSpinBox *h_spinbox = nullptr;
    CSpinBox *w_spinbox = nullptr;
};

#endif // CUTATTRIBUTIONREGISTER_H
