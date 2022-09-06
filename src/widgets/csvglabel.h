// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CSVGLABEL_H
#define CSVGLABEL_H

#include <DLabel>
#include <QSvgRenderer>
DWIDGET_USE_NAMESPACE
class QWidget;

class CSvgLabel : public DLabel
{
public:
    CSvgLabel(const QString &str = QString(), QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer m_svgRender;
};

#endif // CSVGLABEL_H
