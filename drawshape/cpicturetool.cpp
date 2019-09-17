#include "cpicturetool.h"

#include <QDebug>
#include <QtConcurrent>


CPictureTool::CPictureTool(DWidget *parent)
    : DWidget (parent)
{
    m_progressLayout = new ProgressLayout();
    connect(this, SIGNAL(addImageSignal(QPixmap, int, CDrawScene *, CCentralwidget *)), this, SLOT(addImages(QPixmap, int, CDrawScene *, CCentralwidget *)));

}
CPictureTool::~CPictureTool()
{

}


void CPictureTool::drawPicture(CDrawScene *scene, CCentralwidget *centralWindow)
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

        m_progressLayout->showInCenter(centralWindow->window());
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


void CPictureTool::addImages(QPixmap pixmap, int itemNumber, CDrawScene *scene, CCentralwidget *centralWindow)
{
    scene->clearSelection();
    //CPictureItem *pixmapItem = new CPictureItem(QRectF(0, 0, centralWindow->width(), centralWindow->height()), pixmap);
    int width = pixmap.width();
    int height = pixmap.height();
    int widgetWidth = centralWindow->width() - 58 ;
    int widgetHeight = centralWindow->height();
    if (height == 0) {
        return;
    }
    double scale = ((double)pixmap.width() / (double)pixmap.height());


    if (pixmap.width() > widgetWidth || pixmap.height() > widgetHeight) {
        if (scale >= (widgetWidth / widgetHeight)) {
            width = widgetWidth;
            height = (int)(width / scale);
        } else {
            height = widgetHeight;
            width = (int)(height * scale);
        }

    }
    qDebug() << "picture size" << scale << pixmap.width() << pixmap.height() << centralWindow->width() << centralWindow->height() << width << height << endl;

    CPictureItem *pixmapItem = new CPictureItem(QRectF(0, 0, width, height), pixmap);

    pixmapItem->setSelected(false);
    scene->addItem(pixmapItem);
    emit scene->itemAdded(pixmapItem);
    //m_picturetItems << pixmapItem;
    m_progressLayout->setProgressValue(itemNumber);
    if (itemNumber == m_picNum) {
        m_progressLayout->close();
        delete m_progressLayout;
        pixmapItem->setSelected(true);
    }


}



