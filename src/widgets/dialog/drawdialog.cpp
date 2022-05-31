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
#include "drawdialog.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"
#include "application.h"

#include <QHBoxLayout>
#include <DLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

DrawDialog::DrawDialog(DWidget *parent)
    : DDialog(parent)
{
    setModal(true);

    setWgtAccesibleName(this, "Notice save dialog");

    setIcon(QIcon::fromTheme("deepin-draw"));

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Discard"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    // Input content
    DLabel *title = new DLabel(tr("Save the current contents?"), this);
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
}


void  DrawDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}
extern QWidget *defaultParentWindow();
void DrawDialog::showEvent(QShowEvent *event)
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

