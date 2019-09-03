#include "cclickbutton.h"

#include <QPainter>
#include <QDebug>

CClickButton::CClickButton(const QMap<EClickBtnSatus, QString> &pictureMap, QWidget *parent) :
    DPushButton(parent),
    m_currentStatus(Normal),
    m_pictureMap(pictureMap)
{
    m_currentPicture = m_pictureMap[m_currentStatus];
}

void CClickButton::setDisable(bool isDisable)
{
    if (isDisable) {
        m_currentStatus = Disable;

    } else {
        m_currentStatus = Normal;
    }
    update();

    setEnabled(!isDisable);
}


void CClickButton::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

    QPixmap pixmap(m_pictureMap[m_currentStatus]);
    this->setFixedSize(pixmap.size());
    if (! pixmap.isNull()) {
        painter.drawPixmap(this->rect(), pixmap);
    }
}

void CClickButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Press;

    update();

    emit buttonClick();
}

void CClickButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Normal;

    update();

}

void CClickButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Hover;

    update();

}

void CClickButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_currentStatus = Normal;

    update();

}
