#include "iconbutton.h"

#include <QDebug>

IconButton::IconButton(const QString &normalPic, const QString &hoverPic, const QString
    &pressPic, const QString &text, QWidget *parent) : QPushButton(parent) {

    setStyleSheet("QPushButton {outline: none; background-color: transparent; border: none;}");
    if (!normalPic.isEmpty())
        m_normalPic = normalPic;
    if (!hoverPic.isEmpty())
        m_hoverPic = hoverPic;
    if (!pressPic.isEmpty())
        m_pressPic = pressPic;

    setText(text);
    updateIcon();
}

IconButton::IconButton(const QString &normalPic, const QString &hoverPic, const QString
    &pressPic, const QString &checkedPic, const QString &text, QWidget *parent) : QPushButton(parent) {

     setStyleSheet("QPushButton {outline: none; background-color: transparent; border: none;}");

    if (!normalPic.isEmpty())
        m_normalPic = normalPic;
    if (!hoverPic.isEmpty())
        m_hoverPic = hoverPic;
    if (!pressPic.isEmpty())
        m_pressPic = pressPic;
    if (!checkedPic.isEmpty())
        m_checkedPic = checkedPic;

    setText(text);
    updateIcon();
}

IconButton::~IconButton() {}

void IconButton::setNormalPic(const QString &normalPic) {
    m_normalPic = normalPic;
    updateIcon();
}

void IconButton::setHoverPic(const QString &hoverPic) {
    m_hoverPic = hoverPic;
    updateIcon();
}
void IconButton::setPressPic(const QString &pressPic) {
    m_pressPic = pressPic;
    updateIcon();
}

void IconButton::setCheckedPic(const QString &checkedPic) {
    m_checkedPic = checkedPic;
    updateIcon();
}

IconButton::State IconButton::getState()  const {
    return m_state;
}

void IconButton::enterEvent(QEvent* event) {
    setCursor(Qt::PointingHandCursor);

    if (!isChecked()) {
        setState(Hover);
    }

    event->accept();
}

void IconButton::leaveEvent(QEvent* event)  {
    if (!isChecked()) {
        setState(Normal);
    }

    event->accept();
}

void IconButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (isCheckable()) {
         if (isChecked()) {
             setChecked(false);
             setState(Press);
         } else {
             setChecked(true);
             setState(Checked);
         }
    } else {
        setState(Press);
    }
    event->accept();
}

void IconButton::mouseReleaseEvent(QMouseEvent* event)  {
    if (!rect().contains(event->pos())) {
        return;
    }


    if (isCheckable()) {
        if (isChecked()) {
            setState(Checked);
        } else {
            setState(Normal);
        }
    } else {
        setState(Hover);
    }

    event->accept();
    QPushButton::mouseReleaseEvent(event);
}

void IconButton::mouseMoveEvent(QMouseEvent* event) {
    if (!isCheckable() && !rect().contains(event->pos())) {
        setState(Normal);
    }
}

void IconButton::updateIcon() {
    switch (m_state) {
    case Hover:{ if (!m_hoverPic.isEmpty()) setIcon(QPixmap(m_hoverPic));  qDebug() << "Hover:" <<  QPixmap(m_hoverPic).isNull();break;}
    case Press: if (!m_pressPic.isEmpty()) setIcon(QPixmap(m_pressPic)); qDebug() << "Press:" <<  QPixmap(m_pressPic).isNull(); break;
    case Checked: if (!m_checkedPic.isEmpty()) setIcon(QPixmap(m_checkedPic)); qDebug() << "Checked:" <<  QPixmap(m_checkedPic).isNull(); break;
    default: if (!m_normalPic.isEmpty()) setIcon(QPixmap(m_normalPic));   break;
    }

    Q_EMIT stateChanged();
}

void IconButton::setState(State state) {
    m_state = state;
    updateIcon();
}



