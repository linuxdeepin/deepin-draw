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
#ifndef CEXPORTIMAGEDIALOG_H
#define CEXPORTIMAGEDIALOG_H


#include <DWidget>
#include <DDialog>
#include <DLineEdit>
#include <DComboBox>
#include <DLabel>
#include <DSlider>

DWIDGET_USE_NAMESPACE

class CExportImageDialog : public DDialog
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
    explicit CExportImageDialog(DWidget *parent = nullptr);
    ~CExportImageDialog();

    void showMe();
    int getImageType() const;
    QString getImageFormate() const;
    int getQuality() const;

private slots:
    void slotOnSavePathChange(int index);
    void slotOnFormatChange(int index);
    void slotOnDialogButtonClick(int index, const QString &text);
    void slotOnQuestionDialogButtonClick(int index, const QString &text);
    void slotOnQualityChanged(int value);
signals:
    void signalDoSave(QString);

private:
    DLineEdit *m_fileNameEdit;
    DComboBox *m_savePathCombox;
    DComboBox *m_formatCombox;
    DSlider *m_qualitySlider;
    DLabel *m_qualityLabel;

//    QString m_fileName;
    QString m_savePath;
    QString m_saveFormat;
    int m_quality;

    DDialog *m_questionDialog;


    int  m_saveBtnId = -1;


private:
    void initUI();
    void initConnection();

    void showDirChoseDialog();
    void showQuestionDialog(const QString &path);
    bool isHaveSuffix(const QString &src);
    QString getCompleteSavePath();
};

#endif // CEXPORTIMAGEDIALOG_H
