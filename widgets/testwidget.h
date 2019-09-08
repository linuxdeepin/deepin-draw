#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <DWidget>
DWIDGET_USE_NAMESPACE

class TestWidget : public DWidget
{
    Q_OBJECT
public:
    explicit TestWidget(DWidget *parent = nullptr);

signals:

public slots:
};

#endif // TESTWIDGET_H
