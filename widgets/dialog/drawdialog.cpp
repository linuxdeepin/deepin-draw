#include "drawdialog.h"
#include "utils/baseutils.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

const QSize DIALOG_SIZE = QSize(420, 120);

DrawDialog::DrawDialog(QWidget* parent)
    :Dialog(parent)
{
    setModal(true);
    setFixedSize(DIALOG_SIZE);
    setIconPixmap(QPixmap(":/theme/common/images/deepin-draw-64.png"));

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Discard"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    // Input content
    const QString subStyle = getFileContent(":/drawdialog.qss");
    QLabel *title = new QLabel(tr("Save the current contents?"), this);
    title->setStyleSheet(subStyle);
    title->setObjectName("DialogTitle");
    title->setAlignment(Qt::AlignLeft);

    QWidget *w = new QWidget(this);
    w->setFixedHeight(this->height() - 60);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    addContent(w);

    connect(this, &DrawDialog::buttonClicked, this, [=](int id){
        qDebug() << "deepin-draw button clicked:" << id;
        if (id == 0)
        {
            this->close();
        }
        else if (id == 1)
        {
            qApp->quit();
        } else {
            emit saveDrawImage();
        }

    });
}

void  DrawDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}
