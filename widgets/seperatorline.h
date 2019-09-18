/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#ifndef SEPERATORLINE_H
#define SEPERATORLINE_H

#include <DLabel>
#include <DApplicationHelper>
#include <QDebug>
DWIDGET_USE_NAMESPACE
class SeperatorLine : public DLabel
{
    Q_OBJECT
public:
    SeperatorLine(DWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setFixedSize(6, 24);
//        //setStyleSheet("border: 1px solid rgba(0, 0, 0, 0.1);");
//        this->setFrameShape(QFrame::Box);
//        DPalette pa;
//        pa = DApplicationHelper::instance()->palette(this);
//        pa.setColor(DPalette::FrameBorder, QColor("#ececf8"));
//        DApplicationHelper::instance()->setPalette(this, pa);
//        //this->setPalette(pa);
        QPixmap seprtatorLine;
        seprtatorLine = QPixmap(QString(":/theme/common/line.svg"));
        this->setPixmap(seprtatorLine);
    }
    ~SeperatorLine() {}

};
#endif // SEPERATORLINE_H
