#ifndef TIPSLABEL_H
#define TIPSLABEL_H

#include <QLabel>

class TipsLabel : public QLabel {
    Q_OBJECT
public:
    TipsLabel(QWidget* parent = 0);
    ~TipsLabel();

};

#endif // TIPSLABEL_H
