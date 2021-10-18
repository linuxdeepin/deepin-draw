/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

    QString iconSvg;
    switch (_message.messageType) {
    case ENormalMsg:
        iconSvg = ":/theme/common/images/deepin-draw-64.svg";
        break;
    case EWarningMsg:
        iconSvg = ":/icons/deepin/builtin/texts/Bullet_window_warning.svg";
        break;
    case EQuestionMsg:
        iconSvg = ":/icons/deepin/builtin/texts/Bullet_window_warning.svg";
        break;
    }
    this->setIcon(QPixmap(iconSvg));

    if (_message.btns.size() == _message.btnType.size())
        for (int i = 0; i < _message.btns.size(); ++i)
            this->insertButton(i, _message.btns.at(i), false, DDialog::ButtonType(_message.btnType.at(i)));
}
