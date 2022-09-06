// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DIALOG_H
#define DIALOG_H

#include "sitemdata.h"

#include <DDialog>
#include <DWidget>

DWIDGET_USE_NAMESPACE

enum   EMessageType {ENormalMsg, EWarningMsg, EQuestionMsg};
enum   EButtonType {ENormalMsgBtn = DDialog::ButtonNormal,
                    EWarningMsgBtn = DDialog::ButtonWarning,
                    ESuggestedMsgBtn = DDialog::ButtonRecommend
                   };
struct SMessage {
    QString      message;
    int          maxMsgPixelLenth = 1920;
    EMessageType messageType = EWarningMsg;
    QStringList  btns;
    QList<EButtonType>  btnType;

    SMessage(const QString &msg = "",
             EMessageType msgType = EWarningMsg,
             const QStringList &moreBtns = QStringList() << QObject::tr("OK"),
             const QList<EButtonType> &btntps = QList<EButtonType>() << ENormalMsgBtn): message(msg),
        messageType(msgType), btns(moreBtns), btnType(btntps)
    {

    }
};
Q_DECLARE_METATYPE(SMessage)

class MessageDlg : public DDialog
{
    Q_OBJECT
public:
    explicit MessageDlg(DWidget *parent = nullptr);
    MessageDlg(const SMessage &message, DWidget *parent = nullptr);

    static int execMessage(const QString &message, QWidget *parent = nullptr);
    static int execMessage(const QString &message, EMessageType msgTp, QWidget *parent = nullptr);
    static int execMessage(const QString &message,
                           EMessageType msgTp,
                           const QStringList &moreBtns,
                           const QList<EButtonType> &btntps,
                           QWidget *parent = nullptr);
    static int execMessage(const SMessage &message, QWidget *parent = nullptr);

    void setMessage(const SMessage &message);
    SMessage message() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void updateMessage();

private:
    SMessage _message;
};

#endif // DIALOG_H
