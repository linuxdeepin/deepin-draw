#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <DPushButton>
#include <QMap>

DWIDGET_USE_NAMESPACE

class CCheckButton : public DPushButton
{
    Q_OBJECT
public:
    enum EButtonSattus {
        Normal,
        Hover,
        Press,
        Active
    };


public:
    explicit CCheckButton(const QMap<EButtonSattus, QString> &pictureMap, DWidget *parent = nullptr, bool isCheckLock = true);
    void setChecked(bool);
    bool isChecked() const;

signals:
    void buttonClick();
public slots:

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_isHover;
    bool m_isChecked;
    bool m_isPressed;
    bool m_isCheckLock; //是否只允许点击选中 不允许点击取消选中
    EButtonSattus m_currentStatus;
    EButtonSattus m_tmpStatus;

    QPixmap m_currentPicture;

    QMap<EButtonSattus, QString> m_pictureMap;

};

#endif // CPUSHBUTTON_H
