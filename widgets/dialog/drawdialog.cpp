#include "drawdialog.h"
#include "utils/baseutils.h"
#include "savedialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

const QSize DIALOG_SIZE = QSize(420, 120);

DrawDialog::DrawDialog(DWidget *parent)
    : Dialog(parent)
{
    setModal(true);
    setFixedSize(DIALOG_SIZE);
    setIconPixmap(QPixmap(":/theme/common/images/deepin-draw-64.svg"));

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Discard"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    // Input content
    //const QString subStyle = getFileContent(":/drawdialog.qss");
    DLabel *title = new DLabel(tr("Save the current contents?"), this);
    //title->setStyleSheet(subStyle);
    title->setObjectName("DialogTitle");
    title->setAlignment(Qt::AlignLeft);

    DWidget *w = new DWidget(this);
    w->setFixedHeight(this->height() - 60);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    addContent(w);

    connect(this, &DrawDialog::buttonClicked, this, [ = ](int id) {
        qDebug() << "deepin-draw button clicked:" << id;
        if (id == 0) {
            this->close();
        } else if (id == 1) {
            qApp->quit();
        } else {
            emit saveDrawImage();

//            QList<QPixmap> pixs;
//            SaveDialog *saveDraw = new SaveDialog(pixs);
//            saveDraw->show();

        }

    });
}

void  DrawDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}

