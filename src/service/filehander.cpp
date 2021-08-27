#include "filehander.h"
#include "application.h"
#include "cdrawparamsigleton.h"
#include "cdrawscene.h"
#include "cviewmanagement.h"

#include <QFile>
#include <QDataStream>
#include <QtConcurrent>
#include <QImageReader>
#include <QPdfWriter>
#include <QImageWriter>
using Future = QFuture<void>;

#define CURRENTTHREADID \
    reinterpret_cast<long long>(QThread::currentThreadId())
class FilePageHander::FilePageHander_private
{
public:
    FilePageHander_private(FilePageHander *hander): _hander(hander) {}
    ~FilePageHander_private()
    {
        foreach (auto f, _futures) {
            if (f.isRunning())
                f.waitForFinished();
        }
    }

    void addFuture(const Future &f, const qint64 &key)
    {
        qWarning() << "addFuture ======= " << key;
        QMutexLocker locker(&_mutex);
        _futures.insert(key, f);

        if (_futures.count() == 1)
            emit _hander->begin();
    }
    Future future(qint64 key)const
    {
        return _futures[key];
    }
    void removeFuture(const qint64 &key)
    {
        qWarning() << "removeFuture ======= " << key;
        QMutexLocker locker(&_mutex);
        _futures.remove(key);
        if (_futures.count() == 0)
            emit _hander->end();
    }
    template<class F>
    void run(F f, const QString &fileKey, bool syn = false)
    {
        if (syn) {
            //QSignalBlocker bloker(_hander);
            f();
        } else {
            Future *fure = new Future;
            *fure = QtConcurrent::run([ = ] {
                this->addFuture(*fure, CURRENTTHREADID);
                f();
                this->removeFuture(CURRENTTHREADID);
                QMetaObject::invokeMethod(_hander, [ = ]()
                {
                    delete fure;
                }, Qt::QueuedConnection);
            });
        }
    }

    void quit(const qint64 &fileKey)
    {
        auto f = _futures.find(fileKey);
        if (f != _futures.end()) {
            f->cancel();
        }
    }
    void quitAll()
    {
        for (auto fb = _futures.begin(); fb != _futures.end(); ++fb) {
            quit(fb.key());
        }
    }

private:

    FilePageHander *_hander;

    QMap<qint64, Future> _futures;

    QMutex _mutex;

    friend class FilePageHander;
};

