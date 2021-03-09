/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cddfmanager.h"
#include "bzItems/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "bzItems/cgraphicsrectitem.h"
#include "bzItems/cpictureitem.h"
#include "bzItems/cgraphicsellipseitem.h"
#include "bzItems/cgraphicslineitem.h"
#include "bzItems/cgraphicstriangleitem.h"
#include "bzItems/cgraphicstextitem.h"
#include "bzItems/cgraphicspenitem.h"
#include "bzItems/cgraphicspolygonitem.h"
#include "bzItems/cgraphicspolygonalstaritem.h"
#include "frame/cgraphicsview.h"
#include "frame/cviewmanagement.h"
#include "widgets/dialog/cprogressdialog.h"
#include "application.h"
#include "frame/ccentralwidget.h"
#include "cdrawscene.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QtConcurrent>
#include <QCryptographicHash>
#include <QStorageInfo>


CDDFManager::CDDFManager(CGraphicsView *view)
    : QObject(view)
    , m_view(view)
    , m_CProgressDialog(new CProgressDialog(view))
    , m_pSaveDialog(new CAbstractProcessDialog(view))
    , m_lastSaveStatus(false)
    , m_lastErrorString("")
    , m_lastError(QFileDevice::NoError)
{
    //connect(this, SIGNAL(signalUpdateProcessBar(int)), m_CProgressDialog, SLOT(slotupDateProcessBar(int)));
    connect(this, SIGNAL(signalUpdateProcessBar(int, bool)), this, SLOT(slotProcessSchedule(int, bool)));
    connect(this, SIGNAL(signalSaveDDFComplete()), this, SLOT(slotSaveDDFComplete()));
    connect(this, SIGNAL(signalLoadDDFComplete(const QString &, bool)), this, SLOT(slotLoadDDFComplete(const QString &, bool)));
}


void CDDFManager::saveToDDF(const QString &path, const QGraphicsScene *scene, bool finishedNeedClose)
{
    //saveDdfWithNoCombinGroup(path, scene, finishedNeedClose);
    saveDdfWithCombinGroup(path, scene, finishedNeedClose);
}

bool CDDFManager::loadDDF(const QString &path, bool isOpenByDDF)
{
    bool result = true;

    EDdfVersion ddfVersion = getDdfVersion(path);

    if (ddfVersion > EDdfCurVersion) {
        qWarning() << "Cannot open higher version file!";

        auto pView = m_view;
        //因为如果右键多个ddf文件进行打开的情况下,遇到加载文件失败时,如果是直接弹出对话框那么会阻塞后续文件的加载,通过队列的方式在加载完成后提示
        QMetaObject::invokeMethod(this, [ = ]() {
            //中文翻译:文件版本与当前应用不兼容，请安装最新版应用
            drawApp->exeMessage(tr("The file is incompatible with the old app, please install the latest version"), Application::EWarningMsg, false);
            this->closeViewPage(pView, true);
        }, Qt::QueuedConnection);

        result = false;

    } else if (ddfVersion < 0) {
        qWarning() << "Cannot open unknown version file!";
        result = false;
    } else {
        if (ddfVersion >= EDdf5_9_0_3_LATER) {
            loadDdfWithCombinGroup(path, isOpenByDDF);
        } else {
            loadDdfWithNoCombinGroup(path, isOpenByDDF);
        }
    }
    return result;
}

bool CDDFManager::getLastSaveStatus() const
{
    return m_lastSaveStatus;
}

QString CDDFManager::getSaveLastErrorString() const
{
    return m_lastErrorString;
}

QFileDevice::FileError CDDFManager::getSaveLastError() const
{
    return m_lastError;
}

void CDDFManager::slotLoadDDFComplete(const QString &path, bool success)
{
    m_CProgressDialog->hide();

    if (success) {
        m_view->getDrawParam()->setDdfSavePath(m_path);
        m_view->setModify(false);
        emit singalEndLoadDDF();
    } else {
        QFileInfo fInfo(path);
        DDialog dia(drawApp->topMainWindowWidget());
        dia.setFixedSize(404, 163);
        dia.setModal(true);
        QString shortenFileName = QFontMetrics(dia.font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia.width() / 2);
        dia.setMessage(tr("Unable to open the broken file \"%1\"").arg(shortenFileName));
        dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));
        dia.addButton(tr("OK"), false, DDialog::ButtonNormal);
        dia.exec();
        closeViewPage(m_view, true);
    }
}

