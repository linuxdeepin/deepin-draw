#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <DPushButton>
#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QApplication>

#include "editlabel.h"
#include "drawshape/globaldefine.h"

class CCheckButton;

DWIDGET_USE_NAMESPACE

class ColorButton : public DPushButton
{
    Q_OBJECT
public:
    ColorButton(const QColor &color, DWidget *parent = nullptr);
    ~ColorButton();

    void setDisableColor(bool disable);

signals:
    void colorButtonClicked(QColor color);

protected:
    void paintEvent(QPaintEvent *);

private:
    QColor m_color;
    bool      m_disable;
};

class CAlphaControlWidget;
class PickColorWidget;

class ColorPanel : public DWidget
{
    Q_OBJECT
public:
    ColorPanel(DWidget *parent = nullptr);
    ~ColorPanel();
    void updateColorPanel(DrawStatus status);

signals:
    void updateHeight();
    void signalColorChanged();

public slots:
    void slotPickedColorChanged(QColor);
    void setConfigColor(QColor color);

private:
    DLineEdit *m_colLineEdit;
    CCheckButton *m_colorfulBtn;
    CAlphaControlWidget *m_alphaControlWidget;
    QList<QColor> m_colList;
    QList<ColorButton *> m_cButtonList;
    DrawStatus m_drawstatus;
    QButtonGroup *m_colorsButtonGroup;
    PickColorWidget *m_pickColWidget;
    bool m_expand;

private:
    void initUI();
    void initConnection();
    void resetColorBtn();
};

#endif // COLORPANEL_H
