#ifndef SEPERATORLINE_H
#define SEPERATORLINE_H

#include <DLabel>
DWIDGET_USE_NAMESPACE
class SeperatorLine : public DLabel
{
    Q_OBJECT
public:
    SeperatorLine(DWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setFixedSize(1, 18);
        //setStyleSheet("border: 1px solid rgba(0, 0, 0, 0.1);");
    }
    ~SeperatorLine() {}

};
#endif // SEPERATORLINE_H
