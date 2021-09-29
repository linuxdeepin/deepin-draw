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
#include "cexportimagedialog.h"
#include "widgets/csvglabel.h"
#include "application.h"
#include  "dialog.h"
#include "ccentralwidget.h"
#include "cvalidator.h"
#include "cexportimagedialog_p.h"

#include <DFileDialog>
#include <DDialog>
#include <DMessageBox>

#include <QImageWriter>
#include <QStandardPaths>
#include <QPdfWriter>
#include <QPainter>
#include <QCloseEvent>
#include <QDebug>
#include <QDateTime>
#include <QAbstractButton>

#define NAME_MAX 255
const QSize DIALOG_SIZE = QSize(380, 280);
const QSize LINE_EDIT_SIZE = QSize(250, 35);
enum {ECancel = -1, EReExec, EOK};

CExportImageDialog::CExportImageDialog(DWidget *parent)
    : DDialog(parent)
{
    _pPrivate = new CExportImageDialog_private(this);
    initUI();
    initConnection();
}

CExportImageDialog::~CExportImageDialog()
{
    delete _pPrivate;
}

//void CExportImageDialog::setSupImageSuffix(const QStringList &suffixs)
//{
//    m_formatCombox->clear();
//    foreach (auto suffix, suffixs) {
//        if (!suffix.isEmpty()) {
//            m_formatCombox->addItem(suffix.toLower());
//        }
//    }
//}


int CExportImageDialog::getImageType() const
{
    return  m_formatCombox->currentIndex();
}

QString CExportImageDialog::getImageFormate() const
{
    return  m_saveFormat;
}

int CExportImageDialog::getQuality() const
{
    return m_quality;
}

int CExportImageDialog::exec()
{
    quitRet = 1;
    m_fileNameEdit->setText(tr("Unnamed"));
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

Exec:
    quitRet = DDialog::exec();

    if (quitRet == 1) {
        int ret = execCheckFile(resultFile());

        if (ret == EReExec) {
            goto Exec;
        }
        quitRet = ret;
    }

    return quitRet;
}

int CExportImageDialog::execFor(const Page *page)
{
    d_pri()->resetImageSettingSizeTo(page->pageRect().size().toSize());

    if (exec() == 1) {
        if (page->saveToImage(resultFile(), desImageSize(), getQuality()))
            return 1;
        else {
            return 0;
        }
    }
    return -1;
}


QString CExportImageDialog::resultFile() const
{
    if (quitRet == 1)
        return getCompleteSavePath();

    return "";
}

QSize CExportImageDialog::desImageSize() const
{
    return QSize(d_pri()->_widthEditor->text().toInt(), d_pri()->_heightEditor->text().toInt());
}

