#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DWidget>
#include <DLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QUndoStack>
#include <QGraphicsItem>




DWIDGET_USE_NAMESPACE

class CLeftToolBar;
class CGraphicsView;
class CDrawScene;
class ProgressLayout;

class CCentralwidget: public DWidget
{
    Q_OBJECT
public:
    CCentralwidget(DWidget *parent = nullptr);
    ~CCentralwidget();
    CLeftToolBar *getLeftToolBar();
    CGraphicsView *getGraphicsView() const;
    CDrawScene *getDrawScene() const;

    void openPicture(QString path);
signals:

    //传递图片的旋转和翻转信号
    void signalPassPictureOper(int);

    /**
     * @brief signalAttributeChangedFromScene 传递场景中选择图元改变信号
     */
    void signalAttributeChangedFromScene(bool, int);
    void signalSetScale(const qreal);

    void signalForwardQuitCutMode();
    void signalUpdateCutSize();





public slots:
    void importPicture();
    void slotResetOriginPoint();
    void slotAttributeChanged();
    void slotZoom(qreal scale);
    void slotSetScale(const qreal scale);
    void slotShowExportDialog();
    void slotNew();
    void slotPrint();
    void slotShowCutItem();


private slots:
    void slotDoCut(QRectF rect);
    void slotQuitCutMode();

private:
    CLeftToolBar *m_leftToolbar;
    DLabel *m_seperatorLine;

    QVBoxLayout *m_vLayout;
    QHBoxLayout *m_hLayout;

    int m_horizontalMargin;
    int m_verticalMargin;

    DMenu *m_contextMenu;
    CGraphicsView *m_pGraphicsView;
    CDrawScene *m_pDrawScene;
    ProgressLayout *m_progressLayout;
    int m_picNum;

private:
    void initUI();
    void initConnect();

};

#endif // MAINWIDGET_H
