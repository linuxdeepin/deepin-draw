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

#include <DToolButton>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class CClickButton : public DToolButton
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
    explicit CClickButton(const QMap<int, QMap<EClickBtnSatus, QString> > &pictureMap, const QSize &size, DWidget *parent = nullptr);
    void setDisable(bool);

    void setCurrentTheme(int currentTheme);

signals:
    void buttonClick();
public slots:

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
//    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
private:
    void updateImage();

private:
    EClickBtnSatus m_currentStatus;
    int m_currentTheme;


    QMap<int, QMap<EClickBtnSatus, QString> > m_pictureMap;
};

#endif // CCLICKBUTTON_H
