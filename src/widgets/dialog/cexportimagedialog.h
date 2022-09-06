// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CEXPORTIMAGEDIALOG_H
#define CEXPORTIMAGEDIALOG_H
#include "globaldefine.h"

#include <DWidget>
#include <DDialog>
#include <DLineEdit>
#include <DComboBox>
#include <DLabel>
#include <DSlider>
#include <DToolButton>
#include <DSuggestButton>

DWIDGET_USE_NAMESPACE

class Page;

class PathActiveButton: public DSuggestButton
{
    Q_OBJECT
public:
    using DSuggestButton::DSuggestButton;

protected:
    void paintEvent(QPaintEvent *event) override;
};

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

protected:
    void showEvent(QShowEvent *event) override;
    void saveSetting();

private:
    DLineEdit *m_fileNameEdit;
    QComboBox *m_savePathCombox;
    QComboBox *m_formatCombox;
    DSlider *m_qualitySlider;
    DLabel *m_qualityLabel;
    DLineEdit *m_pathEditor;
    PathActiveButton *m_pathChosenButton;

//    QString m_fileName;
    QString m_savePath;
    QString m_saveFormat;
    int m_quality;
    int m_lastIndex = 0;

    DDialog *m_questionDialog;


    int  m_saveBtnId = -1;

    int quitRet = 1;

private:
    void initUI();
    void initConnection();

    void showDirChoseDialog();
    int  execFileIsExists(const QString &path);
    QString getCompleteSavePath()const;

    void keyPressEvent(QKeyEvent *event) override;

    DECLAREPRIVATECLASS(CExportImageDialog)
};

#endif // CEXPORTIMAGEDIALOG_H
