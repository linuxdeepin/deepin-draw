#ifndef CICONBUTTON_H
#define CICONBUTTON_H

#include <DIconButton>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class CIconButton : public DIconButton
{
    Q_OBJECT
public:
    enum EIconButtonSattus {
        Normal,
        Hover,
        Press,
        Active
    };


public:
    explicit CIconButton(const QMap<int, QMap<EIconButtonSattus, QString>> &pictureMap,
                         const QSize &size, DWidget *parent = nullptr,
                         bool isCheckLock = true);
    void setChecked(bool);
    bool isChecked() const;

    void setTheme(int currentTheme);
    void setIconMode();
    void setButtonIcon(QIcon t_icon);

signals:
    void buttonClick();
    void mouseRelease();
public slots:

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

private:
    void updateImage();

private:
    bool m_isHover;
    bool m_isChecked;
    bool m_isPressed;
    bool m_isCheckLock; //是否只允许点击选中 不允许点击取消选中
    EIconButtonSattus m_currentStatus;
    EIconButtonSattus m_tmpStatus;
    int m_currentTheme;
    int m_iconMode = false;//是否属于icon模式的图标
    QIcon m_normalIcon;//normal状态下的icon图标切图

    QMap<int, QMap<EIconButtonSattus, QString> > m_pictureMap;

};

#endif // CICONBUTTON_H
