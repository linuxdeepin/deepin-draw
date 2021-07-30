/*
* Copyright (C) 2020 ~ 2021 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: WangYu<wangyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "ccutdialog.h"
#include "application.h"

#include <QKeyEvent>
#include <QHBoxLayout>
#include <DLabel>
#include <QLineEdit>
#include <QAbstractButton>
#include <QDebug>

CCutDialog::CCutDialog(DWidget *parent)
    : Dialog(parent)
    , m_cutStatus(Discard)
{
    setModal(true);

    setWgtAccesibleName(this, "Notice cut info dialog");

    this->setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

    addButton(tr("Discard"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);
    this->setObjectName("CutDialog");
    this->getButton(1)->setObjectName("CutDialogSave");

    // Input content
    DLabel *title = new DLabel(tr("Do you want to save the cropped image?"), this);
    title->setWordWrap(true);
    title->setAlignment(Qt::AlignCenter);

    DWidget *w = new DWidget(this);
    w->setFixedHeight(this->height() - 80);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    addContent(w);

    connect(this, &Dialog::buttonClicked, this, [ = ](int index, const QString & text) {
        Q_UNUSED(text)
        /*if (0 == index) {
            m_cutStatus = Discard;
        } else */if (0 == index) {
            m_cutStatus = Discard;
        } else if (1 == index) {
            m_cutStatus = Save;
        }
        this->close();
    });
}

CCutDialog::CutStatus CCutDialog::getCutStatus()
{
    return m_cutStatus;
}

int CCutDialog::exec()
{
    Dialog::exec();
    return getCutStatus();
}

void CCutDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        m_cutStatus = Discard;
        this->close();
    }
}
