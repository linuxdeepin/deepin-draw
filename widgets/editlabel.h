#ifndef EDITLABEL_H
#define EDITLABEL_H

#include <DLabel>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

class EditLabel : public DLabel
{
    Q_OBJECT
public:
    EditLabel(DWidget *parent = 0);
    ~EditLabel();

    void setTitle(QString title);
    void setEditText(QString text);
    void setEditWidth(int width);
    void setTitleSpacing(int spacing);
    QString editText();

signals:
    void editTextChanged(QString text);

private:
    DLabel *m_titleLabel;
    DLineEdit *m_edit;
    int m_titleSpacing;
};

#endif // EDITLABEL_H