/********************************************  LOAD STATIC FUNCTIONS ********************************************/
//keep compatibility
static void loadDdfWithNoCombinGroup(const QString &path, PageContext *contex, FilePageHander *hander, bool syn)
{
    QString error;
    int messageType = 0;
    emit hander->loadBegin();
    QFile readFile(path);
    if (readFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&readFile);

        CGraphics head;
        in >> head;
        qDebug() << QString("load ddf(%1)").arg(path) << " ddf version = " << head.version << "graphics count = " << head.unitCount <<
                 "scene size = " << head.rect;

        bool firstBlurFlag = true;
        bool firstDrawPen = true;
        for (int i = 0; i < head.unitCount; i++) {
            CGraphicsUnit unit;
            unit.reson = ESaveToDDf;
            in >> unit;

            if (TextType == unit.head.dataType) {
                contex->addSceneItem(unit);
            } else {
                //如果有老的模糊图元 那么进行交互提示模糊图元会被丢失，或者放弃加载
                bool foundBlur = (unit.head.dataType == BlurType);
                if (foundBlur && firstBlurFlag) {
                    firstBlurFlag = false;
                    bool finished = false;
                    if (!syn) {
                        QMetaObject::invokeMethod(hander, [ =, &finished, &messageType]() {
                            int ret = DrawBoard::exeMessage(QObject::tr("The blur effect will be lost as the file is in old version. Proceed to open it?"),
                                                            DrawBoard::EWarningMsg, false, QStringList() << QObject::tr("Open") << QObject::tr("Cancel"),
                                                            QList<int>() << 1 << 0);
                            if (ret == 1 || ret == -1) {
                                finished = true;
                                messageType = 1;
                            }
                        }, Qt::BlockingQueuedConnection);
                    }
                    if (finished) {
                        unit.release();
                        readFile.close();
                        contex->deleteLater();
                        contex = nullptr;
                        goto END;
                    }
                }

                bool foundDpen = (unit.head.dataType == PenType);
                if (foundDpen && firstDrawPen) {
                    firstDrawPen = false;
                    bool finished = false;
                    if (!syn) {
                        QMetaObject::invokeMethod(hander, [ =, &finished, &messageType]() {
                            int ret = DrawBoard::exeMessage(QObject::tr("The file is in an older version, and the properties of elements will be changed. " \
                                                                        "Proceed to open it?"),
                                                            DrawBoard::EWarningMsg, false, QStringList() << QObject::tr("Open") << QObject::tr("Cancel"),
                                                            QList<int>() << 1 << 0);
                            if (ret == 1 || ret == -1) {
                                finished = true;
                                messageType = 1;
                            }
                        }, Qt::BlockingQueuedConnection);
                    }
                    if (finished) {
                        unit.release();
                        readFile.close();
                        contex->deleteLater();
                        contex = nullptr;
                        goto END;
                    } else {
                        if (!(unit.head.pen.width() > 0)) {
                            unit.release();
                            emit hander->loadUpdate(i + 1, head.unitCount);
                            continue;
                        }
                    }
                }
                //EDdf5_9_0_3_LATER之后的版本不再存在模糊图元,所以如果加载EDdf5_9_0_3_LATER之前的ddf文件那么不用加载模糊
                if (unit.head.dataType != BlurType) {
                    contex->addSceneItem(unit);
                } else {
                    unit.release();
                }
            }
            emit hander->loadUpdate(i + 1, head.unitCount);
        }
        in >> head.version;
        readFile.close();
    }
END:
    if (contex != nullptr)
        contex->setFile(path);
    emit hander->loadEnd(contex, error, messageType);
}

