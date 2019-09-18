#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLineEdit>
#include <DFontComboBox>

#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class TextColorButton;

class TextWidget : public DWidget
{
    Q_OBJECT
public:
    TextWidget(DWidget *parent = nullptr);
    ~TextWidget();

public slots:
    void updateTextWidget();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void resetColorBtns();
    void signalTextAttributeChanged();

private:
    TextColorButton *m_fillBtn;
    DSlider *m_fontSizeSlider;
    DLineEdit *m_fontSizeEdit;
    DFontComboBox *m_fontComBox;
    bool m_isUsrDragSlider;

private:
    void initUI();
    void initConnection();
};

#endif // TEXTWIDGET_H
