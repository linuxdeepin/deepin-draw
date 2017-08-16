#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QKeyEvent>

#include  <darrowrectangle.h>

#include "widgets/colorpanel.h"
#include "utils/baseutils.h"

DWIDGET_USE_NAMESPACE

class TopToolbar : public QFrame {
    Q_OBJECT

public:
    TopToolbar(QWidget* parent = 0);
    ~TopToolbar();

    enum Status {
        Empty,
        Cut,
        DrawLine,
        FillShape,
        DrawText,
        DrawBlur,
        AdjustSize,
    };

    void initStackWidget();
    void initMenu();
    bool shapesWidgetExist();

    QMenu* mainMenu();

signals:
    void openImage(QString imageFileName);
    void rotateImage(const QString &imagePath, int degree);
    void mirroredImage(bool horizonta, bool vertical);
    void cutImage();
    void  initShapeWidgetAction(const QString &shape);
    void shapesColorChanged(DrawStatus drawstatus, QColor);
    void shapesLineWidthChanged(int lineWidth);
    void lineShapeChanged(QString lineShape);
    void textFontsizeChanged(int fontsize);
    void blurLineWidthChanged(int lineWidth);

public slots:
    void importImage();
    void drawShapes(QString shape);
    void setMiddleStackWidget(Status status);
    void setDrawStatus(DrawStatus drawstatus);
    void setShapesColor(QColor color);
    void setLineShape(int lineIndex);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QString m_path;
    bool m_shapesWidgetExist;
    QStackedWidget* m_stackWidget;
    int m_textFontsize = 12;

    QWidget* m_emptyWidget;
    QWidget* m_cutWidget;
    QWidget* m_drawLineWidget;
    QWidget* m_fillShapeWidget;
    QWidget* m_drawTextWidget;
    QWidget* m_drawBlurWidget;
    QWidget* m_adjustsizeWidget;

    DArrowRectangle* m_strokeARect;
    ColorPanel* m_colorPanel;

    Status            m_middleWidgetStatus;
    DrawStatus  m_drawStatus;

    QMenu* m_mainMenu;
};

#endif // TOPTOOLBAR_H
