#include "cpicturetool.h"

#include <QDebug>
#include <QtConcurrent>
#include <DMessageBox>
#include <QGraphicsItem>


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
        int exitPicNum = 0;

        //获取已导入图片数量
        QList<QGraphicsItem *> items = scene->items();

        if ( items.count() != 0 ) {
            for (int i = 0; i < items.size(); i++) {
                if (items[i]->type() == PictureType) {
                    exitPicNum = exitPicNum + 1;
                };

            }

        }

        //大于30张报错，主要是适应各种系统环境，不给内存太大压力
        if (exitPicNum + m_picNum > 30) {
            // DMessageBox messageBox= new DMessageBox();
            DMessageBox::warning(nullptr, tr("Import warning"), tr("30 pictures are allowed to be imported at most, "
                                                                   "already imported %1 pictures, please try again").arg(exitPicNum));
            return;

        }

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

    //调整图片在画板中显示的大小
    int width = pixmap.width();
    int height = pixmap.height();
    int widgetWidth = scene->width();
    int widgetHeight = scene->height();
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
    //qDebug() << "picture size" << scale << pixmap.width() << pixmap.height() << centralWindow->width() << centralWindow->height() << width << height << endl;

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



