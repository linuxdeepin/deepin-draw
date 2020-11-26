/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     JiangChangli
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
#include "cpicturetool.h"
#include "frame/cgraphicsview.h"
#include "frame/cviewmanagement.h"

#include <QDebug>
#include <QtConcurrent>
#include <QGraphicsItem>
#include <QImageReader>
#include <QScreen>
#include <QGuiApplication>

#include "application.h"

#include <DMessageBox>
#include <DDialog>

#include "service/cmanagerattributeservice.h"
#include "frame/cundoredocommand.h"


CPictureTool::CPictureTool(DWidget *parent)
    : DWidget(parent)
{
}
CPictureTool::~CPictureTool()
{
}

void CPictureTool::drawPicture(QStringList filePathList
                               , CDrawScene *scene
                               , CCentralwidget *centralWindow
                               , bool asFirstImageSize
                               , bool addUndoRedo)
{
    QStringList filenames = filePathList;
    int exitPicNum = 0;

    //获取已导入图片数量
    QList<QGraphicsItem *> items = scene->items();

    if (items.count() != 0) {
        for (int i = 0; i < items.size(); i++) {
            if (items[i]->type() == PictureType) {
                exitPicNum = exitPicNum + 1;
            };
        }
    }

    //大于30张报错，主要是适应各种系统环境，不给内存太大压力
    if (exitPicNum + filenames.size() > 30) {
        Dtk::Widget::DDialog warnDlg(centralWindow);
        warnDlg.setIcon(QIcon::fromTheme("dialog-warning"));
        warnDlg.setTitle(tr("You can import up to 30 pictures, please try again!"));
        warnDlg.addButtons(QStringList() << tr("OK"));
        warnDlg.setDefaultButton(0);
        warnDlg.exec();
        return;
    }

    getProgressLayout()->setRange(0, filenames.size());

    // [bug:26525] 设置默认值
    getProgressLayout()->setProgressValue(1);
    getProgressLayout()->show();
    getProgressLayout()->raise();

    //启动图片导入线程
    QtConcurrent::run([ = ] {
        QList<QString> failedFiles;
        QList<QString> successFiles;
        int processValue = 1;
        bool firestImageAdjustScence = asFirstImageSize;
        for (int i = 0; i < filenames.size(); i++)
        {
            QString file = filenames[i];

            QPixmap pixmap = getPixMapQuickly(file);

            if (pixmap.isNull()) {
                failedFiles.append(file);
                continue;
            }

            successFiles.append(file);

            QFile f(file);
            QByteArray srcBytes;
            if (f.open(QFile::ReadOnly)) {
                srcBytes = f.readAll();
            }

            QMetaObject::invokeMethod(this, "addImages", Qt::QueuedConnection,
                                      Q_ARG(QPixmap, pixmap),
                                      Q_ARG(CDrawScene *, scene),
                                      Q_ARG(CCentralwidget *, centralWindow),
                                      Q_ARG(const QByteArray &, srcBytes),
                                      Q_ARG(bool, firestImageAdjustScence),
                                      Q_ARG(bool, addUndoRedo),
                                      Q_ARG(bool, i == filenames.size() - 1));


            // 多张图片只以第一张图片的大小进行自适应
            if (firestImageAdjustScence) {
                firestImageAdjustScence = false;
            }

            processValue++;

            //invoke process
            QMetaObject::invokeMethod(this, [ = ]() {
                this->getProgressLayout()->setProgressValue(processValue);
            }, Qt::QueuedConnection);
        }

        //invoke finished
        QMetaObject::invokeMethod(this, [ = ]()
        {
            this->onLoadImageFinished(successFiles, failedFiles, asFirstImageSize);
            this->getProgressLayout()->setProgressValue(filenames.size());
        }, Qt::QueuedConnection);
    });

    // [bug:25615] 第二次导入图片，属性栏中“自适应”按钮置灰
    CManagerAttributeService::getInstance()->signalIsAllPictureItem(true, false);
}