void CExportImageDialog::initUI()
{
    setWgtAccesibleName(this, "Export dialog");
    //setFixedSize(DIALOG_SIZE);
    resize(DIALOG_SIZE);
    setModal(true);

    setContentsMargins(0, 0, 0, 0);

    CSvgLabel *logoLable = new CSvgLabel(":/theme/common/images/logo.svg", this);
    logoLable->setFixedSize(QSize(32, 32));


    logoLable->move(10, 9);
    logoLable->setAlignment(Qt::AlignLeft);

    setWindowTitle(tr("Export"));;


    m_fileNameEdit = new DLineEdit(this);
    setWgtAccesibleName(m_fileNameEdit, "Export name line editor");
    //m_fileNameEdit->setFixedSize(LINE_EDIT_SIZE);
    m_fileNameEdit->setClearButtonEnabled(false);
    //编译器会对反斜杠进行转换，要想在正则表达式中包括一个\，需要输入两次，例如\\s。要想匹配反斜杠本身，需要输入4次，比如\\\\。
    m_fileNameEdit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[^\\\\ /:*?\"<>|]+"), m_fileNameEdit->lineEdit()));

    m_savePathCombox = new DComboBox(this);
    setWgtAccesibleName(m_savePathCombox, "Export path comboBox");
    m_savePathCombox->insertItem(Pictures, tr("Pictures"));
    if (!Application::isTabletSystemEnvir()) {
        m_savePathCombox->insertItem(Documents, tr("Documents"));
        m_savePathCombox->insertItem(Downloads, tr("Downloads"));
        m_savePathCombox->insertItem(Desktop, tr("Desktop"));
        m_savePathCombox->insertItem(Videos, tr("Videos"));
        m_savePathCombox->insertItem(Music, tr("Music"));
        m_savePathCombox->insertItem(UsrSelect, tr("Select other directories"));
    }
    //m_savePathCombox->setFixedSize(LINE_EDIT_SIZE);


    m_formatCombox = new DComboBox(this);
    setWgtAccesibleName(m_formatCombox, "Export format comboBox");
    m_formatCombox->insertItem(JPG, tr("jpg"));
    m_formatCombox->insertItem(PNG, tr("png"));
    m_formatCombox->insertItem(BMP, tr("bmp"));
    m_formatCombox->insertItem(TIF, tr("tif"));
    m_formatCombox->insertItem(PDF, tr("pdf"));
    //m_formatCombox->setFixedSize(LINE_EDIT_SIZE);

    m_qualitySlider = new DSlider(Qt::Horizontal, this);
    setWgtAccesibleName(m_qualitySlider, "Export quality slider");
    m_qualitySlider->setMinimum(1);
    m_qualitySlider->setMaximum(100);
    m_qualitySlider->setValue(100);
    m_qualitySlider->setFixedSize(QSize(120, LINE_EDIT_SIZE.height()));

    m_qualityLabel = new DLabel(this);

    QHBoxLayout *qualityHLayout = new QHBoxLayout;
    qualityHLayout->setMargin(0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addSpacing(3);
    qualityHLayout->addWidget(m_qualitySlider);
    qualityHLayout->addSpacing(20);
    qualityHLayout->addWidget(m_qualityLabel);

    DWidget *contentWidget = new DWidget(this);
    setWgtAccesibleName(contentWidget, "Export content widget");
    contentWidget->setContentsMargins(0, 0, 0, 0);
    QFormLayout *fLayout = new QFormLayout(contentWidget);
    fLayout->setFormAlignment(Qt::AlignJustify);
    fLayout->setHorizontalSpacing(10);
    fLayout->addRow(tr("Name:"), m_fileNameEdit);
    fLayout->addRow(tr("Save to:"), m_savePathCombox);
    fLayout->addRow(tr("Format:"), m_formatCombox);
    fLayout->addRow(tr("Quality:"), qualityHLayout);

    d_pri()->initSizeSettingLayoutUi(fLayout, contentWidget);

    addContent(contentWidget);

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    m_saveBtnId = addButton(tr("Save"), true, DDialog::ButtonRecommend);

    m_questionDialog = new DDialog(this);
    m_questionDialog->setIcon(QIcon::fromTheme("dialog-warning"));
    m_questionDialog->setModal(true);
    m_questionDialog->addButton(tr("Cancel"));
    m_questionDialog->addButton(tr("Replace"), false, DDialog::ButtonWarning);
}

void CExportImageDialog::initConnection()
{
    connect(m_savePathCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnSavePathChange(int)), Qt::QueuedConnection);
    connect(m_formatCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnFormatChange(int)));
    //connect(this, &DDialog::buttonClicked, this, &CExportImageDialog::execCheckFile);
    connect(m_qualitySlider, SIGNAL(valueChanged(int)), this, SLOT(slotOnQualityChanged(int)));
    //connect(m_questionDialog, &DDialog::buttonClicked, this, &CExportImageDialog::slotOnQuestionDialogButtonClick);

    //设置的文件名为空时应该要设置保存按钮为disable
    connect(m_fileNameEdit, &DLineEdit::textChanged, this, [ = ](const QString & text) {

        QString newText = text;

        //        newText.remove(dApp->fileNameRegExp());

        //        m_fileNameEdit->blockSignals(true);
        //        m_fileNameEdit->setText(newText);
        //        m_fileNameEdit->blockSignals(false);

        bool isEmpty = newText.isEmpty();

        if (m_saveBtnId != -1) {
            QAbstractButton *pBtn = getButton(m_saveBtnId);

            if (pBtn != nullptr) {
                pBtn->setEnabled(!isEmpty);
            }
        }
    });

}