CGroupBzItemsTreeInfo deserializationToTree_helper_1(QDataStream &inStream, int &outBzItemCount, int &outGroupCount, FilePageHander *hander,
                                                     bool &bcomplete, bool syn, bool &firstMeetPen, std::function<void(int, int)> f = nullptr)
{
    CGroupBzItemsTreeInfo result;
    int groupCount = 0;
    inStream >> groupCount;
    qDebug() << "read group count  = " << groupCount;
    for (int i = 0; i < groupCount; ++i) {
        CGroupBzItemsTreeInfo child = deserializationToTree_helper_1(inStream, outBzItemCount, outGroupCount, hander, bcomplete, syn, firstMeetPen, f);
        result.childGroups.append(child);
    }
    int bzItemCount = 0;
    inStream >> bzItemCount;
    //qDebug() << "read item count   = " << bzItemCount;
    for (int i = 0; i < bzItemCount; ++i) {
        CGraphicsUnit unit;
        unit.reson = ESaveToDDf;
        inStream >> unit;

        if (unit.head.dataType == PenType) {
            if (firstMeetPen) {
                firstMeetPen = false;
                if (!syn) {
                    bool finished = false;
                    QMetaObject::invokeMethod(hander, [ =, &finished]() {
                        int ret = DrawBoard::exeMessage(QObject::tr("The file is in an older version, and the properties of elements will be changed. " \
                                                                    "Proceed to open it?"),
                                                        DrawBoard::EWarningMsg, false, QStringList() << QObject::tr("Open") << QObject::tr("Cancel"),
                                                        QList<int>() << 1 << 0);
                        if (ret == 1) {
                            finished = true;
                        }
                    }, Qt::BlockingQueuedConnection);
                    if (finished) {
                        bcomplete = false;
                        unit.release();
                        return CGroupBzItemsTreeInfo();
                    }
                }
            }

            if (!(unit.head.pen.width() > 0)) {
                ++outBzItemCount;
                if (f != nullptr) {
                    f(outBzItemCount, outGroupCount);
                }
                continue;
            }
        }
        result.bzItems.append(unit);
        ++outBzItemCount;
        if (f != nullptr) {
            f(outBzItemCount, outGroupCount);
        }
    }
    CGraphicsUnit unit;
    unit.reson = ESaveToDDf;
    inStream >> unit;
    //qDebug() << "read group info   = " << unit.head.dataType;
    result.data = unit;
    ++outGroupCount;
    if (f != nullptr) {
        f(outBzItemCount, outGroupCount);
    }
    return result;
}
static void loadDdfWithCombinGroup(const QString &path, PageContext *contex, FilePageHander *hander, bool syn)
{
    emit hander->loadBegin();
    //QThread::sleep(3);
    QFile readFile(path);
    if (readFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&readFile);

        //1.反序列化到头结构中
        CGraphics head;
        in >> head;
        qint64 totalItemsCountHead = head.unitCount;
        //2.通知主线程设置当前场景的大小
        //emit signalStartLoadDDF(m_graphics.rect);
        QMetaObject::invokeMethod(hander, [ = ]() {
            contex->setPageRect(head.rect);
        }, Qt::QueuedConnection);

        //3.反序列化生成图元结构树,同时获取基本图元和组合图元的个数
        int bzItemsCount   = 0;
        int groupItemCount = 0;
        bool bcomplete = true;
        std::function<void(int, int)> fProcess = [ = ](int bzCount, int gpCount) -> void {
            if (totalItemsCountHead != 0)
                emit hander->loadUpdate(bzCount + gpCount, totalItemsCountHead);
        };
        bool firstMeetPen = true;
        CGroupBzItemsTreeInfo tree = deserializationToTree_helper_1(in, bzItemsCount, groupItemCount, hander, bcomplete, syn, firstMeetPen, fProcess);
        if (!bcomplete) { //选择不使用老版本
            readFile.close();
            contex->deleteLater();
            contex = nullptr;
            emit hander->loadEnd(contex, "");
            return;
        }
        int totalItemsCount = bzItemsCount + groupItemCount;
        if (totalItemsCountHead != totalItemsCount) {
            qWarning() << "get total items count from ddf head that = " << totalItemsCountHead;
            qWarning() << "get total items count from ddf deserialization items that = " << totalItemsCount;
            qWarning() << "items count from size not match deserialization items count !!! now we use the latter.";
        }

        //4.加载图元结构树(队列通知主线程加载)
        QMetaObject::invokeMethod(hander, [ = ]() {

            auto scene = contex->scene();
            //禁止选中和自动赋予z值的操作(z值可以通过数据加载确定)
            scene->blockAssignZValue(true);
            scene->blockSelect(true);

            CGraphicsItemGroup *pGroup = scene->loadGroupTreeInfo(tree);
            scene->cancelGroup(pGroup, false);
            delete pGroup;
            PageScene::releaseBzItemsTreeInfo(tree);

            scene->blockSelect(false);
            scene->blockAssignZValue(false);

        }, Qt::QueuedConnection);

        //5.关闭文件结束
        readFile.close();
    }
    contex->setFile(path);
    emit hander->loadEnd(contex, "");
}

QImage loadImage(const QString &path, FilePageHander *hander)
{
    auto fOptimalConditions = [ = ](const QImageReader & reader) {
        QSize orgSz = reader.size();
        return (orgSz.width() > 1920 && orgSz.height() > 1080);
    };

    QImageReader reader;
    reader.setFileName(path);
    bool shouldOptimal = fOptimalConditions(reader);
    qreal radio = shouldOptimal ? 0.5 : 1.0;
    QSize orgSize = reader.size();
    QSize newSize = orgSize * radio;
    reader.setScaledSize(newSize);

    if (shouldOptimal)
        reader.setQuality(30);

    //当不能读取数据时,设置以内容判断格式进行加载,这样可以避免后缀名修改后文件无法打开的情况
    if (!reader.canRead()) {
        reader.setAutoDetectImageFormat(true);
        reader.setDecideFormatFromContent(true);
        reader.setFileName(path); //必须重新设置一下文件,才能触发内部加载方式的切换
    }

    if (reader.canRead()) {
        QImage img = reader.read();
        //维持原大小
        bool haveOptimal = shouldOptimal;
        if (haveOptimal) {
            img = img.scaled(orgSize);
        }
        return img;
    }
    return QImage();
}
/********************************************  LOAD  FUNCTIONS ********************************************/


