/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     JiangChangli
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

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

const QSize DIALOG_SIZE = QSize(422, 160);

DrawDialog::DrawDialog(DWidget *parent)
    : Dialog(parent)
{
    setModal(true);
//    setFixedSize(DIALOG_SIZE);
    setIconPixmap(QPixmap(":/theme/common/images/deepin-draw-64.svg"));

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Discard"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    // Input content
    //const QString subStyle = getFileContent(":/drawdialog.qss");
//    DLabel *title = new DLabel(tr("The changes you made have not been saved yet. Do you want to save them?"), this);
//    title->setWordWrap(true);
//    //title->setStyleSheet(subStyle);
//    title->setObjectName("DialogTitle");
//    title->setAlignment(Qt::AlignLeft);

//    DWidget *w = new DWidget(this);
//    w->setFixedHeight(this->height() - 60);

//    QVBoxLayout *layout = new QVBoxLayout(w);
//    layout->setContentsMargins(0, 0, 0, 0);
//    layout->setSpacing(8);
//    layout->addStretch();
//    layout->addWidget(title);
//    layout->addStretch();
//    addContent(w);

    //setTitle(tr("The changes you made have not been saved yet. Do you want to save them?"));

    //是否保存当前画板内容？
    setTitle(tr("Save the current contents?"));

    connect(this, &DrawDialog::buttonClicked, this, [ = ](int id) {
        if (id == 0) {
            this->close();
        } else if (id == 1) {
            this->close();
            emit singalDoNotSaveToDDF();
        } else {
            //点击发出保存信号后，要先把询问对话框关闭了
            this->close();
            emit signalSaveToDDF();
        }
    });
}


void  DrawDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}

