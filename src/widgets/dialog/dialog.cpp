// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dialog.h"
#include <QKeyEvent>
#include <QApplication>
#include <QLabel>

MessageDlg::MessageDlg(DWidget *parent) : DDialog(parent)
{
    setModal(true);
    setWordWrapMessage(true);
    setMinimumSize(403, 163);
}

MessageDlg::MessageDlg(const SMessage &message, DWidget *parent): MessageDlg(parent)
{
    setMessage(message);
}

QWidget *defaultParentWindow()
{
    if (qApp->activeModalWidget() != nullptr) {
        return qApp->activeModalWidget();
    }
    return qApp->activeWindow();
}

int MessageDlg::execMessage(const SMessage &message, QWidget *parent)
{
    MessageDlg dialog(message, parent == nullptr ? defaultParentWindow() : parent);

    return dialog.exec();
}

int MessageDlg::execMessage(const QString &message, QWidget *parent)
{
    SMessage msg(message);
    return execMessage(msg, parent);
}

int MessageDlg::execMessage(const QString &message, EMessageType msgTp, QWidget *parent)
{
    SMessage msg(message, msgTp);
    return execMessage(msg, parent);
}

int MessageDlg::execMessage(const QString &message, EMessageType msgTp,
                            const QStringList &moreBtns,
                            const QList<EButtonType> &btntps,
                            QWidget *parent)
{
    SMessage msg(message, msgTp, moreBtns, btntps);
    return execMessage(msg, parent);
}


void MessageDlg::setMessage(const SMessage &message)
{
    _message = message;
    updateMessage();
}

SMessage MessageDlg::message() const
{
    return _message;
}

void MessageDlg::keyPressEvent(QKeyEvent *e)
{
    if (e->matches(QKeySequence::Cancel)) {
        close();
        return;
    }
    DDialog::keyPressEvent(e);
}

void MessageDlg::showEvent(QShowEvent *event)
{
    QMetaObject::invokeMethod(this, [ = ]() {
        QMetaObject::invokeMethod(this, [ = ]() {

            auto window = this->parentWidget() != nullptr ? this->parentWidget()->window() : defaultParentWindow();
            if (window != nullptr) {
                QPoint centerPos = window->geometry().center() - this->geometry().center();
                QRect parentWindowGem = window->geometry();

                centerPos = parentWindowGem.topLeft() + QPoint((parentWindowGem.width() - this->width()) / 2,
                                                               (parentWindowGem.height() - this->height()) / 2);

                this->move(centerPos);
            }

        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);

    DDialog::showEvent(event);
}

void MessageDlg::updateMessage()
{
    clearButtons();

    auto showText = QFontMetrics(this->font()).elidedText(_message.message, Qt::ElideMiddle, _message.maxMsgPixelLenth);

    this->DDialog::setMessage(showText);
    QSize s;
    // QString iconSvg;
    QPixmap iconSvg;
    switch (_message.messageType) {
    case ENormalMsg:
        iconSvg = QPixmap(QIcon::fromTheme("deepin-draw").pixmap(s));
        break;
    case EWarningMsg:
        iconSvg = QPixmap(":/icons/deepin/builtin/texts/Bullet_window_warning.svg");
        break;
    case EQuestionMsg:
        iconSvg = QPixmap(":/icons/deepin/builtin/texts/Bullet_window_warning.svg");
        break;
    }
    this->setIcon(iconSvg);

    if (_message.btns.size() == _message.btnType.size())
        for (int i = 0; i < _message.btns.size(); ++i)
            this->insertButton(i, _message.btns.at(i), false, DDialog::ButtonType(_message.btnType.at(i)));
}