/********************************************  SAVE STATIC FUNCTIONS ********************************************/
struct STreePlatInfo {
    QByteArray headBytes;
    QByteArray itemsBytes;
    QByteArray md5;
    int bzItemCount = 0;
    int groupCount = 0;
};
void serializationTree_helper_1(QDataStream &countStream, int &bzItemCount, int &groupCount,
                                const CGroupBzItemsTreeInfo &tree, std::function<void(int, int)> f = nullptr)
{
    countStream << tree.childGroups.size();

    qDebug() << "save group count  = " << tree.childGroups.size();

    foreach (auto p, tree.childGroups) {
        serializationTree_helper_1(countStream, bzItemCount, groupCount, p, f);
    }

    countStream << tree.bzItems.size();

    qDebug() << "save item count   = " << tree.bzItems.size();

    foreach (auto i, tree.bzItems) {
        countStream << i;
        qDebug() << "save item info    = " << i.head.dataType;

        ++bzItemCount;
        if (f != nullptr) {
            f(bzItemCount, groupCount);
        }
    }

    //bzItemCount += tree.bzItems.size();

    countStream << tree.data;

    qDebug() << "save group info   = " << tree.data.head.dataType;

    ++groupCount;
    if (f != nullptr) {
        f(bzItemCount, groupCount);
    }
}
STreePlatInfo serializationTreeToBytes_1(const CGroupBzItemsTreeInfo &tree)
{
    STreePlatInfo info;

    QDataStream streamForCountBytes(&info.itemsBytes, QIODevice::WriteOnly);
    serializationTree_helper_1(streamForCountBytes, info.bzItemCount, info.groupCount, tree);

    return info;
}

