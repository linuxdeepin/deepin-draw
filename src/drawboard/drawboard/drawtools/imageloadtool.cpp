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
#include "imageloadtool.h"
#include "drawboard.h"

#include <QDebug>
#include <QStandardPaths>
#include <QToolButton>


class ImageLoadTool::ImageLoadTool_private
{
public:
    explicit ImageLoadTool_private(ImageLoadTool *father): _father(father) {}
    ImageLoadTool *_father;
};
ImageLoadTool::ImageLoadTool(QObject *parent)
    : DrawTool(parent), ImageLoadTool_d(new ImageLoadTool_private(this))
{

    auto m_picBtn = toolButton();
    m_picBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_I)));
    setWgtAccesibleName(m_picBtn, "Import tool button");
    m_picBtn->setToolTip(tr("Import (I)"));
    m_picBtn->setIconSize(QSize(48, 48));
    m_picBtn->setFixedSize(QSize(37, 37));
    m_picBtn->setCheckable(true);

    connect(m_picBtn, &QToolButton::toggled, m_picBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_picture tools_normal", QIcon(RCC_DRAWBASEPATH + "ddc_picture tools_normal_48px.svg"));
        QIcon activeIcon = QIcon::fromTheme("ddc_picture tools_disable", QIcon(RCC_DRAWBASEPATH + "ddc_picture tools_disable_48px.svg"));
        m_picBtn->setIcon(b ? activeIcon : icon);
    });
    m_picBtn->setIcon(QIcon::fromTheme("ddc_picture tools_normal", QIcon(RCC_DRAWBASEPATH + "ddc_picture tools_normal_48px.svg")));
}

SAttrisList ImageLoadTool::attributions()
{
    SAttrisList result;
    return result;
}

ImageLoadTool::~ImageLoadTool()
{
    //delete ImageLoadTool_d;
}

int ImageLoadTool::toolType() const
{
    return picture;
}

void ImageLoadTool::pressOnScene(ToolSceneEvent *event)
{

}

void ImageLoadTool::moveOnScene(ToolSceneEvent *event)
{

}

void ImageLoadTool::releaseOnScene(ToolSceneEvent *event)
{

}


void ImageLoadTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
//    if (nowStatus == EReady) {
//        drawBoard()->setCurrentTool(selection);
//    }
    if (nowStatus == EReady) {
        DrawTool::onStatusChanged(oldStatus, nowStatus);
        QFileDialog fileDialog(drawBoard());
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        fileDialog.setWindowTitle(tr("Import Picture"));
        QStringList filters;
        filters << "*.png *.jpg *.bmp *.tif";
        fileDialog.setNameFilters(filters);
        fileDialog.setFileMode(QFileDialog::ExistingFiles);

        if (fileDialog.exec() == QDialog::Accepted) {
            QStringList filenames = fileDialog.selectedFiles();

            if (drawBoard() != nullptr) {
                drawBoard()->loadFiles(filenames);
            }
        }
        drawBoard()->setCurrentTool(selection);
    }
}