void CDDFManager::slotProcessSchedule(int process, bool isSave)
{
    if (isSave) {
        m_pSaveDialog->setProcess(process);
    } else {
        m_CProgressDialog->slotupDateProcessBar(process);
    }
}

bool CDDFManager::isDdfFileDirty(const QString &filePath)
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

void CDDFManager::closeViewPage(CGraphicsView *pView, bool quitIfEmpty)
{
    if (pView != nullptr) {
        CCentralwidget *pCertralWidget = dynamic_cast<CCentralwidget *>(pView->parentWidget());
        if (pCertralWidget != nullptr) {
            pCertralWidget->closeSceneView(pView, quitIfEmpty, true);
            if (pView == m_view)
                m_view = nullptr;
        }
    }
}

//void CDDFManager::writeMd5ToDdfFile(const QString &filePath)
//{
//    qDebug() << "write Md5 To DdfFile begin, file = " << filePath;
//    bool result = false;
//    QByteArray srcBinArry;
//    QFile f(filePath);
//    if (f.open(QFile::ReadWrite)) {

//        srcBinArry = f.readAll();

//        QDataStream stream(&f);

//        //加密得到md5值
//        QByteArray md5 = QCryptographicHash::hash(srcBinArry, QCryptographicHash::Md5);

//        stream.device()->seek(srcBinArry.size());

//        //防止 << 操作符写入长度，所以用这个writeRawData函数 (只写入md5值不写md5的长度，因为md5是固定16个字节的)
//        stream.writeRawData(md5.data(), md5.size());

//        f.close();

//        result = true;

//        qDebug() << "ddfFile file contex bin size = " << srcBinArry.size() << "result md5 = " << md5.toHex().toUpper();
//    }
//    qDebug() << "write Md5 To DdfFile end, file = " << filePath << " result = " << result;
//}

