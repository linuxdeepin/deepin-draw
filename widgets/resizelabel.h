#ifndef RESIZELABEL_H
#define RESIZELABEL_H

#include <DLabel>
#include <QPaintEvent>

#include "utils/shapesutils.h"

DWIDGET_USE_NAMESPACE

class ResizeLabel : public DLabel
{
    Q_OBJECT
public:
    ResizeLabel(QWidget *parent = 0);
    ~ResizeLabel();

public slots:
    void paintResizeLabel(bool drawing, FourPoints fpoints);

protected:
    void paintEvent(QPaintEvent *e);

private:
    bool m_drawArtboard;
    FourPoints m_artboardMPoints;

};

#endif // RESIZELABEL_H
