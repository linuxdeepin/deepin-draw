#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QKeyEvent>

#include <darrowrectangle.h>

#include "widgets/colorpanel.h"

DWIDGET_USE_NAMESPACE

class ToolButton : public QPushButton {
    Q_OBJECT
public:
    ToolButton(QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
    }

    ToolButton(QString text, QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
        setText(text);
    }
    ~ToolButton() {}
};

class TopToolbar : public QFrame {
    Q_OBJECT

public:
    TopToolbar(QWidget* parent = 0);
    ~TopToolbar();

signals:
    void openImage(QString imageFileName);
    void  initShapeWidgetAction(QString shape, bool needInited);

public slots:
    void initStackWidget();
    void importImage();
    void drawShapes(QString shape);
    bool shapesWidgetExist();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    bool m_shapesWidgetExist;
    QStackedWidget* m_stackWidget;

    QWidget* m_fillShapeWidget;
    QWidget* m_cutWidget;
    QWidget* m_drawShapeWidget;
    QWidget* m_picWidget;
    QWidget* m_textWidget;
    QWidget* m_blurWidget;

    DArrowRectangle* m_strokeARect;
    ColorPanel* m_colorPanel;
};

#endif // TOPTOOLBAR_H
