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
#ifndef CPICTURETOOL_H
#define CPICTURETOOL_H

#include "idrawtool.h"
#include "cpictureitem.h"
#include "cdrawscene.h"
#include"widgets/progresslayout.h"
#include "frame/ccentralwidget.h"

#include <DFileDialog>


#include <DWidget>

DWIDGET_USE_NAMESPACE

class CPictureTool: public IDrawTool
{
    Q_OBJECT
public:
    explicit CPictureTool();

    /**
     * @brief toolButton 定义工具的激活按钮
     */
    QAbstractButton *initToolButton() override;

    DrawAttribution::SAttrisList attributions() override;

    /**
     * @brief registerAttributionWidgets
     */
    void  registerAttributionWidgets() override;

    ~CPictureTool();

    /**
     * @brief addImages 添加图像
     * @param pixmap 图像的信息
     * @param fileSrcData 图像的压缩源数据
     * @param asImageSize 加载成功后是否将场景大小设置为图像大小
     * @param addUndoRedo 这次添加是否记录到撤销还原
     * @param selected    加载成功后是否选中
     */
    Q_SLOT void addImage(QPixmap pixmap, CDrawScene *scene,
                         const QByteArray &fileSrcData,
                         bool asImageSize = false,
                         bool addUndoRedo = false,
                         bool selected = true);

    /**
     * @brief addLocalImages 添加本地图像,加载完程后默认会选中第一张图像
     * @param filePathList 本地图像的路径
     * @param scene        要被添加到的场景
     * @param asFirstImageSize 加载成功后是否将场景大小设置为第一张图像大小
     * @param addUndoRedo      这次添加是否记录到撤销还原
     * @param appFirstExec　 程序是否通过打开文件的方式初次运行(这个时候如果打开失败那么在最后需要关闭程序)
     */
    Q_SLOT void addLocalImages(const QStringList &filePathList,
                               CDrawScene *scene,
                               bool asFirstImageSize = false,
                               bool addUndoRedo = false,
                               bool hadCreatedOneViewScene = false,
                               bool appFirstExec = false);


private slots:
    /**
     * @brief onLoadImageFinished 显示加载失败的图片文件
     * @param files 失败的文件
     */
    void onLoadImageFinished(const QStringList &successFiles,
                             const QStringList &failedFiles,
                             const bool clearSelection = false,
                             const bool hadCreatedOneViewScene = false,
                             const bool appFirstExec = false);



    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

private:
    /**
     * @brief exeLoadFailedFilesDialog 显示加载失败的图片文件
     * @param files 失败的文件
     */
    void exeLoadFailedFilesDialog(const QStringList &files);

    /**
     * @brief readPixMapQuickly 快速读取本地图像
     * @param imagePath 本地图像文件
     */
    QPixmap readPixMapQuickly(const QString &imagePath);

    /**
     * @brief setSceneSize  获取一个进度显示控件
     * @param scene 场景指针
     * @param imageSize 要设置的大小
     */
    void setSceneSize(CDrawScene *scene, const QSize &imageSize);

private:
    /**
     * @brief getProgressLayout [UI] 获取一个进度显示控件
     * @param firstShow 创建该控件时是否立刻显示
     */
    ProgressLayout *getProgressLayout(bool firstShow = true);

private:
    ProgressLayout *m_progressLayout = nullptr;
};


#endif // CPICTURETOOL_H
