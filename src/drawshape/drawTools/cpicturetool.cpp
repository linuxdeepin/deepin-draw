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
#include <DPushButton>
#include <DToolButton>

#include "frame/cundoredocommand.h"
#include "mainwindow.h"
#include "ccentralwidget.h"
#include "cattributeitemwidget.h"
#include "cattributemanagerwgt.h"
#include "cgraphicslayer.h"

CPictureTool::CPictureTool()
    : IDrawTool(picture)
{
}

QAbstractButton *CPictureTool::initToolButton()
{
    DToolButton *m_picBtn = new DToolButton;
    m_picBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_I)));
    drawApp->setWidgetAccesibleName(m_picBtn, "Import tool button");
    m_picBtn->setToolTip(tr("Import(I)"));
    m_picBtn->setIconSize(QSize(48, 48));
    m_picBtn->setFixedSize(QSize(37, 37));
    m_picBtn->setCheckable(true);

    connect(m_picBtn, &DToolButton::toggled, m_picBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_picture tools_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_picture tools_disable");
        m_picBtn->setIcon(b ? activeIcon : icon);
    });
    m_picBtn->setIcon(QIcon::fromTheme("ddc_picture tools_normal"));
    return m_picBtn;
}

SAttrisList CPictureTool::attributions()
{
//    DrawAttribution::SAttrisList result;
//    result << defaultAttriVar(DrawAttribution::EImageLeftRot)
//           << defaultAttriVar(DrawAttribution::EImageRightRot)
//           << defaultAttriVar(DrawAttribution::EImageHorFilp)
//           << defaultAttriVar(DrawAttribution::EImageVerFilp)
//           << defaultAttriVar(DrawAttribution::EImageAdaptScene);

    DrawAttribution::SAttrisList result;
    DrawAttribution::SAttri attri(DrawAttribution::ETitle, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getShowViewNameByModifyState());
    result << attri;
    return result;
}

