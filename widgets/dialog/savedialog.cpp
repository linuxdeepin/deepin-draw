#include "savedialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>

SaveDialog::SaveDialog(QWidget *parent)
    : Dialog(parent)
{
    setModal(true);

    setTitle(tr("Save"));
    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    QLineEdit* contentFile = new QLineEdit(this);

    QStringList saveDirs;
    saveDirs << tr("Desktop") << tr("Documents") << tr("Downloads")
                      << tr("Pictures") << tr("Videos")         << tr("Music") << tr("Select other directories");
    QComboBox* contentSaveCBox = new QComboBox(this);
    contentSaveCBox->addItems(saveDirs);

    QStringList fileFormat;
    fileFormat << tr("PNG") << tr("JPG") << tr("BMP") << tr("TIF") << "PDF"
                         << tr("ddf");
    QComboBox* contentFormatCBox = new QComboBox(this);
    contentFormatCBox->addItems(fileFormat);

    QSlider* qualitySlider = new QSlider(Qt::Horizontal,this);
    QLabel* valueLabel = new QLabel(this);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(qualitySlider);
    hLayout->addWidget(valueLabel);

    QWidget* w = new QWidget(this);
    QFormLayout* fLayout = new QFormLayout(w);
    fLayout->addRow(tr("Name"), contentFile);
    fLayout->addRow(tr("Save to"), contentSaveCBox);
    fLayout->addRow(tr("Format"), contentFormatCBox);
    fLayout->addRow(tr("Quality"), hLayout);

    addContent(w);

}

void SaveDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}
