#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DWidget>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE

class TopToolbar;
class CCentralwidget;

class MainWindow: public DMainWindow
{
    Q_OBJECT
public:
    MainWindow(DWidget *parent = 0);
    ~MainWindow();

    void activeWindow();

    void showDrawDialog();
signals:
    void signalResetOriginPoint();


protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    TopToolbar *m_topToolbar;
    CCentralwidget *m_centralWidget;
    int m_titlebarWidth;

private:
    void initConnection();
    void initUI();
};

#endif // MAINWINDOW_H
