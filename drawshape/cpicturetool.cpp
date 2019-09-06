#include "cpicturetool.h"

#include <QDebug>
#include <QtConcurrent>


CPictureTool::CPictureTool(QWidget *parent)
    : QWidget (parent)
{
    m_progressLayout = new ProgressLayout();
    connect(this, SIGNAL(addImageSignal(QPixmap, int, CDrawScene *, QWidget *)), this, SLOT(addImages(QPixmap, int, CDrawScene *, QWidget *)));

}
CPictureTool::~CPictureTool()
{

}


void CPictureTool::drawPicture(CDrawScene *scene, QWidget *centralWindow)
{
    //告知单例正在画图模式s
    // CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(importPicture);
    //qDebug() << "importImageSignal" <<  "drawPicture" << endl;
    DFileDialog *fileDialog = new DFileDialog();
    QStringList filters;
    filters << "Image files (*.png *.jpg *.bmp *.tif *.pdf *.ddf)";
    fileDialog->setNameFilters(filters);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    if (fileDialog->exec() ==   QDialog::Accepted) {


        QStringList filenames = fileDialog->selectedFiles();
        // qDebug() << filenames << endl;
        m_picNum = filenames.size();

        m_progressLayout->setRange(0, m_picNum);

        m_progressLayout->showInCenter(centralWindow);
        //progressLayout->show();//此处还需要找个show的zhaiti载体

        //启动图片导入线程
        QtConcurrent::run([ = ] {
            for (int i = 0; i < m_picNum; i++)
            {

                QPixmap pixmap = QPixmap (filenames[i]);

                emit addImageSignal(pixmap, i + 1, scene, centralWindow);
                // emit loadImageNum(i + 1);
                //qDebug() << "importProcessbar" << i + 1 << endl;

            }

        });


    }

}


void CPictureTool::addImages(QPixmap pixmap, int itemNumber, CDrawScene *scene, QWidget *centralWindow)
{
    scene->clearSelection();
    //CPictureItem *pixmapItem = new CPictureItem(QRectF(0, 0, centralWindow->width(), centralWindow->height()), pixmap);
    CPictureItem *pixmapItem = new CPictureItem(QRectF(0, 0, pixmap.width(), pixmap.height()), pixmap);

    pixmapItem->setSelected(false);
    scene->addItem(pixmapItem);
    //m_picturetItems << pixmapItem;
    m_progressLayout->setProgressValue(itemNumber);
    if (itemNumber == m_picNum) {
        m_progressLayout->close();
        delete m_progressLayout;
        pixmapItem->setSelected(true);
    }


}



