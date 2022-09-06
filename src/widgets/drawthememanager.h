// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DRAWTHEMEMANAGER_H
#define DRAWTHEMEMANAGER_H

#include <QObject>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class DrawThemeManager : public QObject
{
    Q_OBJECT
    explicit DrawThemeManager(QObject *parent = nullptr);
public:
    static DrawThemeManager *instance();

signals:
    void themeChanged(QString themeName);

public slots:
//    QString getCurrentTheme();
//    void setCurrentTheme(const QString themeName);
//    QString getQssForWidget(QString className);
//    void updateQss();

private:
    static DrawThemeManager *m_drawTheme;
    QString m_currentTheme = "light";
};

#endif // DRAWTHEMEMANAGER_H
