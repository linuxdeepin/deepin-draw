#ifndef DRAWDIALOG_H
#define DRAWDIALOG_H

#include "dialog.h"

class DrawDialog : public Dialog
{
    Q_OBJECT
public:
    explicit DrawDialog(DWidget *parent = 0);

signals:
    void saveDrawImage();


protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
};

#endif // DRAWDIALOG_H
