#ifndef TEXTFONTLABEL_H
#define TEXTFONTLABEL_H

#include <QLabel>
#include <QLineEdit>
#include "toolbutton.h"

class TextFontLabel : public QLabel
{
    Q_OBJECT
public:
    TextFontLabel(QWidget* parent = 0);
    ~TextFontLabel();

signals:
    void textFontsizeChanged(int fontsize);

private:
    int m_fontsize;
};

#endif // TEXTFONTLABEL_H
