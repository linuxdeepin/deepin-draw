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
#ifndef CDDFMANAGER_H
#define CDDFMANAGER_H

#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"

#include <QObject>
#include <QFile>

#include <DWidget>

class QGraphicsItem;
class QGraphicsScene;
class CGraphicsView;
class CProgressDialog;
class CAbstractProcessDialog;

DWIDGET_USE_NAMESPACE

class CDDFManager : public QObject
{
    Q_OBJECT

public:
    explicit CDDFManager(CGraphicsView *view = nullptr);
    /**
     * @brief saveToDDF 保存ＤＤＦ文件
     * @param path　路径
     * @param scene　场景句柄
     */
    void saveToDDF(const QString &path, const QGraphicsScene *scene, bool finishedNeedClose = false);
    /**
     * @brief loadDDF　载入ＤＤＦ文件
     * @param path　路径
     * @param isOpenByDDF　是否是通过ＤＤＦ文件启动画板
     */
    void loadDDF(const QString &path, bool isOpenByDDF = false);
    /**
     * @brief getLastSaveStatus　获取上一次保存状态
     */
    bool getLastSaveStatus() const;
    /**
     * @brief getSaveLastErrorString　获取上一次保存失败原因
     * @return QString　失败原因
     */
    QString getSaveLastErrorString() const;
    /**
     * @brief getSaveLastError　获取上一次失败错误
     * @return QFileDevice::FileError　失败原因
     */
    QFileDevice::FileError getSaveLastError() const;

signals:
    /**
     * @brief signalSaveDDFComplete　完成保存ＤＤＦ文件信号
     */
    void signalSaveDDFComplete();
    /**
     * @brief signalLoadDDFComplete　完成载入ＤＤＦ文件信号
     */
    void signalLoadDDFComplete(const QString &path, bool success);
    /**
     * @brief signalSaveDDFFaild　保存ＤＤＦ文件失败信号
     */
    void signalSaveDDFFaild();
    /**
     * @brief singalLoadDDFFaild　载入ＤＤＦ文件失败信号
     */
    void singalLoadDDFFaild();
    /**
     * @brief signalUpdateProcessBar　更新进度条信号
     */
    void signalUpdateProcessBar(int, bool);
    /**
     * @brief signalStartLoadDDF　发送开始载入ＤＤＦ文件信号
     * @param rect　场景矩形
     */
    void signalStartLoadDDF(QRectF rect);
    /**
     * @brief signalAddItem　添加图元信号
     * @param item　图元
     */
    void signalAddItem(QGraphicsItem *item, bool pushToStack = false);


    void signalAddTextItem(CGraphicsUnit &data,
                           bool pushToStack = false);

    /**
     * @brief signalContinueDoOtherThing　保存完成后继续做某事信号
     */
    void signalSaveFileFinished(const QString &savedFile,
                                bool status,
                                QString errorString,
                                QFileDevice::FileError error,
                                bool needClose);
    /**
     * @brief singalEndLoadDDF　结束载入ＤＤＦ文件信号
     */
    void singalEndLoadDDF();
    /**
     * @brief signalClearSceneBeforLoadDDF　载入ＤＤＦ文件前先清空场景信号
     */
    void signalClearSceneBeforLoadDDF();

private slots:
    /**
     * @brief slotSaveDDFComplete　完成保存ＤＤＦ文件槽函数
     */
    void slotSaveDDFComplete();
    /**
     * @brief slotLoadDDFComplete　完成载入ＤＤＦ文件槽函数
     */
    void slotLoadDDFComplete(const QString &path, bool success);

    /**
     * @brief slotSaveDDFComplete　处理保存/加载的进度显示
     */
    void slotProcessSchedule(int process, bool isSave);


private:

    /**
     * @brief isDdfFileDirty　通过md5判断ddf文件是否被修改过(已经变脏了)
     */
    bool isDdfFileDirty(const QString &filePath);


    void writeMd5ToDdfFile(const QString &filePath);

private:
    CGraphicsView *m_view;
    CGraphics m_graphics;
    CProgressDialog *m_CProgressDialog;
    CAbstractProcessDialog *m_pSaveDialog;
    QString m_path;

    bool m_lastSaveStatus; //最后一次保存状态
    QString m_lastErrorString; //最后一次保存的错误字符串
    QFileDevice::FileError m_lastError; // 最后一次保存的错误
    bool m_finishedClose = false;
};

#endif // CDDFMANAGER_H