void CPictureTool::registerAttributionWidgets()
{
    auto m_leftRotateBtn = new DPushButton;
    m_leftRotateBtn->setObjectName("PicLeftRotateBtn");
    m_leftRotateBtn->setMaximumSize(QSize(38, 38));
    m_leftRotateBtn->setIcon(QIcon::fromTheme("ddc_contrarotate_normal"));
    m_leftRotateBtn->setIconSize(QSize(48, 48));
    m_leftRotateBtn->setToolTip(tr("Rotate 90° CCW"));
    m_leftRotateBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_leftRotateBtn, &DPushButton::clicked, m_leftRotateBtn, [ = ]() {
        emit drawApp->attributionsWgt()->attributionChanged(EImageLeftRot, true);
    });
    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, m_leftRotateBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_leftRotateBtn) {
            m_leftRotateBtn->setEnabled(var.toBool());
        }
    });
    CAttributeManagerWgt::installComAttributeWgt(EImageLeftRot, m_leftRotateBtn, false);


    auto m_rightRotateBtn = new DPushButton;
    m_rightRotateBtn->setObjectName("PicRightRotateBtn");
    m_rightRotateBtn->setMaximumSize(QSize(38, 38));
    m_rightRotateBtn->setIcon(QIcon::fromTheme("ddc_clockwise rotation_normal"));
    m_rightRotateBtn->setIconSize(QSize(48, 48));
    m_rightRotateBtn->setToolTip(tr("Rotate 90° CW"));
    m_rightRotateBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_rightRotateBtn, &DPushButton::clicked, m_rightRotateBtn, [ = ]() {
        emit drawApp->attributionsWgt()->attributionChanged(EImageRightRot, true);
    });
    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, m_rightRotateBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_rightRotateBtn) {
            m_rightRotateBtn->setEnabled(var.toBool());
        }
    });
    CAttributeManagerWgt::installComAttributeWgt(EImageRightRot, m_rightRotateBtn, false);

    auto m_flipHBtn = new DPushButton;
    m_flipHBtn->setObjectName("PicFlipHBtn");
    m_flipHBtn->setMaximumSize(QSize(38, 38));
    m_flipHBtn->setIcon(QIcon::fromTheme("ddc_flip horizontal_normal"));
    m_flipHBtn->setIconSize(QSize(48, 48));
    m_flipHBtn->setToolTip(tr("Flip horizontally"));
    m_flipHBtn->setFocusPolicy(Qt::NoFocus);

    connect(m_flipHBtn, &DPushButton::clicked, m_flipHBtn, [ = ]() {
        emit drawApp->attributionsWgt()->attributionChanged(EImageHorFilp, true);
    });

    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, m_flipHBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_flipHBtn) {
            m_flipHBtn->setEnabled(var.toBool());
        }
    });

    CAttributeManagerWgt::installComAttributeWgt(EImageHorFilp, m_flipHBtn, false);

    auto m_flipVBtn = new DPushButton;
    m_flipVBtn->setObjectName("PicFlipVBtn");
    m_flipVBtn->setMaximumSize(QSize(38, 38));
    m_flipVBtn->setIcon(QIcon::fromTheme("ddc_flip vertical_normal"));
    m_flipVBtn->setIconSize(QSize(48, 48));
    m_flipVBtn->setToolTip(tr("Flip vertically"));
    m_flipVBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_flipVBtn, &DPushButton::clicked, m_flipVBtn, [ = ]() {
        emit drawApp->attributionsWgt()->attributionChanged(EImageVerFilp, true);
    });

    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, m_flipVBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_flipVBtn) {
            m_flipVBtn->setEnabled(var.toBool());
        }
    });

    CAttributeManagerWgt::installComAttributeWgt(EImageVerFilp, m_flipVBtn, false);

    auto m_flipAdjustment = new DPushButton;
    m_flipAdjustment->setObjectName("PicFlipAdjustmentBtn");
    m_flipAdjustment->setMaximumSize(QSize(38, 38));
    m_flipAdjustment->setIcon(QIcon::fromTheme("ddc_flip_adjustment_normal"));
    m_flipAdjustment->setIconSize(QSize(48, 48));
    m_flipAdjustment->setToolTip(tr("Auto fit"));
    m_flipAdjustment->setFocusPolicy(Qt::NoFocus);

    connect(m_flipAdjustment, &DPushButton::clicked, m_flipAdjustment, [ = ]() {
        //emit drawApp->attributionsWgt()->attributionChanged(EImageAdaptScene, true);
        auto curScene = CManageViewSigleton::GetInstance()->getCurScene();
        if (curScene != nullptr) {
            auto rect = curScene->selectGroup()->sceneBoundingRect();
            if (rect != curScene->sceneRect()) {
                CCmdBlock block(curScene, CSceneUndoRedoCommand::ESizeChanged);
                curScene->setSceneRect(rect);
                curScene->updateAttribution();
                curScene->update();
            }
        }
    });

    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, m_flipAdjustment, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_flipAdjustment) {
            m_flipAdjustment->setEnabled(var.toBool());
        }
    });

    CAttributeManagerWgt::installComAttributeWgt(EImageAdaptScene, m_flipAdjustment, true);
}
CPictureTool::~CPictureTool()
{
}
// 参数'filePathList'应该通过const引用传递
void CPictureTool::addLocalImages(const QStringList &filePathList
                                  , CDrawScene *scene
                                  , bool asFirstImageSize
                                  , bool addUndoRedo
                                  , bool hadCreatedOneViewScene
                                  , bool appFirstExec)
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
        Dtk::Widget::DDialog warnDlg(/*centralWindow*/drawApp->topMainWindowWidget());
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

            QPixmap pixmap = readPixMapQuickly(file);

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

            //QThread::msleep(100);
            QMetaObject::invokeMethod(this, "addImage", Qt::QueuedConnection,
                                      Q_ARG(QPixmap, pixmap),
                                      Q_ARG(CDrawScene *, scene),
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
            this->onLoadImageFinished(successFiles, failedFiles, asFirstImageSize, hadCreatedOneViewScene, appFirstExec);
            this->getProgressLayout()->setProgressValue(filenames.size());
        }, Qt::QueuedConnection);
    });
}

