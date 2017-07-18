#ifndef SEPERATORLINE_H
#define SEPERATORLINE_H

#include <QLabel>

class SeperatorLine : public QLabel {
    Q_OBJECT
public:
    SeperatorLine(QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(1, 20);
        setStyleSheet("border: 1px solid rgba(0, 0, 0, 0.3);");
    }
    ~SeperatorLine() {}

};
#endif // SEPERATORLINE_H
