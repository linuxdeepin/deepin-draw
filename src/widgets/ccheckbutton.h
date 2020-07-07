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
#ifndef CCHECKBUTTON_H
#define CCHECKBUTTON_H

#include <QMap>
#include <DToolButton>

DWIDGET_USE_NAMESPACE

class CCheckButton : public DToolButton
{
    Q_OBJECT
public:
    enum EButtonSattus {
        Normal,
        Hover,
        Press,
        Active
    };


public:
    explicit CCheckButton(const QMap<int, QMap<EButtonSattus, QString> > &pictureMap, const QSize &size, DWidget *parent = nullptr, bool isCheckLock = true);

    void setChecked(bool);
    bool isChecked() const;

    void setCurrentTheme(int currentTheme);
    int getCurrentTheme() const;

    EButtonSattus getCurrentStatus();

signals:
    void buttonClick();
    void mouseRelease();
public slots:

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
//    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void updateImage();

private:
    bool m_isHover;
    bool m_isChecked;
    bool m_isPressed;
    bool m_isCheckLock; //是否只允许点击选中 不允许点击取消选中
    EButtonSattus m_currentStatus;
    EButtonSattus m_tmpStatus;
    int m_currentTheme;

    QMap<int, QMap<EButtonSattus, QString> > m_pictureMap;

};

#endif // CCHECKBUTTON_H
