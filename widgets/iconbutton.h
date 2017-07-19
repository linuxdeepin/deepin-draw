#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>


class IconButton : public QPushButton {
    Q_OBJECT
public:
    enum State {Normal, Hover, Press, Checked};

    IconButton(const QString &normalPic, const QString &hoverPic,
               const QString &pressPic, const QString &text, QWidget* parent = 0);
    IconButton(const QString &normalPic, const QString &hoverPic,
               const QString &pressPic, const QString &checkedPic, const QString &text, QWidget* parent = 0);
    ~IconButton();

    void setNormalPic(const QString &normalPic);
    void setHoverPic(const QString &hoverPic);
    void setPressPic(const QString &pressPic);
    void setCheckedPic(const QString &checkedPic);

    inline const QString getNormalPic() const { return m_normalPic;}
    inline const QString getHoverPic() const { return m_hoverPic;}
    inline const QString getPressPic() const { return m_pressPic;}
    inline const QString getCheckedPic() const { return m_checkedPic;}

    State getState() const;

Q_SIGNALS:
    void stateChanged();

protected:
    void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
    void updateIcon();
    void setState(State state);

private:
    State m_state = Normal;

    QString m_normalPic;
    QString m_hoverPic;
    QString m_pressPic;
    QString m_checkedPic;

};
#endif // ICONBUTTON_H
