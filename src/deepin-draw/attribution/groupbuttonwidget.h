#ifndef CGROUPBUTTONWGT_H
#define CGROUPBUTTONWGT_H
#include <QPushButton>
#include "attributewidget.h"

class GroupButtonWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit GroupButtonWidget(QWidget *parent = nullptr);

    void setGroupFlag(bool canGroup, bool canUngroup);

signals:
    void buttonClicked(bool doGroup, bool doUngroup);

protected:
    virtual void paintEvent(QPaintEvent *event)override;
private:
    DIconButton *groupButton;
    DIconButton *unGroupButton;

    //QLabel *_labelGroup;
    //QLabel *_labelUngroup;
    QLabel *m_titleLabel;
};

#endif // CGROUPBUTTONWGT_H