QByteArray serializationHeadToBytes(const CGraphics &head)
{
    QByteArray  array;
    QDataStream streamForCountBytes(&array, QIODevice::WriteOnly);
    streamForCountBytes << head;

    return array;
}
bool saveDdfWithCombinGroup(const QString &path, PageContext *contex, FilePageHander *hander)
{
    QString error;
    bool result = true;

    emit hander->saveBegin(contex);

    QFileInfo fInfo(path);
    if (fInfo.exists() && !fInfo.isWritable()) {
        error = QObject::tr("This file is read-only, please save with another name");
        emit hander->saveEnd(contex, error);
        return false;
    }

    auto pDrawScen = contex->scene();

    CGroupBzItemsTreeInfo treeInfo = pDrawScen->getGroupTreeInfo(nullptr, ESaveToDDf);

    //0.开始之前设置当前要保存的文件名,并去掉针对该文件的监视(自身程序修改该文件不需要提示文件被修改了)

    //1.将treeInfo进行序列化到内存这个过程很快(很多图片的时候内存是否足够?)
    STreePlatInfo info = serializationTreeToBytes_1(treeInfo);

    //2.初始化文件的头信息(主要包含了ddf版本号,总的图元个数,场景的大小)并序列化到内存
    CGraphics head;
    head.version   = qint32(EDdfCurVersion);
    head.unitCount = info.groupCount + info.bzItemCount;         //在加载时可快速获知图元总数
    head.rect      = pDrawScen->sceneRect();                     //场景的大小
    info.headBytes = serializationHeadToBytes(head);

    //3.将树结构的二进制流数据进行md5加密
    info.md5 = QCryptographicHash::hash(info.headBytes + info.itemsBytes, QCryptographicHash::Md5);

    qDebug() << "save head size = " << info.headBytes.count() << "byte size = " << info.itemsBytes.count() << "md5 count = " << info.md5.count();

    //4.再进行磁盘检查(判断目标文件所处的磁盘分区空间是否足够),如果未通过,交互提示返回
    if (!hander->isVolumeSpaceAvailabel(path, info.itemsBytes.size() + info.md5.size())) {
        qWarning() << "volum space is not enough !!!";
        error = QObject::tr("Unable to save. There is not enough disk space.");
        result = false;
    } else {
        //5.将数据保存到文件(或者一开始就启动线程?)
        QFile writeFile(path);
        if (writeFile.open(QIODevice::WriteOnly | QIODevice::ReadOnly | QIODevice::Truncate)) {
            QDataStream out(&writeFile);

            //5.1 首先保存沿用的老的头信息(主要包含了ddf版本号,总的图元个数,场景的大小)
            int resultByteSize = out.writeRawData(info.headBytes.data(), info.headBytes.size());
            qDebug() << "write heads bytes: " << info.headBytes.size() << "success byteSize: " << resultByteSize;

            //5.2 写入图元的数据(安排一下进度?)
            int granularityByteSize = 200; //设置字节粒度
            resultByteSize = 0;
            while (resultByteSize < info.itemsBytes.size()) {
                qApp->processEvents();
                QByteArray bytes = info.itemsBytes.mid(resultByteSize, granularityByteSize);
                resultByteSize += out.writeRawData(bytes.data(), bytes.size());
                //if (!syn)
                emit hander->saveUpdate(contex, resultByteSize, info.itemsBytes.size());
            }
            qDebug() << "write items bytes: " << info.itemsBytes.size() << "success byteSize: " << resultByteSize;

            //5.3 写入md5数据
            resultByteSize = out.writeRawData(info.md5.data(), info.md5.size());
            qDebug() << "write  md5  bytes: " << info.md5.size() << "success byteSize: " << resultByteSize << "md5 = " << info.md5.toHex().toUpper();

            //5.4 关闭文件(执行后才会真的写入到磁盘)
            writeFile.close();

            if (writeFile.error() != QFileDevice::NoError)
                error = writeFile.errorString();
            else {
                contex->setFile(path);
            }

            qDebug() << "save ddf end---------------- " << writeFile.error();
        }
    }

    PageScene::releaseBzItemsTreeInfo(treeInfo);

    emit hander->saveEnd(contex, error);

    return result;
}

/********************************************  SAVE STATIC FUNCTIONS ********************************************/



FilePageHander::FilePageHander(QObject *parent): QObject(parent)
{
    _pPrivate = new FilePageHander_private(this);
}

FilePageHander::~FilePageHander()
{
    delete _pPrivate;
}
QStringList FilePageHander::supPictureSuffix()
{
    static QStringList supPictureSuffixs = QStringList() << "png" << "jpg" << "bmp" << "tif" << "jpeg" ;
    return supPictureSuffixs;
//    QStringList supPictureSuffixs;
//    foreach (auto it, QImageWriter::supportedImageFormats()) {
//        supPictureSuffixs.append(it);
//    }
//    return supPictureSuffixs;
}

QStringList FilePageHander::supDdfStuffix()
{
    static QStringList supDdfSuffixs = QStringList() << "ddf";
    return supDdfSuffixs;
}
bool FilePageHander::isLegalFile(const QString &path)
{
    if (path.isEmpty()) {
//        if (outErrorReson != nullptr) {
//            *outErrorReson = 0;
//        }
        return false;
    }

    QRegExp regExp("[:\\*\\?\"<>\\|]");

    if (path.contains(regExp)) {

//        if (outErrorReson != nullptr) {
//            *outErrorReson  = 1;
//        }
        return false;
    }

    QRegExp splitExp("[/\\\\]");


    int pos = splitExp.indexIn(path, 0);

    while (pos != -1) {
        QString dirStr = path.left(pos + 1);
        if (dirStr.count() > 1) {
            QDir dir(dirStr);
            if (!dir.exists()) {
//                if (outErrorReson != nullptr) {
//                    *outErrorReson  = 2;
//                }
                return false;
            }
        }
        pos = splitExp.indexIn(path, pos + 1);
    }

    bool isdir = (path.endsWith('/') || path.endsWith('\\'));
    return !isdir;
}

