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
#ifndef CSPINBOX_H
#define CSPINBOX_H

#include <QWidget>
#include "globaldefine.h"

#ifdef USE_DTK
#include <DSpinBox>
#define CSPINBOXFATHERCLASS DSpinBox
DWIDGET_USE_NAMESPACE
#else
#include <QSpinBox>
#define CSPINBOXFATHERCLASS QSpinBox
#endif

class QTimer;

class CSpinBox : public CSPINBOXFATHERCLASS
{
    Q_OBJECT
public:
    explicit CSpinBox(QWidget *parent = nullptr);

    bool isTimerRunning();

    bool isChangedByWheelEnd();

    void setValueChangedKeepFocus(bool b);

    void setSpinRange(int min, int max);

    void setSpecialText(QString sp = "— —");

    void setSpinPhaseValue(int value, EChangedPhase phase);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void showEvent(QShowEvent *event) override;

    void keyPressEvent(QKeyEvent *event)override;
    void keyReleaseEvent(QKeyEvent *event)override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    bool eventFilter(QObject *o, QEvent *e) override;

    Q_SLOT void timerStart();
    Q_SLOT void timerEnd();

signals:
    void focusChanged(bool isFocus);

    void valueChanged(int value, EChangedPhase phase);

private:
    QTimer *getTimer();

    void updateMaxSize();

public:
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    QTimer *_wheelTimer = nullptr;
    bool    _wheelbegin = false;
    bool    _wheelEnd   = false;
    bool    _keepFocus = false;

    int _s_value = -100;
    int _s_phase = -100;

    int m_min = -1;
    int m_max = INT_MAX;



    //QSize _sizeHit;
};

#endif // CSPINBOX_H
