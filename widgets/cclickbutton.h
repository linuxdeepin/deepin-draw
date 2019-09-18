/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
#ifndef CCLICKBUTTON_H
#define CCLICKBUTTON_H

#include <DPushButton>

DWIDGET_USE_NAMESPACE

class CClickButton : public DPushButton
{
    Q_OBJECT
public:
    enum EClickBtnSatus {
        Normal,
        Hover,
        Press,
        Disable
    };

public:
    explicit CClickButton(const QMap<EClickBtnSatus, QString> &pictureMap, DWidget *parent = nullptr );
    void setDisable(bool);

signals:
    void buttonClick();
public slots:

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    EClickBtnSatus m_currentStatus;

    QPixmap m_currentPicture;

    QMap<EClickBtnSatus, QString> m_pictureMap;
};

#endif // CCLICKBUTTON_H