QString FilePageHander::toLegalFile(const QString &filePath)
{
    QString result = filePath;
    QFileInfo info(filePath);
    if (!info.exists()) {
        QUrl url(filePath);
        if (url.isLocalFile()) {
            result = url.toLocalFile();
        }
    }

    if (!isLegalFile(result))
        return "";

    return result;
}

bool FilePageHander::load(const QString &path, bool forcePageContext, bool syn, PageContext **out, QImage *outImg)
{
    auto legalPath = toLegalFile(path);
    if (legalPath.isEmpty()) {
        return false;
    }
    QFileInfo info(legalPath);
    auto stuff = info.suffix().toLower();
    if (FilePageHander::supDdfStuffix().contains(stuff)) {
        bool result = checkFileBeforeLoad(legalPath);
        if (result) {
            PageContext *contex = new PageContext(legalPath);
            d_pri()->run([ = ] {
                EDdfVersion ddfVersion = getDdfVersion(legalPath);
                if (ddfVersion >= EDdf5_9_0_3_LATER)
                {
                    loadDdfWithCombinGroup(legalPath, contex, this, syn);
                } else
                {
                    loadDdfWithNoCombinGroup(legalPath, contex, this, syn);
                }
            }, legalPath, syn);
            if (syn && out != nullptr) {
                *out = contex;
            }
        }
        return result;
    } else if (FilePageHander::supPictureSuffix().contains(stuff)) {
        PageContext *cxt = nullptr;
        if (forcePageContext) {
            cxt = new PageContext;
        }
        d_pri()->run([ = ] {
            emit loadBegin();
            QString error;
            QImage img = loadImage(legalPath, this);
            if (img.isNull())
            {
                error = tr("Damaged file, unable to open it");
            }
            if (forcePageContext)
            {
                cxt->addImage(img);
//                cxt->setFile(legalPath);
//                cxt->setPageRect(img.rect());
//                cxt->setDirty(false);
                emit loadEnd(cxt, error);
            } else
            {
                emit loadEnd(img, error);
            }
            if (syn && outImg != nullptr)
            {
                *outImg = img;
            }
        }, legalPath, syn);
    }

    return true;
}

bool FilePageHander::save(PageContext *context, const QString &file,
                          bool syn, int imageQuility)
{
    bool ret = false;
    auto filePath = file.isEmpty() ? context->file() : file;
    if (checkFileBeforeSave(filePath)) {
        ret = true;
        auto stuffix = QFileInfo(file).suffix().toLower();
        if (supPictureSuffix().contains(stuffix) || stuffix == "pdf") {
            bool type = stuffix == "png" ? true : false;
            QImage image = context->scene()->renderToImage(type);
            d_pri()->run([ = ] {
                if (context->page() != nullptr)
                    context->page()->borad()->fileWatcher()->removePath(filePath);
                saveToImage(image, file, stuffix, imageQuility);
                if (context->page() != nullptr)
                    context->page()->borad()->fileWatcher()->addWather(filePath);
            }, file, syn);
        } else {
            d_pri()->run([ =, &ret] {
                if (context->page() != nullptr)
                    context->page()->borad()->fileWatcher()->removePath(filePath);
                ret = saveDdfWithCombinGroup(filePath, context, this);
                if (context->page() != nullptr)
                    context->page()->borad()->fileWatcher()->addWather(filePath);
            }, file, syn);
        }
    }
    return  ret;
}

