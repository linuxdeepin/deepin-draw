#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QKeyEvent>

#include "utils/baseutils.h"

#include "cutwidget.h"
#include "linewidget.h"
#include "fillshapewidget.h"
#include "textwidget.h"
#include "blurwidget.h"
#include "adjustsizewidget.h"
#include "widgets/arrowrectangle.h"
#include "widgets/colorpanel.h"
#include "widgets/toolbutton.h"

DWIDGET_USE_NAMESPACE

class TopToolbar : public QFrame {
    Q_OBJECT

public:
    TopToolbar(QWidget* parent = 0);
    ~TopToolbar();

    void initStackWidget();
    void initMenu();

    QMenu* mainMenu();

signals:
    void drawShapeChanged(QString shape);
    void rotateImage(int degree);
    void mirroredImage(bool horizonta, bool vertical);
    void cutImageAction();
    void generateSaveImage();
    void printImage();
    void updatePicTooltip(bool import);
    void fillShapeSelectedActive(bool selected);
    void adjustArtBoardSize(QSize size);
    void importPicBtnClicked();
    void cutImageFinished();
    void updateSelectedBtn(bool checked);
    void autoCrop();

    void resetPicBtn();
    void resizeArtboard(bool resized, QSize size);

public:
//    void showDrawDialog();
    void showSaveDialog();
    void drawShapes(QString shape);
    void showPrintDialog();
    MiddleWidgetStatus middleWidgetStatus();

public slots:

    void updateMiddleWidget(int type);


    void setDrawStatus(DrawStatus drawstatus);
    void showColorfulPanel(DrawStatus drawstatus,
                           QPoint pos, bool visible = true);
//    void updateCurrentShape(QString shape);
    void updateColorPanelVisible(QPoint pos);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QString m_path;
    QStringList m_paths;
    QStackedWidget* m_stackWidget;
    int m_textFontsize = 12;


    QWidget* m_emptyWidget;
    CutWidget* m_cutWidget;
    LineWidget* m_drawLineWidget;
    FillshapeWidget* m_fillShapeWidget;
    TextWidget* m_drawTextWidget;
    BlurWidget* m_drawBlurWidget;
    AdjustsizeWidget* m_adjustsizeWidget;

    ArrowRectangle* m_colorARect;
    ColorPanel* m_colorPanel;
    MiddleWidgetStatus m_middleWidgetStatus;
    DrawStatus  m_drawStatus;

    QMenu* m_mainMenu;

private:
    void setMiddleStackWidget(int status);
};

#endif // TOPTOOLBAR_H