QPixmap CPictureTool::getPixMapQuickly(const QString &imagePath)
{
    auto fOptimalConditions = [ = ](const QImageReader & reader) {
        QSize orgSz = reader.size();
        return (orgSz.width() > 1920 && orgSz.height() > 1080);
    };

    QPixmap pixmap;
    QImageReader reader;
    reader.setAutoDetectImageFormat(true);
    reader.setDecideFormatFromContent(true);
    reader.setFileName(imagePath);
    bool shouldOptimal = fOptimalConditions(reader);
    qreal radio = shouldOptimal ? 0.5 : 1.0;
    QSize orgSize = reader.size();
    QSize newSize = orgSize * radio;
    reader.setScaledSize(newSize);


    if (shouldOptimal)
        reader.setQuality(30);

    if (reader.canRead()) {
        QImage img = reader.read();
        pixmap = QPixmap::fromImage(img);

        //维持原大小
        bool haveOptimal = shouldOptimal;
        if (haveOptimal) {
            pixmap = pixmap.scaled(orgSize);
        }
    }

    //qDebug() << " QImageReader load used ms  ================ " << ti.elapsed();;

    return pixmap;
}

ProgressLayout *CPictureTool::getProgressLayout(bool firstShow)
{
    if (m_progressLayout == nullptr) {
        m_progressLayout = new ProgressLayout(drawApp->topMainWindowWidget());

        if (firstShow) {
            QMetaObject::invokeMethod(this, [ = ]() {
                QRect rct = drawApp->topMainWindowWidget()->geometry();
                getProgressLayout()->move(rct.topLeft() + QPoint((rct.width() - m_progressLayout->width()) / 2,
                                                                 (rct.height() - m_progressLayout->height()) / 2));

                m_progressLayout->raise();
                m_progressLayout->show();
            }, Qt::QueuedConnection);
        }
    }
    return m_progressLayout;
}

void CPictureTool::setScenceSizeByImporteImage(CDrawScene *scene, const QSize &imageSize)
{
    QSize screenSize = QGuiApplication::primaryScreen()->availableSize();
    if (imageSize.width() > screenSize.width() || imageSize.height() > screenSize.height()) {
        scene->setSceneRect(0, 0, screenSize.width(), screenSize.height());
    } else {
        scene->setSceneRect(0, 0, imageSize.width(), imageSize.height());
    }
}

void CPictureTool::addImages(QPixmap pixmap,
                             CDrawScene *scene
                             , CCentralwidget *centralWindow
                             , const QByteArray &fileSrcData
                             , bool asImageSize
                             , bool addUndoRedo, bool selected)
{
    Q_UNUSED(centralWindow);
    CPictureItem *pixmapItem = nullptr;
    if (!pixmap.isNull()) {

        scene->clearSelection();

        pixmapItem = new CPictureItem(QRectF(scene->sceneRect().topLeft().x(), scene->sceneRect().topLeft().y(), pixmap.width(), pixmap.height()),
                                      pixmap, nullptr, fileSrcData);


        CCmdBlock cmd(addUndoRedo ? scene : nullptr, CSceneUndoRedoCommand::EItemAdded, QList<QGraphicsItem *>() << pixmapItem);

        pixmapItem->setSelected(false);
        pixmapItem->drawScene()->addCItem(pixmapItem);
        // 判断当前图片是否需要进行自适应设置
        if (asImageSize) {
            setScenceSizeByImporteImage(scene, pixmap.size());
        }
    }
    if (!pixmap.isNull() && selected) {
        if (pixmapItem != nullptr) {
            pixmapItem->drawScene()->clearSelectGroup();
            pixmapItem->drawScene()->selectItem(pixmapItem);
        }
    }
}

void CPictureTool::onLoadImageFinished(const QStringList &successFiles,
                                       const QStringList &failedFiles, const bool clearSelection)
{
    Q_UNUSED(successFiles)

    if (m_progressLayout != nullptr) {
        m_progressLayout->close();
    }

    if (!failedFiles.isEmpty()) {
        showLoadFailedFiles(failedFiles);
    }

    if (clearSelection) {
        dynamic_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene())->clearSelection();
        CManagerAttributeService::getInstance()->refreshSelectedCommonProperty(false);
        CManageViewSigleton::GetInstance()->getCurView()->setModify(true);
    }
}

void CPictureTool::showLoadFailedFiles(const QStringList &files)
{
    if (files.isEmpty())
        return;

    DDialog dia(drawApp->topMainWindowWidget());
    dia.setFixedSize(404, 163);
    dia.setModal(true);

    dia.setMessage(tr("Damaged file, unable to open it"));

    dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));
    dia.addButton(tr("OK"), false, DDialog::ButtonNormal);
    dia.exec();
}



