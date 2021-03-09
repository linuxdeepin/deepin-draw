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
#ifndef CICONBUTTON_H
#define CICONBUTTON_H

#include <DIconButton>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class CIconButton : public DIconButton
{
    Q_OBJECT
public:
    enum EIconButtonSattus {
        Normal,
        Hover,
        Press,
        Active
    };


public:
    explicit CIconButton(const QMap<int, QMap<EIconButtonSattus, QString>> &pictureMap,
                         const QSize &size, DWidget *parent = nullptr,
                         bool isCheckLock = true);
    void setChecked(bool);
    bool isChecked() const;

    void setTheme(int currentTheme);
    void setIconMode();
//    void setButtonIcon(QIcon t_icon);

signals:
    void buttonClick();
    void mouseRelease();
public slots:

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;

private:
    void updateImage();

private:
    bool m_isHover;
    bool m_isChecked;
    bool m_isPressed;
    bool m_isCheckLock; //是否只允许点击选中 不允许点击取消选中
    EIconButtonSattus m_currentStatus;
    EIconButtonSattus m_tmpStatus;
    int m_currentTheme;
    int m_iconMode = false;//是否属于icon模式的图标
    QIcon m_normalIcon;//normal状态下的icon图标切图

    QMap<int, QMap<EIconButtonSattus, QString> > m_pictureMap;

};

#endif // CICONBUTTON_H
