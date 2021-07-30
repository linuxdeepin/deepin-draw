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
#include "filehander.h"

CPictureTool::CPictureTool()
    : IDrawTool(picture)
{
}

QAbstractButton *CPictureTool::initToolButton()
{
    DToolButton *m_picBtn = new DToolButton;
    m_picBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_I)));
    setWgtAccesibleName(m_picBtn, "Import tool button");
    m_picBtn->setToolTip(tr("Import (I)"));
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
    DrawAttribution::SAttrisList result;
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
        drawBoard()->setDrawAttribution(EImageLeftRot, true);
    });
    connect(drawBoard()->attributionWidget(), &CAttributeManagerWgt::updateWgt, m_leftRotateBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_leftRotateBtn) {
            m_leftRotateBtn->setEnabled(var.toBool());
        }
    });
    drawBoard()->attributionWidget()->installComAttributeWgt(EImageLeftRot, m_leftRotateBtn, false);


    auto m_rightRotateBtn = new DPushButton;
    m_rightRotateBtn->setObjectName("PicRightRotateBtn");
    m_rightRotateBtn->setMaximumSize(QSize(38, 38));
    m_rightRotateBtn->setIcon(QIcon::fromTheme("ddc_clockwise rotation_normal"));
    m_rightRotateBtn->setIconSize(QSize(48, 48));
    m_rightRotateBtn->setToolTip(tr("Rotate 90° CW"));
    m_rightRotateBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_rightRotateBtn, &DPushButton::clicked, m_rightRotateBtn, [ = ]() {
        drawBoard()->setDrawAttribution(EImageRightRot, true);
    });
    connect(drawBoard()->attributionWidget(), &CAttributeManagerWgt::updateWgt, m_rightRotateBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_rightRotateBtn) {
            m_rightRotateBtn->setEnabled(var.toBool());
        }
    });
    drawBoard()->attributionWidget()->installComAttributeWgt(EImageRightRot, m_rightRotateBtn, false);

    auto m_flipHBtn = new DPushButton;
    m_flipHBtn->setObjectName("PicFlipHBtn");
    m_flipHBtn->setMaximumSize(QSize(38, 38));
    m_flipHBtn->setIcon(QIcon::fromTheme("ddc_flip horizontal_normal"));
    m_flipHBtn->setIconSize(QSize(48, 48));
    m_flipHBtn->setToolTip(tr("Flip horizontally"));
    m_flipHBtn->setFocusPolicy(Qt::NoFocus);

    connect(m_flipHBtn, &DPushButton::clicked, m_flipHBtn, [ = ]() {
        drawBoard()->setDrawAttribution(EImageHorFilp, true);
    });

    connect(drawBoard()->attributionWidget(), &CAttributeManagerWgt::updateWgt, m_flipHBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_flipHBtn) {
            m_flipHBtn->setEnabled(var.toBool());
        }
    });

    drawBoard()->attributionWidget()->installComAttributeWgt(EImageHorFilp, m_flipHBtn, false);

    auto m_flipVBtn = new DPushButton;
    m_flipVBtn->setObjectName("PicFlipVBtn");
    m_flipVBtn->setMaximumSize(QSize(38, 38));
    m_flipVBtn->setIcon(QIcon::fromTheme("ddc_flip vertical_normal"));
    m_flipVBtn->setIconSize(QSize(48, 48));
    m_flipVBtn->setToolTip(tr("Flip vertically"));
    m_flipVBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_flipVBtn, &DPushButton::clicked, m_flipVBtn, [ = ]() {
        drawBoard()->setDrawAttribution(EImageVerFilp, true);
    });

    connect(drawBoard()->attributionWidget(), &CAttributeManagerWgt::updateWgt, m_flipVBtn, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_flipVBtn) {
            m_flipVBtn->setEnabled(var.toBool());
        }
    });

    drawBoard()->attributionWidget()->installComAttributeWgt(EImageVerFilp, m_flipVBtn, false);

    auto m_flipAdjustment = new DPushButton;
    m_flipAdjustment->setObjectName("PicFlipAdjustmentBtn");
    m_flipAdjustment->setMaximumSize(QSize(38, 38));
    m_flipAdjustment->setIcon(QIcon::fromTheme("ddc_flip_adjustment_normal"));
    m_flipAdjustment->setIconSize(QSize(48, 48));
    m_flipAdjustment->setToolTip(tr("Auto fit"));
    m_flipAdjustment->setFocusPolicy(Qt::NoFocus);

    connect(m_flipAdjustment, &DPushButton::clicked, m_flipAdjustment, [ = ]() {
        auto page = this->drawBoard()->currentPage();
        if (page == nullptr)
            return;

        auto curScene = page->scene();
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

    connect(drawBoard()->attributionWidget(), &CAttributeManagerWgt::updateWgt, m_flipAdjustment, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_flipAdjustment) {
            m_flipAdjustment->setEnabled(var.toBool());
        }
    });

    drawBoard()->attributionWidget()->installComAttributeWgt(EImageAdaptScene, m_flipAdjustment, true);
}
CPictureTool::~CPictureTool()
{
}


void CPictureTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
    if (nowStatus == EReady) {
        IDrawTool::onStatusChanged(oldStatus, nowStatus);
        DFileDialog fileDialog(drawBoard());
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
            if (drawBoard() != nullptr) {
                bool first = (drawBoard()->count() == 0);
                foreach (auto file, filenames) {
                    drawBoard()->load(file, first);
                    first = false;
                }
            }
        }
        drawBoard()->setCurrentTool(selection);
    }
}



