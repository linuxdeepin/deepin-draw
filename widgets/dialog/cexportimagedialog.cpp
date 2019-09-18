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
#include "cexportimagedialog.h"

#include <DFileDialog>
#include <DDialog>

#include <QFormLayout>
#include <QImageWriter>
#include <QStandardPaths>
#include <QPdfWriter>
#include <QPainter>

#include <QDebug>


const QSize DIALOG_SIZE = QSize(380, 280);
const QSize LINE_EDIT_SIZE = QSize(250, 35);

CExportImageDialog::CExportImageDialog(DWidget *parent)
    : DDialog(parent)
{
    initUI();
    initConnection();
}

CExportImageDialog::~CExportImageDialog()
{

}

void CExportImageDialog::showMe(const QPixmap &pixmap)
{
    m_saveImage = pixmap;

    m_fileNameEdit->setText("deepin.jpg");


    if (m_savePathCombox->count() == Other + 1) {
        m_savePathCombox->blockSignals(true);
        m_savePathCombox->removeItem(Other);
    }
    m_savePathCombox->blockSignals(false);

    m_savePathCombox->setCurrentIndex(Pictures);
    m_formatCombox->setCurrentIndex(JPG);
    m_qualitySlider->setValue(100);


    slotOnSavePathChange(Pictures);
    slotOnFormatChange(JPG);
    slotOnQualityChanged(m_qualitySlider->value());

    show();
}

void CExportImageDialog::initUI()
{
    setFixedSize(DIALOG_SIZE);
    setModal(true);

    setContentsMargins(0, 0, 0, 0);

    DLabel *logoLable = new DLabel(this);
    logoLable->setPixmap(QPixmap(":/theme/common/images/logo.svg"));
    logoLable->setFixedSize(QSize(32, 32));

    logoLable->move(25, 3);
    logoLable->setAlignment(Qt::AlignLeft);

    setWindowTitle(tr("Export"));;

//    DLabel *titleLabel = new DLabel(tr("Export"), this);
//    titleLabel->setFixedSize(DIALOG_SIZE.width(), 40);

//    QHBoxLayout *titleLayout = new QHBoxLayout(this);
//    titleLayout->setSpacing(0);
//    titleLayout->setMargin(0);
//    titleLayout->addWidget(logoLable, 0, Qt::AlignLeft);
//    titleLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);


//    titleLabel->move(0, 0);
//    titleLabel->setAlignment(Qt::AlignCenter);


    m_fileNameEdit = new DLineEdit(this);
    m_fileNameEdit->setFixedSize(LINE_EDIT_SIZE);
    m_fileNameEdit->setClearButtonEnabled(false);


    m_savePathCombox = new DComboBox(this);
    m_savePathCombox->insertItem(Pictures, tr("Pictures"));
    m_savePathCombox->insertItem(Documents, tr("Documents"));
    m_savePathCombox->insertItem(Downloads, tr("Downloads"));
    m_savePathCombox->insertItem(Desktop, tr("Desktop"));
    m_savePathCombox->insertItem(Videos, tr("Videos"));
    m_savePathCombox->insertItem(Music, tr("Music"));
    m_savePathCombox->insertItem(UsrSelect, tr("Select Other Dir"));
    m_savePathCombox->setFixedSize(LINE_EDIT_SIZE);


    m_formatCombox = new DComboBox(this);
    m_formatCombox->insertItem(JPG, tr("jpg"));
    m_formatCombox->insertItem(PNG, tr("png"));
    m_formatCombox->insertItem(BMP, tr("bmp"));
    m_formatCombox->insertItem(TIF, tr("tif"));
    m_formatCombox->insertItem(PDF, tr("pdf"));
    m_formatCombox->setFixedSize(LINE_EDIT_SIZE);

    m_qualitySlider = new DSlider(Qt::Horizontal, this);
    m_qualitySlider->setMinimum(1);
    m_qualitySlider->setMaximum(100);
    m_qualitySlider->setValue(100);
    m_qualitySlider->setFixedSize(QSize(100, LINE_EDIT_SIZE.height()));

    m_qualityLabel = new DLabel(this);
    m_qualityLabel->setFixedHeight(LINE_EDIT_SIZE.height());

    QHBoxLayout *qualityHLayout = new QHBoxLayout;
    qualityHLayout->setMargin(0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addSpacing(3);
    qualityHLayout->addWidget(m_qualitySlider);
    qualityHLayout->addSpacing(20);
    qualityHLayout->addWidget(m_qualityLabel);

    DWidget *contentWidget = new DWidget(this);
//    contentWidget->setStyleSheet("background-color: rgb(255, 0, 0);");
    contentWidget->setContentsMargins(0, 0, 0, 0);
    QFormLayout *fLayout = new QFormLayout(contentWidget);
    fLayout->setFormAlignment(Qt::AlignJustify);
    fLayout->setHorizontalSpacing(10);
    fLayout->addRow(tr("Name:"), m_fileNameEdit);
    fLayout->addRow(tr("Save to:"), m_savePathCombox);
    fLayout->addRow(tr("Format:"), m_formatCombox);
    fLayout->addRow(tr("Quality:"), qualityHLayout);
    addContent(contentWidget);

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    m_questionDialog = new DDialog(this);
    m_questionDialog->setModal(true);
    m_questionDialog->addButtons(QStringList() << tr("Cancel") << tr("Replace"));
    m_questionDialog->setFixedSize(400, 100);



//    setLayout(titleLayout);
}

void CExportImageDialog::initConnection()
{
    connect(m_savePathCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnSavePathChange(int)));
    connect(m_formatCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnFormatChange(int)));
    connect(this, SIGNAL(buttonClicked(int, const QString & )), this, SLOT(slotOnDialogButtonClick(int, const QString & )));
    connect(m_qualitySlider, SIGNAL(valueChanged(int)), this, SLOT(slotOnQualityChanged(int)));
    connect(m_questionDialog, SIGNAL(buttonClicked(int, const QString & )), this, SLOT(slotOnQuestionDialogButtonClick(int, const QString & )));

}

