#ifndef EDITLABEL_H
#define EDITLABEL_H

#include <QLabel>
#include <QLineEdit>

class EditLabel : public QWidget
{
    Q_OBJECT
public:
    EditLabel(QWidget* parent = 0);
    ~EditLabel();

    void setTitle(QString title);
    void setEditText(QString text);
    void setEditWidth(int width);

private:
    QLabel* m_titleLabel;
    QLineEdit* m_edit;

};

#endif // EDITLABEL_H
