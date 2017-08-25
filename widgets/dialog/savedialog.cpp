#include "savedialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QObject>

SaveDialog::SaveDialog(QWidget *parent)
    : Dialog(parent)
{
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
    QLabel* valueLabel = new QLabel(this);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(qualitySlider);
    hLayout->addWidget(valueLabel);

    QWidget* w = new QWidget;
    QFormLayout* fLayout = new QFormLayout(w);
    fLayout->addRow(tr("Name"), imageEdit);
    fLayout->addRow(tr("Save to"), contentSaveCBox);
    fLayout->addRow(tr("Format"), contentFormatCBox);
    fLayout->addRow(tr("Quality"), hLayout);

    addContent(w);

    connect(this, &SaveDialog::buttonClicked, this, [=](int index){
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
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}
