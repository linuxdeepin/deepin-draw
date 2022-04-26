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
#ifndef CEXPORTIMAGEDIALOG_H
#define CEXPORTIMAGEDIALOG_H
#include "globaldefine.h"

#include <QWidget>

#include <QComboBox>
#include <QLabel>
#include <QSlider>
#ifdef USE_DTK
#include <DDialog>
#include <DLineEdit>
#include <DMessageBox>
#define LINEEDITOR DLineEdit
#define EXPORTFATHER DDialog
DWIDGET_USE_NAMESPACE
#else
#include <QCheckBox>
#include <QDialog>
#define LINEEDITOR QLineEdit
#define EXPORTFATHER QDialog
#endif

class Page;
class DRAWLIB_EXPORT CExportImageDialog : public EXPORTFATHER
{
    Q_OBJECT
public:
    enum ESaveFormat {
        JPG,
        PNG,
        BMP,
        TIF,
        PDF
    };

    enum ESavePath {
        Pictures,
        Documents,
        Downloads,
        Desktop,
        Videos,
        Music,
        UsrSelect,
        Other
    };

public:
    explicit CExportImageDialog(QWidget *parent = nullptr);
    ~CExportImageDialog();

    int getImageType() const;
    QString getImageFormate() const;
    int getQuality() const;

    int exec() override;

    int execFor(const Page *page);

    QString resultFile()const;
    QSize desImageSize()const;

private slots:
    void slotOnSavePathChange(int index);
    void slotOnFormatChange(int index);
    int  execCheckFile(const QString &text);
    void slotOnQualityChanged(int value);
signals:
    void signalDoSave(QString);

private:
    LINEEDITOR *m_fileNameEdit;

    QComboBox *m_savePathCombox;
    QComboBox *m_formatCombox;
    QSlider *m_qualitySlider;
    QLabel *m_qualityLabel;

    QString m_savePath;
    QString m_saveFormat;
    int m_quality;
    int m_saveBtnId = -1;
    int quitRet = 1;

private:
    void initUI();
    void initConnection();

    void showDirChoseDialog();
    int  execFileIsExists(const QString &path);
    QString getCompleteSavePath()const;

    void keyPressEvent(QKeyEvent *event) override;

    PRIVATECLASS(CExportImageDialog)
};

#endif // CEXPORTIMAGEDIALOG_H
