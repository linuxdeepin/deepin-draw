#ifndef RESIZELABEL_H
#define RESIZELABEL_H

#include <QLabel>
#include <QPaintEvent>

#include "utils/shapesutils.h"

class ResizeLabel : public QLabel
{
    Q_OBJECT
public:
    ResizeLabel(QWidget* parent = 0);
    ~ResizeLabel();

public slots:
    void paintResizeLabel(bool drawing, FourPoints fpoints);

protected:
    void paintEvent(QPaintEvent* e);

private:
    bool m_drawArtboard;
    FourPoints m_artboardMPoints;

};

#endif // RESIZELABEL_H