void CExportImageDialog::slotOnSavePathChange(int index)
{
    switch (index) {
    case Pictures:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        break;
    case Documents:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        break;
    case Downloads:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        break;
    case Desktop:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        break;
    case Videos:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        break;
    case Music:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
        break;
    case UsrSelect:
        showDirChoseDialog();
        break;
    case Other:
        m_savePath = m_savePathCombox->itemText(index);
        break;
    default:
        m_savePath = "";
        break;
    }
}

void CExportImageDialog::slotOnFormatChange(int index)
{
    switch (index) {
    case PDF:
    case BMP:
    case TIF:
        m_qualitySlider->setValue(100);
        m_qualitySlider->setEnabled(false);
        break;
    case JPG:
    case PNG:
        m_qualitySlider->setEnabled(true);
        break;
    default:
        break;
    }

    m_saveFormat = m_formatCombox->itemText(index);

    QString name = m_fileNameEdit->text().trimmed();

    name = name.mid(0, name.lastIndexOf(".") + 1);

    name += m_saveFormat;

    m_fileNameEdit->setText(name);
}

void CExportImageDialog::slotOnDialogButtonClick(int index, const QString &text)
{
    Q_UNUSED(text)

    if (index == 1) {
        QString completePath = m_savePath + "/" + m_fileNameEdit->text().trimmed();
        if (QFileInfo(completePath).exists()) {
            hide();
            showQuestionDialog(completePath);
        } else {
            doSave();
            hide();
        }
    }
}

void CExportImageDialog::slotOnQuestionDialogButtonClick(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == 1) {
        doSave();
    }
    m_questionDialog->hide();
}

void CExportImageDialog::slotOnQualityChanged(int value)
{
    m_qualityLabel->setText(QString("%1%").arg(value));
    m_quality = value;
}

void CExportImageDialog::showDirChoseDialog()
{
    DFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    dialog.setViewMode(DFileDialog::Detail);
    dialog.setFileMode(DFileDialog::DirectoryOnly);
    if (dialog.exec()) {
        QString fileDir = dialog.selectedFiles().first();
        if (m_savePathCombox->count() < Other + 1) {
            m_savePathCombox->insertItem(Other, fileDir);
        } else {
            m_savePathCombox->setItemText(Other, fileDir);
        }
        m_savePathCombox->setCurrentText(fileDir);
    }
}

void CExportImageDialog::showQuestionDialog(const QString &path)
{
    m_questionDialog->setMessage((QString(tr("%1 already exists, do you want to replace?")).arg(path)));
    m_questionDialog->show();
}

void CExportImageDialog::doSave()
{
    QString completePath = m_savePath + "/" + m_fileNameEdit->text().trimmed();
    if (m_formatCombox->currentIndex() == PDF) {
        QPdfWriter writer(completePath);
        int ww = writer.width();
        int wh = writer.height();
        QPainter painter(&writer);
        painter.drawPixmap(0, 0, QPixmap(m_saveImage).scaled(QSize(ww, wh), Qt::KeepAspectRatio));
    } else {
        bool isSuccess = m_saveImage.save(completePath, m_saveFormat.toUpper().toLocal8Bit().data(), m_quality);
        qDebug() << "!!!!!!!!!" << isSuccess << "::" << completePath << "::" << m_saveFormat;;
    }
}
