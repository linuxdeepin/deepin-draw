#ifndef TIPSLABEL_H
#define TIPSLABEL_H

#include <DLabel>
DWIDGET_USE_NAMESPACE
class TipsLabel : public DLabel
{
    Q_OBJECT
public:
    TipsLabel(DWidget *parent = 0);
    ~TipsLabel();

};

#endif // TIPSLABEL_H
