#include "savedialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QObject>

const QSize DIALOG_SIZE = QSize(326, 221);

SaveDialog::SaveDialog(QWidget *parent)
    : Dialog(parent)
{
    setFixedSize(DIALOG_SIZE);
    setModal(true);

    setTitle(tr("Save"));
    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

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

    QComboBox* contentFormatCBox = new QComboBox(this);
    contentFormatCBox->addItems(fileFormat);

    QSlider* qualitySlider = new QSlider(Qt::Horizontal,this);
    qualitySlider->setMinimum(50);
    qualitySlider->setMaximum(100);
    qualitySlider->setFixedWidth(119);
    qualitySlider->setValue(qualitySlider->maximum());

    QLabel* valueLabel = new QLabel(this);

    QHBoxLayout* qualityHLayout = new QHBoxLayout;
    qualityHLayout->setMargin(0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addWidget(qualitySlider);
    qualityHLayout->addWidget(valueLabel);

    QWidget* w = new QWidget;
    QFormLayout* fLayout = new QFormLayout(w);
    fLayout->addRow(tr("Name"), imageEdit);
    fLayout->addRow(tr("Save to"), contentSaveCBox);
    fLayout->addRow(tr("Format"), contentFormatCBox);
    fLayout->addRow(tr("Quality"), qualityHLayout);

    addContent(w);

    connect(contentSaveCBox, &QComboBox::currentTextChanged, this, [=](QString dir){
        if (dir == tr("Select other directories")) {
            m_fileDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    });

    connect(contentFormatCBox, &QComboBox::currentTextChanged, this,
            [=](QString format){
        QString name = imageEdit->text();
        QString suffix = QFileInfo(name).suffix();
        name = imageEdit->text().remove(suffix) + format.toLower();
        imageEdit->setText(name);
    });

    connect(this, &SaveDialog::buttonClicked, this, [=](int index) {
        if (m_fileDir.isEmpty())
        {
            m_fileDir = getSaveDir(contentSaveCBox->currentText());
        }

        if (QFileInfo(imageEdit->text()).suffix().isEmpty())
        {
            m_filePath = QString("%1/%2.%3").arg(m_fileDir).arg(
                        imageEdit->text()).arg(contentFormatCBox->currentText().toLower());
        } else {
            m_filePath = QString("%1/%2").arg(m_fileDir).arg(imageEdit->text());
        }
        if (index == 1) {
            emit saveToPath(m_filePath);
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

void SaveDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        this->close();
    }
}