QPixmap CPictureTool::readPixMapQuickly(const QString &imagePath)
{
    auto fOptimalConditions = [ = ](const QImageReader & reader) {
        QSize orgSz = reader.size();
        return (orgSz.width() > 1920 && orgSz.height() > 1080);
    };

    QPixmap pixmap;
    QImageReader reader;
    reader.setFileName(imagePath);
    bool shouldOptimal = fOptimalConditions(reader);
    qreal radio = shouldOptimal ? 0.5 : 1.0;
    QSize orgSize = reader.size();
    QSize newSize = orgSize * radio;
    reader.setScaledSize(newSize);


    if (shouldOptimal)
        reader.setQuality(30);

    //当不能读取数据时,设置以内容判断格式进行加载,这样可以避免后缀名修改后文件无法打开的情况
    if (!reader.canRead()) {
        reader.setAutoDetectImageFormat(true);
        reader.setDecideFormatFromContent(true);
        reader.setFileName(imagePath); //必须重新设置一下文件,才能触发内部加载方式的切换
    }

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

void CPictureTool::setSceneSize(CDrawScene *scene, const QSize &imageSize)
{
//    QSize screenSize = QGuiApplication::primaryScreen()->availableSize();
//    if (imageSize.width() > screenSize.width() || imageSize.height() > screenSize.height()) {
//        scene->setSceneRect(0, 0, screenSize.width(), screenSize.height());
//    } else {
//        scene->setSceneRect(0, 0, imageSize.width(), imageSize.height());
//    }
    scene->setSceneRect(0, 0, imageSize.width(), imageSize.height());
}

void CPictureTool::addImage(QPixmap pixmap,
                            CDrawScene *scene
                            , const QByteArray &fileSrcData
                            , bool asImageSize
                            , bool addUndoRedo, bool selected)
{
    //CPictureItem *pixmapItem = nullptr;
    JDynamicLayer *pixmapItem = nullptr;
    if (!pixmap.isNull()) {
//        pixmapItem = new CPictureItem(QRectF(scene->sceneRect().topLeft().x(),
//                                             scene->sceneRect().topLeft().y(),
//                                             pixmap.width(), pixmap.height()),
//                                      pixmap, nullptr, fileSrcData);

        pixmapItem = new JDynamicLayer(pixmap.toImage());
        CCmdBlock cmd(addUndoRedo ? scene : nullptr, CSceneUndoRedoCommand::EItemAdded, QList<QGraphicsItem *>() << pixmapItem);

        scene->addCItem(pixmapItem);

        // 判断当前图片是否需要进行自适应设置
        if (asImageSize) {
            setSceneSize(scene, pixmap.size());
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
                                       const QStringList &failedFiles,
                                       const bool clearSelection,
                                       const bool hadCreatedOneViewScene,
                                       const bool appFirstExec)
{
    Q_UNUSED(successFiles)

    if (m_progressLayout != nullptr) {
        m_progressLayout->close();
    }

    if (!failedFiles.isEmpty()) {
        exeLoadFailedFilesDialog(failedFiles);
    }

    //qWarning() << "successFiles count = " << successFiles.count() << "appFirstExec = " << appFirstExec;
    //如果加载没有成功的,并且是初次打开程序(标签数为1,且当前标签是没有修改过的(不能撤销不能重做的))
    if (successFiles.isEmpty() && (appFirstExec || hadCreatedOneViewScene)) {
        auto view = CManageViewSigleton::GetInstance()->getCurView();
        if (view != nullptr) {
            //没有被操作过那么就可以关闭这个view
            if (view->isModifyStashEmpty()) {
                drawApp->topMainWindow()->getCCentralwidget()->closeViewScense(view);
                CManageViewSigleton::GetInstance()->quitIfEmpty();
                return;
            }
        }
    }
    if (clearSelection) {
        dynamic_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene())->clearSelection();
        //CManageViewSigleton::GetInstance()->getCurView()->setModify(true);
    }
}

void CPictureTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
    if (nowStatus == EReady) {
        IDrawTool::onStatusChanged(oldStatus, nowStatus);
        DFileDialog fileDialog(drawApp->topMainWindowWidget());
        //设置文件保存对话框的标题
        if (Application::isTabletSystemEnvir())
            fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        else
            fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        fileDialog.setWindowTitle(tr("Import Picture"));
        QStringList filters;
        filters << "*.png *.jpg *.bmp *.tif";
        fileDialog.setNameFilters(filters);
        fileDialog.setFileMode(QFileDialog::ExistingFiles);

        if (fileDialog.exec() == QDialog::Accepted) {
            QStringList filenames = fileDialog.selectedFiles();
            drawApp->openFiles(filenames, false, true);
        }
        drawApp->setCurrentTool(selection);
    }
}

void CPictureTool::exeLoadFailedFilesDialog(const QStringList &files)
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



