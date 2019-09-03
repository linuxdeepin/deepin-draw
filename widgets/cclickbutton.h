#ifndef CCLICKBUTTON_H
#define CCLICKBUTTON_H

#include <DPushButton>

DWIDGET_USE_NAMESPACE

class CClickButton : public DPushButton
{
    Q_OBJECT
public:
    enum EClickBtnSatus {
        Normal,
        Hover,
        Press,
        Disable
    };

public:
    explicit CClickButton(const QMap<EClickBtnSatus, QString> &pictureMap, QWidget *parent = nullptr );
    void setDisable(bool);

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
    EClickBtnSatus m_currentStatus;

    QPixmap m_currentPicture;

    QMap<EClickBtnSatus, QString> m_pictureMap;
};

#endif // CCLICKBUTTON_H
