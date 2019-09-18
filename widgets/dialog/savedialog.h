/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include "dialog.h"

#include <QKeyEvent>
#include <DComboBox>
#include <DSlider>

//该类作用是导出图片的功能，不是保存和另存为功能

class SaveDialog : public Dialog
{
    Q_OBJECT
public:
    explicit SaveDialog(QList<QPixmap> pixs, DWidget *parent = 0);

signals:
    void saveToPath(const QString &path);
    void imageNameChanged(QString imagename);

public:
    QString getSaveDir(QString dir);
    void updateImageSize();
    void saveImage(const QString &path);
    QString createSaveBaseName();
    void setVisible(bool visible);

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_closeDialog;
    QString m_fileDir;
    QString m_filePath;
    QString m_imagePath;
    QString m_lastDir;

    QList<QPixmap> m_pixmaps;
    DSlider *m_qualitySlider;
    DLabel *m_valueLabel;
    DComboBox *m_contentFormatCBox;
    DComboBox *m_contentSaveDirCBox;
};

#endif // SAVEDIALOG_H
