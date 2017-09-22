#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QKeyEvent>

#include  <darrowrectangle.h>

#include "utils/baseutils.h"
#include "widgets/colorpanel.h"
#include "widgets/toolbutton.h"

#include "cutwidget.h"
#include "linewidget.h"
#include "fillshapewidget.h"
#include "textwidget.h"
#include "blurwidget.h"
#include "adjustsizewidget.h"

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

    QMenu* mainMenu();

signals:
    void openImage(QString imageFileName);
    void drawShapeChanged(QString shape);
    void rotateImage(const QString &imagePath, int degree);
    void mirroredImage(bool horizonta, bool vertical);
    void cutImage();
    void saveImage(const QString &path);
    void updatePicTooltip(bool import);
    void fillShapeSelectedActive(bool selected);

public:
    void showDrawDialog();
    void showSaveDialog();
    void drawShapes(QString shape);

public slots:
    void importImage();
    void importImageDir();

    void setMiddleStackWidget(Status status);
    void setDrawStatus(DrawStatus drawstatus);
    void showColorfulPanel(DrawStatus drawstatus, QPoint pos);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QString m_path;
    QStringList m_paths;
    bool m_imageExist;
    QStackedWidget* m_stackWidget;
    int m_textFontsize = 12;

    QHBoxLayout* m_layout;
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
