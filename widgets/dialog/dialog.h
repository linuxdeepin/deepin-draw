#ifndef DIALOG_H
#define DIALOG_H

#include <DDialog>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class Dialog : public DDialog
{
    Q_OBJECT
public:
    explicit Dialog(DWidget *parent = 0);
    void showInCenter(DWidget *w);
};

#endif // DIALOG_H
