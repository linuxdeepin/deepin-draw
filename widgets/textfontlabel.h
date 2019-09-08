#ifndef TEXTFONTLABEL_H
#define TEXTFONTLABEL_H

#include <DLabel>
#include <DLineEdit>

#include "toolbutton.h"
DWIDGET_USE_NAMESPACE
class TextFontLabel : public DLabel
{
    Q_OBJECT
public:
    TextFontLabel(DWidget *parent = 0);
    ~TextFontLabel();

private:
    int m_fontsize;
};

#endif // TEXTFONTLABEL_H