EDdfVersion CDDFManager::getDdfVersion(const QString filePath)
{
    EDdfVersion ver = EDdfUnknowed;
    QFile file(filePath);
    if (file.exists()) {
        if (file.open(QFile::ReadOnly)) {
            QDataStream s(&file);
            //获取版本号
            ver = getVersion(s);
        }
    }
    return ver;
}
//typedef void (*fCount)(int totalBzItems, int totalGroupItems);
void serializationTree_helper(QDataStream &countStream, int &bzItemCount, int &groupCount,
                              const CGroupBzItemsTreeInfo &tree, std::function<void(int, int)> f = nullptr)
{
    countStream << tree.childGroups.size();

    qDebug() << "save group count  = " << tree.childGroups.size();

    for (auto p : tree.childGroups) {
        serializationTree_helper(countStream, bzItemCount, groupCount, p, f);
    }

    countStream << tree.bzItems.size();

    qDebug() << "save item count   = " << tree.bzItems.size();

    for (auto i : tree.bzItems) {
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
CGroupBzItemsTreeInfo deserializationToTree_helper(QDataStream &inStream, int &outBzItemCount, int &outGroupCount,
                                                   std::function<void(int, int)> f = nullptr)
{
    CGroupBzItemsTreeInfo result;
    int groupCount = 0;
    inStream >> groupCount;
    //qDebug() << "read group count  = " << groupCount;
    for (int i = 0; i < groupCount; ++i) {
        CGroupBzItemsTreeInfo child = deserializationToTree_helper(inStream, outBzItemCount, outGroupCount, f);
        result.childGroups.append(child);
    }
    int bzItemCount = 0;
    inStream >> bzItemCount;
    //qDebug() << "read item count   = " << bzItemCount;
    for (int i = 0; i < bzItemCount; ++i) {
        CGraphicsUnit unit;
        unit.reson = ESaveToDDf;
        inStream >> unit;
        //qDebug() << "read item info    = " << unit.head.dataType;
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

//void CDDFManager::saveDdfWithNoCombinGroup(const QString &path, const QGraphicsScene *scene, bool finishedNeedClose)
//{
//    //1.准备和检查
//    m_graphics.vecGraphicsUnit.clear();
//    QList<QGraphicsItem *> itemList = scene->items(Qt::AscendingOrder);

//    int         primitiveCount = 0;
//    QByteArray  allBytes;
//    QDataStream streamForCountBytes(&allBytes, QIODevice::WriteOnly);
//    foreach (QGraphicsItem *item, itemList) {
//        CGraphicsItem *tempItem =  static_cast<CGraphicsItem *>(item);

//        if (tempItem->type() >= RectType && CutType != item->type() && tempItem->type() < MgrType) {
//            CGraphicsUnit graphicsUnit = tempItem->getGraphicsUnit(ESaveToDDf);
//            m_graphics.vecGraphicsUnit.push_back(graphicsUnit);
//            primitiveCount ++;
//            streamForCountBytes << graphicsUnit;
//        }
//    }

//    m_graphics.version = qint32(EDdf5_9_0_1_LATER);
//    m_graphics.unitCount = primitiveCount;
//    m_graphics.rect = scene->sceneRect();

//    streamForCountBytes << m_graphics;

//    int allBytesCount = allBytes.size() + 16; //16个字节是预留给md5

//    /* 如果空间不足那么提示 */
//    QString dirPath = QFileInfo(path).absolutePath();
//    QStorageInfo volume(dirPath);
//    QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes();
//    if (volume.isValid()) {
//        qint64 availabelCount = volume.bytesAvailable();
//        qint64 bytesFree      = volume.bytesFree() ;
//        qDebug() << "availabelCount = " << availabelCount << "bytesFree = " << bytesFree;
//        if (!volume.isReady() || volume.isReadOnly() || availabelCount < allBytesCount) {
//            QFileInfo fInfo(path);
//            DDialog dia(drawApp->topMainWindowWidget());
//            dia.setFixedSize(404, 163);
//            dia.setModal(true);
//            dia.setMessage(tr("Unable to save. There is not enough disk space."));
//            dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

//            dia.addButton(tr("OK"), false, DDialog::ButtonNormal);

//            dia.exec();

//            m_graphics.vecGraphicsUnit.clear();

//            return;
//        }
//    }

//    //2.真的开始
//    m_finishedClose = finishedNeedClose;
//    m_path = path;
//    m_pSaveDialog->show();
//    m_pSaveDialog->setTitle(tr("Saving..."));
//    m_pSaveDialog->setProcess(0);

//    CManageViewSigleton::GetInstance()->removeWacthedFile(path);
//    QtConcurrent::run([ = ] {
//        QFile writeFile(path);
//        m_lastSaveStatus = false;
//        if (writeFile.open(QIODevice::WriteOnly | QIODevice::ReadOnly))
//        {
//            QDataStream out(&writeFile);

//            out << m_graphics;

//            int count = 0;
//            int totalCount = m_graphics.vecGraphicsUnit.count();
//            //int process = 0;

//            for (CGraphicsUnit &unit : m_graphics.vecGraphicsUnit) {
//                out << unit;
//                //进度条处理
//                count ++;
//                //process = static_cast<int>((count * 1.0 / totalCount) * 100);
//                emit signalUpdateProcessBar(static_cast<int>((count * 1.0 / totalCount) * 100), true);

//                //释放内存
//                unit.release();
//            }

//            writeFile.close();

//            writeMd5ToDdfFile(path);

//            m_graphics.vecGraphicsUnit.clear();

//            m_lastSaveStatus = true;
//        } else
//        {
//            m_lastSaveStatus = false;
//        }
//        m_lastErrorString = writeFile.errorString();
//        m_lastError = writeFile.error();
//        emit signalSaveDDFComplete();
//    });
//}

void CDDFManager::saveDdfWithCombinGroup(const QString &path, const QGraphicsScene *scene, bool finishedNeedClose)
{
    if (scene == nullptr)
        return;

    CDrawScene *pDrawScen = qobject_cast<CDrawScene *>(const_cast<QGraphicsScene *>(scene));

    if (pDrawScen == nullptr)
        return;

    CGroupBzItemsTreeInfo treeInfo = pDrawScen->getGroupTreeInfo(nullptr, ESaveToDDf);

    //0.开始之前设置当前要保存的文件名,并去掉针对该文件的监视(自身程序修改该文件不需要提示文件被修改了)
    m_path          = path;
    CManageViewSigleton::GetInstance()->removeWacthedFile(path);
    m_finishedClose = finishedNeedClose;

    //接下来启动线程
    QtConcurrent::run([ = ] {

        //1.将treeInfo进行序列化到内存这个过程很快(很多图片的时候内存是否足够?)
        STreePlatInfo info = serializationTreeToBytes(treeInfo);

        //2.初始化文件的头信息(主要包含了ddf版本号,总的图元个数,场景的大小)并序列化到内存
        m_graphics.version   = qint32(EDdfCurVersion);
        m_graphics.unitCount = info.groupCount + info.bzItemCount;         //在加载时可快速获知图元总数
        m_graphics.rect      = scene->sceneRect();                         //场景的大小
        info.headBytes       = serializationHeadToBytes(m_graphics);

        //3.将树结构的二进制流数据进行md5加密
        info.md5 = QCryptographicHash::hash(info.headBytes + info.itemsBytes, QCryptographicHash::Md5);

        qDebug() << "save head size = " << info.headBytes.count() << "byte size = " << info.itemsBytes.count() << "md5 count = " << info.md5.count();

        //4.再进行磁盘检查(判断目标文件所处的磁盘分区空间是否足够),如果未通过,交互提示返回
        if (!isVolumeSpaceAvailabel(path, info.itemsBytes.size() + info.md5.size()))
        {
            qWarning() << "volum space is not enough !!!";

            //4.1交互提示空间不足
            QMetaObject::invokeMethod(this, [ = ]() {
                drawApp->exeMessage(tr("Unable to save. There is not enough disk space."));
                m_lastSaveStatus = true;
                m_lastErrorString = "volum space is not enough!";
                m_lastError = QFileDevice::WriteError;
                m_graphics.vecGraphicsUnit.clear();
                CDrawScene::releaseBzItemsTreeInfo(treeInfo);
            }, Qt::QueuedConnection);


            //磁盘检查不通过直接结束,不应该更新保存路径
            //emit signalSaveDDFComplete();

            return;
        }

        //5.展示保存的进度交互界面
        QMetaObject::invokeMethod(this, [ = ]()
        {
            m_pSaveDialog->show();
            m_pSaveDialog->setTitle(tr("Saving..."));
            m_pSaveDialog->setProcess(0);
        }, Qt::QueuedConnection);

        //6.将数据保存到文件(或者一开始就启动线程?)
        QFile writeFile(path);
        if (writeFile.open(QIODevice::WriteOnly | QIODevice::ReadOnly | QIODevice::Truncate))
        {
            QDataStream out(&writeFile);

            //6.1 首先保存沿用的老的头信息(主要包含了ddf版本号,总的图元个数,场景的大小)
            int resultByteSize = out.writeRawData(info.headBytes.data(), info.headBytes.size());
            qDebug() << "write heads bytes: " << info.headBytes.size() << "success byteSize: " << resultByteSize;

            emit signalUpdateProcessBar(10, true);

            //6.2 写入图元的数据(安排一下进度?)
            int granularityByteSize = 200; //设置字节粒度
            resultByteSize = 0;
            while (resultByteSize < info.itemsBytes.size()) {
                QByteArray bytes = info.itemsBytes.mid(resultByteSize, granularityByteSize);
                resultByteSize += out.writeRawData(bytes.data(), bytes.size());
                emit signalUpdateProcessBar(100.0 * resultByteSize / info.itemsBytes.size(), true);
            }
            //resultByteSize = out.writeRawData(info.itemsBytes.data(), info.itemsBytes.size());
            qDebug() << "write items bytes: " << info.itemsBytes.size() << "success byteSize: " << resultByteSize;

            //6.3 写入md5数据
            resultByteSize = out.writeRawData(info.md5.data(), info.md5.size());
            qDebug() << "write  md5  bytes: " << info.md5.size() << "success byteSize: " << resultByteSize << "md5 = " << info.md5.toHex().toUpper();

            //6.4 关闭文件(执行后才会真的写入到磁盘)
            writeFile.close();

            emit signalUpdateProcessBar(100, true);

            //6.5 记录可能的错误原因,在结束后展示交互提示
            m_lastSaveStatus = true;
            m_lastErrorString = writeFile.errorString();
            m_lastError = writeFile.error();

            //6.6 通知结束
            emit signalSaveDDFComplete();

            qDebug() << "save ddf end----------------";
        }
        CDrawScene::releaseBzItemsTreeInfo(treeInfo);
    });
}

void CDDFManager::loadDdfWithNoCombinGroup(const QString &path, bool isOpenByDDF)
{
    emit signalClearSceneBeforLoadDDF();

    m_CProgressDialog->showProgressDialog(CProgressDialog::LoadDDF, isOpenByDDF);
    m_path = path;

    auto currentView = m_view;
    QtConcurrent::run([ = ] {

        //首先判断文件是否是脏的(是否是ddf合法的,判断方式为md5校验(如果ddf文件版本是老版本那么会直接返回false))
        if (isDdfFileDirty(path))
        {
            //弹窗提示
            emit signalLoadDDFComplete(path, false);
            return;
        }

        QFile readFile(path);
        if (readFile.open(QIODevice::ReadOnly))
        {
            QDataStream in(&readFile);

            in >> m_graphics;
            qDebug() << QString("load ddf(%1)").arg(path) << " ddf version = " << m_graphics.version << "graphics count = " << m_graphics.unitCount <<
                     "scene size = " << m_graphics.rect;
            //qDebug() << "m_graphics.unitCount = " << m_graphics.unitCount;
            emit signalStartLoadDDF(m_graphics.rect);

            int count = 0;
            //int process = 0;

            bool firstFlag = true;
            for (int i = 0; i < m_graphics.unitCount; i++) {
                CGraphicsUnit unit;
                unit.reson = ESaveToDDf;
                in >> unit;

                if (TextType == unit.head.dataType) {
                    emit signalAddTextItem(unit);
                } else {
                    qDebug() << "unit.head.dataType != BlurType = " << (unit.head.dataType != BlurType);

                    //如果有老的模糊图元 那么进行交互提示模糊图元会被丢失，或者放弃加载
                    bool foundBlur = (unit.head.dataType == BlurType);
                    if (foundBlur && firstFlag) {
                        firstFlag = false;
                        bool finished = false;
                        QMetaObject::invokeMethod(this, [ =, &finished]() {
                            int ret = drawApp->exeMessage(tr("The blur effect will be lost as the file is in old version. Proceed to open it?"),
                                                          Application::EWarningMsg, false, QStringList() << tr("Open") << tr("Cancel"),
                                                          QList<int>() << 1 << 0);
                            if (ret == 1) {

                                emit signalLoadDDFComplete(path, true);
                                this->closeViewPage(currentView, true);
                                finished = true;
                            }
                        }, Qt::BlockingQueuedConnection);
                        if (finished) {
                            unit.release();
                            readFile.close();
                            return;
                        }
                    }


                    //EDdf5_9_0_3_LATER之后的版本不再存在模糊图元,所以如果加载EDdf5_9_0_3_LATER之前的ddf文件那么不用加载模糊
                    if (unit.head.dataType != BlurType) {
                        CGraphicsItem *pItem = CGraphicsItem::creatItemInstance(unit.head.dataType, unit);
                        emit signalAddItem(pItem);
                    }
                    unit.release();
                }

                //进度条处理
                count ++;
//                process = int(qreal(count) / qreal(m_graphics.unitCount) * 100.0);
                emit signalUpdateProcessBar(int(qreal(count) / qreal(m_graphics.unitCount) * 100.0), false);
            }
            in >> m_graphics.version;
            qDebug() << "loadDDF m_graphics.version = " << m_graphics.version << endl;
            readFile.close();
            emit signalLoadDDFComplete(path, true);
        }
    });
}

void CDDFManager::loadDdfWithCombinGroup(const QString &path, bool isOpenByDDF)
{
    //加载前清理场景
    emit signalClearSceneBeforLoadDDF();

    m_CProgressDialog->showProgressDialog(CProgressDialog::LoadDDF, isOpenByDDF);
    m_path = path;

    QtConcurrent::run([ = ] {
        //首先判断文件是否是脏的(是否是ddf合法的,判断方式为md5校验(如果ddf文件版本是老版本那么会直接返回false))
        if (isDdfFileDirty(path))
        {
            //弹窗提示
            emit signalLoadDDFComplete(path, false);
            return;
        }

        QFile readFile(path);
        if (readFile.open(QIODevice::ReadOnly))
        {
            QDataStream in(&readFile);

            //1.反序列化到头结构中
            in >> m_graphics;
            qint64 totalItemsCountHead = m_graphics.unitCount;

            //2.通知主线程设置当前场景的大小
            emit signalStartLoadDDF(m_graphics.rect);

            //3.反序列化生成图元结构树,同时获取基本图元和组合图元的个数
            int bzItemsCount   = 0;
            int groupItemCount = 0;
            std::function<void(int, int)> fProcess = [ = ](int bzCount, int gpCount) -> void {
                if (totalItemsCountHead != 0)
                    emit signalUpdateProcessBar(100 * (bzCount + gpCount) / totalItemsCountHead, false);
            };
            CGroupBzItemsTreeInfo tree = deserializationToTree_helper(in, bzItemsCount, groupItemCount, fProcess);
            int totalItemsCount = bzItemsCount + groupItemCount;
            if (totalItemsCountHead != totalItemsCount) {
                qWarning() << "get total items count from ddf head that = " << totalItemsCountHead;
                qWarning() << "get total items count from ddf deserialization items that = " << totalItemsCount;
                qWarning() << "items count from size not match deserialization items count !!! now we use the latter.";
            }

            //4.加载图元结构树(队列通知主线程加载)
            QMetaObject::invokeMethod(this, [ = ]() {

                //禁止选中和自动赋予z值的操作(z值可以通过数据加载确定)
                m_view->drawScene()->blockAssignZValue(true);
                m_view->drawScene()->blockSelect(true);

                CGraphicsItemGroup *pGroup = m_view->drawScene()->loadGroupTreeInfo(tree);
                m_view->drawScene()->cancelGroup(pGroup, false);

                m_view->drawScene()->blockSelect(false);
                m_view->drawScene()->blockAssignZValue(false);

            }, Qt::QueuedConnection);

            //5.关闭文件结束
            readFile.close();

            //6.通知主线程结束
            emit signalLoadDDFComplete(path, true);
        }
    });
}

bool CDDFManager::isVolumeSpaceAvailabel(const QString &desFile, const int needBytesSize)
{
    /* 如果空间不足那么提示 */
    QString dirPath = QFileInfo(desFile).absolutePath();
    QStorageInfo volume(dirPath);
    QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes();
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

CDDFManager::STreePlatInfo CDDFManager::serializationTreeToBytes(const CGroupBzItemsTreeInfo &tree)
{
    STreePlatInfo info;

    QDataStream streamForCountBytes(&info.itemsBytes, QIODevice::WriteOnly);
    serializationTree_helper(streamForCountBytes, info.bzItemCount, info.groupCount, tree);

    return info;
}

QByteArray CDDFManager::serializationHeadToBytes(const CGraphics &head)
{
    QByteArray  array;
    QDataStream streamForCountBytes(&array, QIODevice::WriteOnly);
    streamForCountBytes << head;

    return array;
}

void CDDFManager::slotSaveDDFComplete()
{
    CManageViewSigleton::GetInstance()->wacthFile(m_path);

    m_pSaveDialog->hide();

    m_view->getDrawParam()->setDdfSavePath(m_path);

    m_view->setModify(false);

    emit signalSaveFileFinished(m_path, getLastSaveStatus(), getSaveLastErrorString(), getSaveLastError(), m_finishedClose);
}
