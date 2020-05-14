/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>
#include <DGuiApplicationHelper>

#include "qtsingleapplication.h"

class Application;

#if  defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<Application *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    int  execDraw(const QStringList &files, QString &glAppPath);

    QStringList getRightFiles(const QStringList &files);

    enum EFileClassEnum {ENotFile    = 0,
                         EDrawAppNotSup,
                         EDrawAppSup,
                         EDrawAppSupAndReadable,
                         EDrawAppSupButNotReadable
                        };
    typedef QMap<EFileClassEnum, QStringList> QFileClassedMap;
    QStringList doFileClassification(const QStringList &inFilesPath, QFileClassedMap &out);

    static QStringList &supPictureSuffix();
    static QStringList &supDdfStuffix();

signals:
    void popupConfirmDialog();

public slots:
    void onMessageRecived(const QString &message);
    void onThemChanged(DGuiApplicationHelper::ColorType themeType);

    void showMainWindow(const QStringList &paths);
    void noticeFileRightProblem(const QStringList &problemfile,
                                Application::EFileClassEnum classTp = EDrawAppNotSup,
                                bool checkQuit = true);
protected:
    void handleQuitAction();

private:
    void initI18n();
};
#endif // APPLICATION_H
