#ifndef FONTSIZELINEEDIT_H
#define FONTSIZELINEEDIT_H

#include <QLineEdit>
#include <QShortcut>
#include <QKeyEvent>
#include <QDebug>

class FontsizeLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    FontsizeLineEdit(QWidget* parent = 0) {
        Q_UNUSED(parent);
    }
    ~FontsizeLineEdit(){}

signals:
    void addSize();
    void reduceSize();

protected:
    void keyPressEvent(QKeyEvent* e)
    {
        if (e->key() == Qt::Key_Up)
        {
            emit addSize();
        } else if (e->key() == Qt::Key_Down)
        {
            emit reduceSize();
        }

        QLineEdit::keyPressEvent(e);
    }

};
#endif // FONTSIZELINEEDIT_H
