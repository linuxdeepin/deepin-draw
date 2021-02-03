#ifndef CFONTCOMBOBOX_H
#define CFONTCOMBOBOX_H
#include <DFontComboBox>
DWIDGET_USE_NAMESPACE
class CFontComboBox : public DFontComboBox
{
    Q_OBJECT
public:
    CFontComboBox(QWidget *parent = nullptr);
    virtual void hidePopup() override;
    virtual void showPopup() override;

signals:
    void signalhidepopup();
    void signalshowpopup();
};

#endif // CFONTCOMBOBOX_H
