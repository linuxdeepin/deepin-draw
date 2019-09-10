#include "cexportimagedialog.h"

#include <DFileDialog>

#include <QFormLayout>
#include <QImageWriter>
#include <QStandardPaths>
#include <QPdfWriter>
#include <QPainter>
#include <QDebug>

const QSize DIALOG_SIZE = QSize(330, 230);
const QSize LINE_EDIT_SIZE = QSize(178, 30);

CExportImageDialog::CExportImageDialog(DWidget *parent)
    : DDialog(parent)
{
    initUI();
    initConnection();
    //blockSignals
}

CExportImageDialog::~CExportImageDialog()
{

}

void CExportImageDialog::showMe(const QPixmap &pixmap)
{
    m_saveImage = pixmap;

    m_fileNameEdit->setText("deepin.jpg");

    if (m_savePathCombox->count() == Other + 1) {
        m_savePathCombox->removeItem(Other);
    }

    m_savePathCombox->setCurrentIndex(Pictures);
    m_formatCombox->setCurrentIndex(JPG);

    slotOnSavePathChange(Pictures);
    slotOnFormatChange(JPG);

    show();
}

void CExportImageDialog::initUI()
{
    setFixedSize(DIALOG_SIZE);
    setModal(true);

    DLabel *titleLabel = new DLabel(tr("Save"), this);
    titleLabel->setFixedSize(DIALOG_SIZE.width(), 40);
    titleLabel->move(0, 0);
    titleLabel->setAlignment(Qt::AlignCenter);


    m_fileNameEdit = new DLineEdit(this);
    m_fileNameEdit->setFixedSize(LINE_EDIT_SIZE);


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
    m_qualitySlider->setMaximum(1);
    m_qualitySlider->setMaximum(100);
    m_qualitySlider->setValue(100);
    m_qualitySlider->setFixedSize(QSize(135, LINE_EDIT_SIZE.height()));

    m_qualityLabel = new DLabel(this);
    m_qualityLabel->setText(QString("%1").arg(m_qualitySlider->value()));
    m_qualityLabel->setFixedHeight(LINE_EDIT_SIZE.height());

    QHBoxLayout *qualityHLayout = new QHBoxLayout;
    qualityHLayout->setMargin(0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addWidget(m_qualitySlider);
    qualityHLayout->addSpacing(10);
    qualityHLayout->addWidget(m_qualityLabel);

    DWidget *contentWidget = new DWidget(this);
    QFormLayout *fLayout = new QFormLayout(contentWidget);
    fLayout->setFormAlignment(Qt::AlignJustify);
    fLayout->setHorizontalSpacing(25);
    fLayout->addRow(tr("Name:"), m_fileNameEdit);
    fLayout->addRow(tr("Save to:"), m_savePathCombox);
    fLayout->addRow(tr("Format:"), m_formatCombox);
    fLayout->addRow(tr("Quality:"), qualityHLayout);
    addContent(contentWidget);

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);
}

void CExportImageDialog::initConnection()
{
    connect(m_savePathCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnSavePathChange(int)));
    connect(m_formatCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnFormatChange(int)));
    connect(this, SIGNAL(buttonClicked(int, const QString & )), this, SLOT(slotOnDialogButtonClick(int, const QString & )));
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
    m_saveFormat = m_formatCombox->itemText(index);

    QString name = m_fileNameEdit->text().trimmed();

    name = name.mid(0, name.lastIndexOf(".") + 1);

    name += m_saveFormat;

    m_fileNameEdit->setText(name);

//    switch (index) {
//    case JPG:
//        m_saveFormat = m_formatCombox->itemText(index);
//        break;
//    case PNG:

//        break;
//    case BMP:

//        break;
//    case TIF:

//        break;
//    case PDF:

//        break;
//    default:
//        break;
    //    }
}

void CExportImageDialog::slotOnDialogButtonClick(int index, const QString &text)
{
    Q_UNUSED(text)

    if (index == 1) {
        if (m_formatCombox->currentIndex() == PDF) {
            QString completePath = m_savePath + "/" + m_fileNameEdit->text().trimmed();
            QPdfWriter writer(completePath);
            int ww = writer.width();
            int wh = writer.height();
            QPainter painter(&writer);
            painter.drawPixmap(0, 0, QPixmap(m_saveImage).scaled(QSize(ww, wh), Qt::KeepAspectRatio));
        } else {
            QString completePath = m_savePath + "/" + m_fileNameEdit->text().trimmed();
            QString formate = m_saveFormat;
            bool isSuccess = m_saveImage.save(completePath, m_saveFormat.toUpper().toLocal8Bit().data(), 100);
            qDebug() << "!!!!!!!!!" << isSuccess;
        }
    }
    hide();
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