bool FilePageHander::saveToImage(const QImage &image, const QString &file,
                                 const QString &stuff, int imageQuility) const
{
    if (toLegalFile(file).isEmpty())
        return false;

    if (image.isNull())
        return false;

    if (stuff.toLower() == "pdf") {
        QPdfWriter writer(file);
        int ww = image.width();
        int wh = image.height();
        writer.setResolution(96);
        writer.setPageSizeMM(QSizeF(25.4 * ww / 96, 25.4 * wh / 96));
        QPainter painter(&writer);
        painter.drawImage(0, 0, image);
        return true;
    }
    return image.save(file, stuff.toLocal8Bit(), imageQuility);
}

bool FilePageHander::checkFileBeforeLoad(const QString &file)
{
    //0.check file if lege
    if (toLegalFile(file).isEmpty())
        return false;

    //1.first to check ddf version.
    EDdfVersion ddfVersion = getDdfVersion(file);
    if (ddfVersion > EDdfCurVersion) {
        qWarning() << "The file is incompatible with the old app, please install the latest version.";

        //文件版本与当前应用不兼容，请安装最新版应用
        DrawBoard::exeMessage(tr("The file is incompatible with the old app, please install the latest version"),
                              DrawBoard::EWarningMsg, false);

        return false;

    } else if (ddfVersion < 0) {
        qWarning() << "Cannot open unknown version file!";
        return false;
    }

    //2.second to check md5.
    if (isDdfFileDirty(file)) {
        DrawBoard::exeMessage(tr("Unable to open the broken file \"%1\"").arg(QFileInfo(file).fileName()),
                              DrawBoard::EWarningMsg, false);
        return false;
    }

    return true;
}

bool FilePageHander::checkFileBeforeSave(const QString &file)
{
    //0.check file if lege
    if (toLegalFile(file).isEmpty())
        return false;

    return true;
}
bool FilePageHander::isVolumeSpaceAvailabel(const QString &desFile, const int needBytesSize)
{
    /* 如果空间不足那么提示 */
    QString dirPath = QFileInfo(desFile).absolutePath();
    QStorageInfo volume(dirPath);
    //QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes();
    if (volume.isValid()) {
        qint64 availabelCount = volume.bytesAvailable();
        qint64 bytesFree      = volume.bytesFree() ;
        qDebug() << "availabelCount = " << availabelCount << "bytesFree = " << bytesFree;
        if (!volume.isReady() || volume.isReadOnly() || availabelCount < needBytesSize) {
            return false;
        }
    }
    return true;
}

EDdfVersion FilePageHander::getDdfVersion(const QString &file) const
{
    EDdfVersion ver = EDdfUnknowed;
    QFile f(file);
    if (f.exists()) {
        if (f.open(QFile::ReadOnly)) {
            QDataStream s(&f);
            //获取版本号
            ver = getVersion(s);
        }
    }
    return ver;
}
bool FilePageHander::isDdfFileDirty(const QString &filePath)const
{
    QFile file(filePath);
    if (file.exists()) {
        if (file.open(QFile::ReadOnly)) {
            QDataStream s(&file);
            // 先通过版本号判断是否ddf拥有md5校验值
            EDdfVersion ver = getVersion(s);
            if (ver >= EDdf5_8_0_20) {
                QByteArray allBins = file.readAll();
                QByteArray md5    = allBins.right(16);

                qDebug() << "load  head+bytes = " << (allBins.count() - md5.count()) << "md5 count = " << md5.count();
                qDebug() << "direct read MD5 form ddffile file = " << filePath << " MD5 = " << md5.toHex().toUpper();

                QByteArray contex = allBins.left(allBins.size() - md5.size());

                QByteArray nMd5 = QCryptographicHash::hash(contex, QCryptographicHash::Md5);

                qDebug() << "recalculate MD5 form ddffile file = " << filePath << " MD5 = " << nMd5.toHex().toUpper();

                if (md5 != nMd5) {
                    return true;
                }
            } else  if (ver == EDdfUnknowed) {
                return true;
            }
            return false;
        }
    }
    return true;
}

int FilePageHander::activedCount() const
{
    return d_pri()->_futures.count();
}

void FilePageHander::quit()
{
    d_pri()->quitAll();
}
