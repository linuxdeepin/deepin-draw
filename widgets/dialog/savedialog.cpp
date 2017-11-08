#include "savedialog.h"

#include "utils/baseutils.h"
#include "utils/tempfile.h"

#include <QLabel>
#include <QLineEdit>

#include <QPdfWriter>
#include <QFormLayout>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QTimer>
#include <QObject>

const QSize DIALOG_SIZE = QSize(326, 221);

SaveDialog::SaveDialog(const QPixmap &pix, QWidget *parent)
    : Dialog(parent)
{
    setFixedSize(DIALOG_SIZE);
    setModal(true);

    setTitle(tr("Save"));
    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    m_pixmap = pix;

    QLineEdit* imageEdit = new QLineEdit(this);
    connect(this, &SaveDialog::imageNameChanged, this, [=](QString name){
        imageEdit->setText(name);
    });

    QStringList saveDirs;
    saveDirs << tr("Pictures") << tr("Documents") << tr("Downloads")
             << tr("Desktop") << tr("Videos")         << tr("Music")
             << tr("Select other directories");

    QComboBox* contentSaveCBox = new QComboBox(this);
    contentSaveCBox->addItems(saveDirs);

    QStringList fileFormat;
    fileFormat << tr("PNG") << tr("DDF") << tr("JPG") << tr("BMP")
               << tr("TIF") << "PDF";

    m_contentFormatCBox = new QComboBox(this);
    m_contentFormatCBox->addItems(fileFormat);

   m_imagePath = TempFile::instance()->getRandomFile("SaveFile",
            QString(".%1").arg(m_contentFormatCBox->currentText().toLower()));
    pix.save(m_imagePath);
    m_qualitySlider = new QSlider(Qt::Horizontal,this);
    m_qualitySlider->setMinimum(50);
    m_qualitySlider->setMaximum(100);
    m_qualitySlider->setFixedWidth(119);
    m_qualitySlider->setValue(m_qualitySlider->maximum());

    m_valueLabel = new QLabel(this);
    m_valueLabel->setText(QString("%1").arg(sizeToHuman(
                                              QFileInfo(m_imagePath).size())));
    QTimer* timer = new QTimer(this);
    connect(m_qualitySlider, &QSlider::valueChanged, this, [=]{
        timer->start(500);
    });
    connect(timer, &QTimer::timeout, this, [=]{
        updateImageSize();
    });

    QHBoxLayout* qualityHLayout = new QHBoxLayout;
    qualityHLayout->setMargin(0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addWidget(m_qualitySlider);
    qualityHLayout->addSpacing(30);
    qualityHLayout->addWidget(m_valueLabel);

    QWidget* w = new QWidget;
    QFormLayout* fLayout = new QFormLayout(w);
    fLayout->setHorizontalSpacing(20);
    fLayout->addRow(tr("Name"), imageEdit);
    fLayout->addRow(tr("Save to"), contentSaveCBox);
    fLayout->addRow(tr("Format"), m_contentFormatCBox);
    fLayout->addRow(tr("Quality"), qualityHLayout);

    addContent(w);

    connect(contentSaveCBox, &QComboBox::currentTextChanged, this, [=](QString dir){
        if (dir == tr("Select other directories")) {
            m_fileDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    });

    connect(m_contentFormatCBox, &QComboBox::currentTextChanged, this,
            [=](QString format){
        QString name = imageEdit->text();
        QString suffix = QFileInfo(name).suffix();
        name = imageEdit->text().remove(suffix) + format.toLower();
        imageEdit->setText(name);

        QString imageSuffix = QFileInfo(m_imagePath).suffix();
        if (format == "PDF" || format == "DDF")
            m_imagePath = m_imagePath.remove(imageSuffix) + "png";
        else
            m_imagePath = m_imagePath.remove(imageSuffix) + format.toLower();
        updateImageSize();
    });

    connect(this, &SaveDialog::buttonClicked, this, [=](int index) {
        if (m_fileDir.isEmpty())
        {
            m_fileDir = getSaveDir(contentSaveCBox->currentText());
        }

        if (QFileInfo(imageEdit->text()).suffix().isEmpty())
        {
            m_filePath = QString("%1/%2.%3").arg(m_fileDir).arg(
                        imageEdit->text()).arg(m_contentFormatCBox->currentText().toLower());
        } else {
            m_filePath = QString("%1/%2").arg(m_fileDir).arg(imageEdit->text());
        }
        if (index == 1) {
            saveImage(m_filePath);
            this->close();
        }
    });
}

QString SaveDialog::getSaveDir(QString dir)
{
    if (dir == "Documents")
    {
        return QStandardPaths::writableLocation(
                    QStandardPaths::DocumentsLocation);
    } else if (dir == "Downloads")
    {
        return QStandardPaths::writableLocation(
                    QStandardPaths::DownloadLocation);
    } else if (dir == "Desktop")
    {
        return QStandardPaths::writableLocation(
                    QStandardPaths::DesktopLocation);
    } else if (dir == "Videos")
    {
        return QStandardPaths::writableLocation(
                    QStandardPaths::MoviesLocation);
    } else if (dir == "Music")
    {
        return QStandardPaths::writableLocation(
                    QStandardPaths::MusicLocation);
    }  else
    {
        return QStandardPaths::writableLocation(
                    QStandardPaths::PicturesLocation);
    }
}

void SaveDialog::updateImageSize()
{
    int val = m_qualitySlider->value();
    int pixWidth = m_pixmap.size().width();
    int pixHeight = m_pixmap.size().height();
    qreal wid =  pixWidth*qreal(val)/qreal(100);
    qreal het = pixHeight*qreal(val)/qreal(100);
    QPixmap scaledPix = m_pixmap.scaled(int(wid), int(het),
                                   Qt::KeepAspectRatio, Qt::FastTransformation);
    scaledPix = scaledPix.scaled(m_pixmap.size(),
                                 Qt::KeepAspectRatio, Qt::FastTransformation);
    scaledPix.save(m_imagePath);
    m_valueLabel->setText(QString("%1").arg(
                            sizeToHuman(QFileInfo(m_imagePath).size())));
}

void SaveDialog::saveImage(const QString &path)
{
    if (m_contentFormatCBox->currentText() == "DDF")
    {

    } else if (m_contentFormatCBox->currentText() == "PDF")
    {
                QPdfWriter writer(path);
                 int ww = writer.width();
                 int wh = writer.height();
                QPainter painter(&writer);
                qDebug() << "pdf save image:" << m_imagePath;
                painter.drawPixmap(0, 0, QPixmap(m_imagePath).scaled(
                    QSize(ww, wh), Qt::KeepAspectRatio));
    } else {
        QPixmap(m_imagePath).save(path);
    }
}

void SaveDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        this->close();
    }
}
