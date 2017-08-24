#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QKeyEvent>

#include  <darrowrectangle.h>

#include "widgets/colorpanel.h"
#include "utils/baseutils.h"

#include "module/cutwidget.h"
#include "module/linewidget.h"
#include "module/fillshapewidget.h"
#include "module/textwidget.h"
#include "module/blurwidget.h"
#include "module/adjustsizewidget.h"

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

public:
    void showDrawDialog();
    void showSaveDialog();

public slots:
    void importImage();
    void drawShapes(QString shape);
    void setMiddleStackWidget(Status status);
    void setDrawStatus(DrawStatus drawstatus);
    void showColorfulPanel(DrawStatus drawstatus, QPoint pos);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QString m_path;
    bool m_shapesWidgetExist;
    QStackedWidget* m_stackWidget;
    int m_textFontsize = 12;

    QWidget* m_emptyWidget;
    CutWidget* m_cutWidget;
    LineWidget* m_drawLineWidget;
    FillshapeWidget* m_fillShapeWidget;
    TextWidget* m_drawTextWidget;
    BlurWidget* m_drawBlurWidget;
    AdjustsizeWidget* m_adjustsizeWidget;

    DArrowRectangle* m_colorARect;
    ColorPanel* m_colorPanel;

    Status            m_middleWidgetStatus;
    DrawStatus  m_drawStatus;

    QMenu* m_mainMenu;
};

#endif // TOPTOOLBAR_H
