#ifndef CCLICKBUTTON_H
#define CCLICKBUTTON_H

#include <DPushButton>

DWIDGET_USE_NAMESPACE

class CClickButton : public DPushButton
{
    Q_OBJECT
public:
    enum CClickBtnSatus {
        Normal,
        Hover,
        Press
    };

public:
    explicit CClickButton(const QMap<CClickBtnSatus, QString> &pictureMap, QWidget *parent = nullptr );


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
    CClickBtnSatus m_currentStatus;

    QPixmap m_currentPicture;

    QMap<CClickBtnSatus, QString> m_pictureMap;
};

#endif // CCLICKBUTTON_H
