#include "savedialog.h"

#include "utils/baseutils.h"
#include "utils/tempfile.h"
#include "utils/drawfile.h"
#include "utils/configsettings.h"

#include <DLabel>
#include <DLineEdit>
#include <QPdfWriter>
#include <QFormLayout>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <DFileDialog>
#include <QTimer>
#include <QObject>

const QSize DIALOG_SIZE = QSize(330, 221);
const QSize LINE_EDIT_SIZE = QSize(178, 22);

SaveDialog::SaveDialog(QList<QPixmap> pixs, DWidget *parent)
    : Dialog(parent)
    , m_closeDialog(false)
{
    setFixedSize(DIALOG_SIZE);
    setModal(true);

    DLabel *m_titleLabel = new DLabel(tr("Save"), this);
    m_titleLabel->setObjectName("TitleLabel");
//    m_titleLabel->setStyleSheet("QLabel#TitleLabel {"
//                                "font-size: 14px;"
//                                "font-weight: medium;}");
    m_titleLabel->setFixedSize(330, 40);
    m_titleLabel->move(0, 0);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    m_pixmaps = pixs;

    DLineEdit *imageEdit = new DLineEdit(this);
    imageEdit->setFixedSize(LINE_EDIT_SIZE);
    connect(this, &SaveDialog::imageNameChanged, this, [ = ](QString name) {
        imageEdit->setText(name);
    });

    QStringList saveDirs;
    saveDirs << tr("Pictures") << tr("Documents") << tr("Downloads")
             << tr("Desktop") << tr("Videos")         << tr("Music")
             << tr("Select other directories");

    m_contentSaveDirCBox = new DComboBox(this);
    m_contentSaveDirCBox->addItems(saveDirs);
    m_contentSaveDirCBox->setFixedSize(LINE_EDIT_SIZE);
    m_lastDir = m_contentSaveDirCBox->currentText();

    QStringList fileFormat;
    fileFormat << tr("PNG") << tr("DDF") << tr("JPG") << tr("BMP")
               << tr("TIF") << "PDF";

    m_contentFormatCBox = new DComboBox(this);
    m_contentFormatCBox->setFixedSize(LINE_EDIT_SIZE);
    m_contentFormatCBox->addItems(fileFormat);

    m_imagePath = TempFile::instance()->getRandomFile("SaveFile",
                                                      QString(".%1").arg(m_contentFormatCBox->currentText().toLower()));
    if (m_contentFormatCBox->currentText() == "JPG") {
        if (pixs.length() == 2)
            pixs[1].save(m_imagePath);
    } else {
        if (pixs.length() > 1)
            pixs[0].save(m_imagePath);
    }

    m_qualitySlider = new DSlider(Qt::Horizontal, this);
    m_qualitySlider->setMinimum(50);
    m_qualitySlider->setMaximum(100);
    m_qualitySlider->setFixedWidth(125);
    m_qualitySlider->setValue(m_qualitySlider->maximum());

    m_valueLabel = new DLabel(this);
    m_valueLabel->setText(QString("%1").arg(sizeToHuman(
                                                QFileInfo(m_imagePath).size())));
    QTimer *timer = new QTimer(this);
    connect(m_qualitySlider, &QSlider::valueChanged, this, [ = ] {
        timer->start(500);
    });
    connect(timer, &QTimer::timeout, this, [ = ] {
        updateImageSize();
    });

    QHBoxLayout *qualityHLayout = new QHBoxLayout;
    qualityHLayout->setMargin(0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addWidget(m_qualitySlider);
    qualityHLayout->addSpacing(3);
    qualityHLayout->addWidget(m_valueLabel);

    DWidget *w = new DWidget(this);
    QFormLayout *fLayout = new QFormLayout(w);
    fLayout->setFormAlignment(Qt::AlignJustify);
    fLayout->setHorizontalSpacing(29);
    fLayout->addRow(tr("Name:"), imageEdit);
    fLayout->addRow(tr("Save to:"), m_contentSaveDirCBox);
    fLayout->addRow(tr("Format:"), m_contentFormatCBox);
    fLayout->addRow(tr("Quality:"), qualityHLayout);
    addContent(w);

    connect(m_contentSaveDirCBox, &QComboBox::currentTextChanged, this, [ = ](QString dir) {
        if (dir == tr("Select other directories")) {
            DFileDialog dialog(this);
            dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
            dialog.setViewMode(DFileDialog::Detail);
            dialog.setFileMode(DFileDialog::DirectoryOnly);
            if (dialog.exec()) {
                QString fileDir = dialog.selectedFiles()[0];
                m_contentSaveDirCBox->setInsertPolicy(DComboBox::InsertBeforeCurrent);
                m_contentSaveDirCBox->addItem(fileDir);
                m_contentSaveDirCBox->setCurrentText(fileDir);
            } else {
                m_contentSaveDirCBox->setCurrentText(m_lastDir);
            }
        } else {
            m_lastDir = m_contentSaveDirCBox->currentText();
        }
    });

    connect(m_contentFormatCBox, &DComboBox::currentTextChanged, this,
    [ = ](QString format) {
        QString name = imageEdit->text();
        QString suffix = QFileInfo(name).suffix();
        name = imageEdit->text().remove(suffix) + format.toLower();
        imageEdit->setText(name);

        if (format == "DDF" || format == "BMP" || format == "TIF" || format == "PDF") {
            m_qualitySlider->setDisabled(true);
            m_valueLabel->setDisabled(true);
        } else {
            m_qualitySlider->setDisabled(false);
            m_valueLabel->setDisabled(false);
        }

        QString imageSuffix = QFileInfo(m_imagePath).suffix();
        if (format == "PDF" || format == "DDF")
            m_imagePath = m_imagePath.remove(imageSuffix) + "png";
        else
            m_imagePath = m_imagePath.remove(imageSuffix) + format.toLower();
        updateImageSize();
    });

    connect(this, &SaveDialog::buttonClicked, this, [ = ](int index) {
        if (m_fileDir.isEmpty()) {
            m_fileDir = getSaveDir(m_contentSaveDirCBox->currentText());
        }

        if (QFileInfo(imageEdit->text()).suffix().isEmpty()) {
            m_filePath = QString("%1/%2.%3").arg(m_fileDir).arg(
                             imageEdit->text()).arg(m_contentFormatCBox->currentText().toLower());
        } else {
            m_filePath = QString("%1/%2").arg(m_fileDir).arg(imageEdit->text());
        }
        if (index == 1) {

            if (QFileInfo(m_filePath).exists()) {
                Dialog *dialog = new Dialog(this);
                dialog->setModal(true);
                dialog->setMessage((QString(tr("%1 already exists, do you want "
                                               "to replace?")).arg(m_filePath)));
                dialog->addButtons(QStringList() << tr("Cancel") << tr("Replace"));
                dialog->showInCenter(window());

                connect(dialog, &Dialog::buttonClicked, this, [ = ](int index, const QString & text) {
                    Q_UNUSED(text);
                    if (index == 1) {
                        m_closeDialog = true;
                        setVisible(false);
                        saveImage(m_filePath);
                    }
                    dialog->hide();
                    dialog->deleteLater();
                });
            } else {
                m_closeDialog = true;
                setVisible(false);
                saveImage(m_filePath);
            }
        } else {
            m_closeDialog = true;
            setVisible(false);
        }
    });

    imageEdit->setText(createSaveBaseName());
}

QString SaveDialog::getSaveDir(QString dir)
{
    if (dir == tr("Pictures")) {
        return QStandardPaths::writableLocation(
                   QStandardPaths::PicturesLocation);
    } else if (dir == tr("Documents")) {
        return QStandardPaths::writableLocation(
                   QStandardPaths::DocumentsLocation);
    } else if (dir == tr("Downloads")) {
        return QStandardPaths::writableLocation(
                   QStandardPaths::DownloadLocation);
    } else if (dir == tr("Desktop")) {
        return QStandardPaths::writableLocation(
                   QStandardPaths::DesktopLocation);
    } else if (dir == tr("Videos")) {
        return QStandardPaths::writableLocation(
                   QStandardPaths::MoviesLocation);
    } else if (dir == tr("Music")) {
        return QStandardPaths::writableLocation(
                   QStandardPaths::MusicLocation);
    }  else if (dir == tr("Select other directories")) {
        return "";
    } else {
        return dir;
    }
}

void SaveDialog::updateImageSize()
{
    int val = m_qualitySlider->value();
    QPixmap pixmap;
    if (m_contentFormatCBox->currentText() == "JPG") {
        pixmap = m_pixmaps[1];
    } else {
        pixmap = m_pixmaps[0];
    }
    int pixWidth = pixmap.size().width();
    int pixHeight = pixmap.size().height();
    qreal wid =  pixWidth * qreal(val) / qreal(100);
    qreal het = pixHeight * qreal(val) / qreal(100);
    QPixmap scaledPix = pixmap.scaled(int(wid), int(het),
                                      Qt::KeepAspectRatio, Qt::FastTransformation);
    scaledPix = scaledPix.scaled(pixmap.size(),
                                 Qt::KeepAspectRatio, Qt::FastTransformation);
    scaledPix.save(m_imagePath);
    m_valueLabel->setText(QString("%1").arg(
                              sizeToHuman(QFileInfo(m_imagePath).size())));
}

void SaveDialog::saveImage(const QString &path)
{
    if (m_contentFormatCBox->currentText() == "DDF") {
        DrawFile *drawFile = new DrawFile();
        QSize windowSize = QSize(
                               ConfigSettings::instance()->value("window", "width").toInt(),
                               ConfigSettings::instance()->value("window", "height").toInt()
                           );
        QSize canvasSize = QSize(
                               ConfigSettings::instance()->value("canvas", "width").toInt(),
                               ConfigSettings::instance()->value("canvas", "height").toInt()
                           );
        QSize artboardSize = QSize(
                                 ConfigSettings::instance()->value("artboard", "width").toInt(),
                                 ConfigSettings::instance()->value("artboard", "height").toInt()
                             );
        drawFile->createddf(windowSize, canvasSize, artboardSize, path,
                            TempFile::instance()->savedShapes());
    } else if (m_contentFormatCBox->currentText() == "PDF") {
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

    if (TempFile::instance()->saveFinishedExit())
        qApp->quit();
}

QString SaveDialog::createSaveBaseName()
{
    const QString nan = tr("Unnamed");
    int num = 0;
    QString filePath = QString("%1/%2.%3").arg(getSaveDir(
                                                   m_contentSaveDirCBox->currentText())).arg(nan).arg(
                           m_contentFormatCBox->currentText().toLower());
    while (QFileInfo(filePath).exists()) {
        num++;
        QString tail = QString("%1_%2").arg(nan).arg(num);
        filePath = QString("%1/%2.%3").arg(getSaveDir(
                                               m_contentSaveDirCBox->currentText())).arg(tail).arg(
                       m_contentFormatCBox->currentText().toLower());
    }
    return QFileInfo(filePath).fileName();
}

void SaveDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}

void SaveDialog::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    m_closeDialog = true;
    this->setVisible(false);
}

void SaveDialog::setVisible(bool visible)
{
    if (visible) {
        Dialog::setVisible(visible);
    } else {
        if (m_closeDialog)
            Dialog::setVisible(visible);
    }
}