void CExportImageDialog::slotOnSavePathChange(int index)
{
    switch (index) {
    case Pictures:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (Application::isTabletSystemEnvir()) {
            m_savePath += "/Drawings";
            QDir dir(m_savePath);
            if (!dir.exists()) {
                dir.mkdir(m_savePath);
            }
        }
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
    m_fileNameEdit->setText(name);
}

int CExportImageDialog::execCheckFile(const QString &text)
{
    //  [BUG: 30843] 希望在导出时，名字以点开头的图片，也能给到提示：以点开始会被隐藏
    QString fileName = m_fileNameEdit->text().trimmed();
    if (fileName.startsWith(".")) {
        Dialog dialog(this);
        dialog.setModal(true);
        dialog.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));
        dialog.setMessage(tr("This file will be hidden if the file name starts with a dot (.). Do you want to hide it?"));
        dialog.addButton(tr("Cancel"), false, DDialog::ButtonNormal);
        dialog.addButton(tr("Confirm"), true, DDialog::ButtonRecommend);
        dialog.showInCenter(this);
        int status = dialog.exec();
        if (status != 1) {
            return EReExec;
        }
    }

    // 判断路径是否超过255字符
    QString completePath = getCompleteSavePath();
    QFileInfo info(completePath);
    // NAME_MAX 文件名字最大长度
    if (info.fileName().toLocal8Bit().length() > NAME_MAX) {
        Dtk::Widget::DDialog dialog(this);
        dialog.setTextFormat(Qt::RichText);
        dialog.addButton(tr("OK"));
        dialog.setIcon(QIcon(":/icons/deepin/builtin/Bullet_window_warning.svg"));
        dialog.setMessage(tr("The file name is too long"));
        dialog.exec();
        return ECancel;
    }

    if (completePath == "") {
        return ECancel;
    }
    if (QFileInfo::exists(completePath)) {
        int ret = execFileIsExists(completePath);
        if (ret != 1) {
            return ECancel;
        }
    }
    return EOK;
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
        auto dirs = dialog.selectedFiles();
        QString fileDir = dirs.isEmpty() ? "" : dirs.first();
        if (!fileDir.isEmpty()) {
            if (m_savePathCombox->count() < Other + 1) {
                m_savePathCombox->insertItem(Other, fileDir);
            } else {
                m_savePathCombox->setItemText(Other, fileDir);
            }
            m_savePathCombox->setCurrentText(fileDir);
        }
    }
}

int CExportImageDialog::execFileIsExists(const QString &path)
{
    QString newStrMsg = path;
    QFontMetrics fontWidth(m_questionDialog->font());   //得到每个字符的宽度
    QString newPath = fontWidth.elidedText(newStrMsg, Qt::ElideRight, 350);   //最大宽度显示超出为省略号显示

    //“XXX”已经存在，您是否要替换？
    m_questionDialog->setMessage(QString(tr("%1 \n already exists, do you want to replace it?")).arg(newPath));
    return m_questionDialog->exec();
}

//bool CExportImageDialog::isHaveSuffix(const QString &src)
//{
//    bool flag = false;
//    if (src.endsWith(".jpg") || src.endsWith(".png") || src.endsWith(".bmp")
//            || src.endsWith(".tif") || src.endsWith(".pdf")) {
//        flag = true;
//    }
//    return flag;
//}

QString CExportImageDialog::getCompleteSavePath() const
{
    QString fileName = m_fileNameEdit->text().trimmed();

    if (fileName.isEmpty() || fileName == "") {
        return "";
    }

    fileName = fileName + "." + m_formatCombox->currentText();

    return m_savePath + "/" + fileName;
}

void CExportImageDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == (Qt::Key_Return) || event->key() == (Qt::Key_Enter)) {
        //if (d_pri()->isFocusInEditor())
        return;
    }
    return DDialog::keyPressEvent(event);
}
