// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CCOLORPICKWIDGET_H
#define CCOLORPICKWIDGET_H

#include <QWidget>
#include <DArrowRectangle>

#include "globaldefine.h"

DWIDGET_USE_NAMESPACE

class ColorPanel;
class CColorPickWidget : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit CColorPickWidget(QWidget *parent = nullptr);

    QColor color();

    ColorPanel *colorPanel();

    QWidget *caller();

    void setCaller(QWidget *pCaller);

    void setExpandWidgetVisble(bool visble);

    void show(int x, int y) override;

public slots:

    void setColor(const QColor &c);

    void setTheme(int theme);

signals:
    void colorChanged(const QColor &color, EChangedPhase phase);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    bool event(QEvent *e) override;

    //void paintEvent(QPaintEvent* event);

private:
    ColorPanel *m_colorPanel = nullptr;

    QColor        _color;

    EChangedPhase _phase = EChangedUpdate;

    QWidget      *_caller = nullptr;
};

#endif // CCOLORPICKWIDGET_H
