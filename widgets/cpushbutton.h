#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <DPushButton>
#include <QMap>

DWIDGET_USE_NAMESPACE

class CPushButton : public DPushButton
{
    Q_OBJECT
public:
    enum CButtonSattus {
        Normal,
        Hover,
        Press,
        Active
    };


public:
    explicit CPushButton(const QMap<CButtonSattus, QString> &pictureMap, QWidget *parent = nullptr );
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
    CButtonSattus m_currentStatus;
    CButtonSattus m_tmpStatus;

    QPixmap m_currentPicture;

    QMap<CButtonSattus, QString> m_pictureMap;

};

#endif // CPUSHBUTTON_H
