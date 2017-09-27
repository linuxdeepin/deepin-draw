#include "savedialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
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

    //    QHBoxLayout* titleHLayout = new QHBoxLayout;
    //    titleHLayout->setMargin(0);
    //    titleHLayout->setSpacing(0);
    //    titleHLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

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

    connect(this, &SaveDialog::buttonClicked, this, [=](int index) {
        QString filePath = QString("%1/%2").arg(QStandardPaths::writableLocation(
                                                    QStandardPaths::PicturesLocation)).arg(imageEdit->text());
        qDebug() << "filePath:" << filePath;
        if (index == 1) {
            emit saveToPath(filePath);
            this->close();
        }
    });
}

void SaveDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        this->close();
    }
}
