#include "drawdialog.h"
#include "utils/baseutils.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

DrawDialog::DrawDialog(QWidget* parent)
    :Dialog(parent)
{
    setModal(true);

    setIconPixmap(QPixmap(":/theme/common/images/deepin-draw-64.png"));

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Discard"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    // Input content
    const QString subStyle = getFileContent(":/drawdialog.qss");

    QLabel *title = new QLabel(tr("Save the current contents"), this);
    title->setStyleSheet(subStyle);
    title->setObjectName("DialogTitle");
    title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QWidget *w = new QWidget(this);
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
