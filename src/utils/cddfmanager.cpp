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
#include "bzItems/cgraphicsmasicoitem.h"
#include "frame/cgraphicsview.h"
#include "frame/cviewmanagement.h"
#include "widgets/dialog/cprogressdialog.h"
#include "application.h"
#include "frame/ccentralwidget.h"

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
    //1.准备和检查
    m_graphics.vecGraphicsUnit.clear();
    QList<QGraphicsItem *> itemList = scene->items(Qt::AscendingOrder);

    int         primitiveCount = 0;
    QByteArray  allBytes;
    QDataStream streamForCountBytes(&allBytes, QIODevice::WriteOnly);
    foreach (QGraphicsItem *item, itemList) {
        CGraphicsItem *tempItem =  static_cast<CGraphicsItem *>(item);

        if (tempItem->type() >= RectType && CutType != item->type() && tempItem->type() < MgrType) {
            CGraphicsUnit graphicsUnit = tempItem->getGraphicsUnit(true);
            m_graphics.vecGraphicsUnit.push_back(graphicsUnit);
            primitiveCount ++;
            streamForCountBytes << graphicsUnit;
        }
    }

    m_graphics.version = qint32(EDdfCurVersion);
    m_graphics.unitCount = primitiveCount;
    m_graphics.rect = scene->sceneRect();

    streamForCountBytes << m_graphics;

    int allBytesCount = allBytes.size() + 16; //16个字节是预留给md5

    /* 如果空间不足那么提示 */
    QString dirPath = QFileInfo(path).absolutePath();
    QStorageInfo volume(dirPath/*path*/);
    QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes();
    if (volume.isValid()) {
        qint64 availabelCount = volume.bytesAvailable();
        qint64 bytesFree      = volume.bytesFree() ;
        qDebug() << "availabelCount = " << availabelCount << "bytesFree = " << bytesFree;
        if (!volume.isReady() || volume.isReadOnly() || availabelCount < allBytesCount) {
            QFileInfo fInfo(path);
            DDialog dia(dApp->topMainWindowWidget());
            dia.setFixedSize(404, 163);
            dia.setModal(true);
            QString shortenFileName = QFontMetrics(dia.font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia.width() / 2);
            //dia.setMessage(tr("volume \'%1\' is out of space,\'%2\' save failed! ").arg(QString::fromUtf8(volume.device())).arg(shortenFileName));
            dia.setMessage(tr("Unable to save. There is not enough disk space."));
            dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

            dia.addButton(tr("OK"), false, DDialog::ButtonNormal);

            dia.exec();

            m_graphics.vecGraphicsUnit.clear();

            return;
        }
    }

    //2.真的开始
    m_finishedClose = finishedNeedClose;
    m_path = path;
    m_pSaveDialog->show();
    m_pSaveDialog->setTitle(tr("Saving..."));
    m_pSaveDialog->setProcess(0);

    CManageViewSigleton::GetInstance()->removeWacthedFile(path);
    QtConcurrent::run([ = ] {
        QFile writeFile(path);
        m_lastSaveStatus = false;
        if (writeFile.open(QIODevice::WriteOnly | QIODevice::ReadOnly))
        {
            QDataStream out(&writeFile);

            out << m_graphics;

            int count = 0;
            int totalCount = m_graphics.vecGraphicsUnit.count();
            int process = 0;

            for (CGraphicsUnit &unit : m_graphics.vecGraphicsUnit) {
                out << unit;
                ///进度条处理
                count ++;
                process = static_cast<int>((count * 1.0 / totalCount) * 100);
                emit signalUpdateProcessBar(process, true);

                ///释放内存
                if (RectType == unit.head.dataType && nullptr != unit.data.pRect) {
                    delete unit.data.pRect;
                    unit.data.pRect = nullptr;
                } else if (EllipseType == unit.head.dataType && nullptr != unit.data.pCircle) {
                    delete unit.data.pCircle;
                    unit.data.pCircle = nullptr;
                } else if (TriangleType == unit.head.dataType && nullptr != unit.data.pTriangle) {
                    delete unit.data.pTriangle;
                    unit.data.pTriangle = nullptr;
                } else if (PolygonType == unit.head.dataType && nullptr != unit.data.pPolygon) {
                    delete unit.data.pPolygon;
                    unit.data.pPolygon = nullptr;
                } else if (polygonalStar == unit.head.dataType && nullptr != unit.data.pPolygonStar) {
                    delete unit.data.pPolygonStar;
                    unit.data.pPolygonStar = nullptr;
                } else if (LineType == unit.head.dataType && nullptr != unit.data.pLine) {
                    delete unit.data.pLine;
                    unit.data.pLine = nullptr;
                } else if (TextType == unit.head.dataType && nullptr != unit.data.pText) {
                    delete unit.data.pText;
                    unit.data.pText = nullptr;
                } else if (PictureType == unit.head.dataType && nullptr != unit.data.pPic) {
                    delete unit.data.pPic;
                    unit.data.pPic = nullptr;
                } else if (PenType == unit.head.dataType && nullptr != unit.data.pPen) {
                    delete unit.data.pPen;
                    unit.data.pPen = nullptr;
                } else if (BlurType == unit.head.dataType && nullptr != unit.data.pBlur) {
                    delete unit.data.pBlur;
                    unit.data.pBlur = nullptr;
                }
            }
            //close时会写入数据到文件这个时候如果什么都不做会触发监视文件的内容改变就会弹出提醒内容被修改了。 所以这里要过滤掉，"设置下次该文件的修改被忽略" 实现过滤
            //内容变化的检测
            //CManageViewSigleton::GetInstance()->addIgnoreCount();

            //CGraphics::recordMd5(out,);

            writeFile.close();

            writeMd5ToDdfFile(path);

            m_graphics.vecGraphicsUnit.clear();

            m_lastSaveStatus = true;
        } else
        {
            m_lastSaveStatus = false;
        }
        m_lastErrorString = writeFile.errorString();
        m_lastError = writeFile.error();
        emit signalSaveDDFComplete();
    });
}

void CDDFManager::loadDDF(const QString &path, bool isOpenByDDF)
{
    emit signalClearSceneBeforLoadDDF();

    m_CProgressDialog->showProgressDialog(CProgressDialog::LoadDDF, isOpenByDDF);
    m_path = path;

    QtConcurrent::run([ = ] {

        //首先判断文件是否是脏的(是否是ddf合法的,判断方式为md5校验(如果ddf文件版本是老版本那么会直接返回false))
        if (isDdfFileDirty(path))
        {
            //弹窗提示
//            QMetaObject::invokeMethod(this, [ = ]() {
//                //证明是被重命名或者删除
//                QFileInfo fInfo(path);
//                DDialog dia(dApp->activationWindow());
//                dia.setFixedSize(404, 163);
//                dia.setModal(true);
//                QString shortenFileName = QFontMetrics(dia.font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia.width() / 2);
//                //dia.setMessage(tr("The file \"%1 \" is damaged and cannot be opened !").arg(shortenFileName));
//                dia.setMessage(tr("Unable to open the broken file \"%1\".").arg(shortenFileName));
//                dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

//                dia.addButton(tr("OK"), false, DDialog::ButtonNormal);
//                dia.exec();
//            }, Qt::QueuedConnection);
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
            int process = 0;

            for (int i = 0; i < m_graphics.unitCount; i++) {
                CGraphicsUnit unit;
                in >> unit;
                if (RectType == unit.head.dataType) {
                    CGraphicsRectItem *item = new CGraphicsRectItem(*(unit.data.pRect), unit.head);
                    item->setXYRedius(unit.data.pRect->xRedius, unit.data.pRect->yRedius);
                    emit signalAddItem(item);

                    if (unit.data.pRect) {
                        delete unit.data.pRect;
                        unit.data.pRect = nullptr;
                    }
                } else if (EllipseType == unit.head.dataType) {
                    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(unit.data.pCircle, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pCircle) {
                        delete unit.data.pCircle;
                        unit.data.pCircle = nullptr;
                    }
                } else if (TriangleType == unit.head.dataType) {
                    CGraphicsTriangleItem *item = new CGraphicsTriangleItem(unit.data.pTriangle, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pTriangle) {
                        delete unit.data.pTriangle;
                        unit.data.pTriangle = nullptr;
                    }
                } else if (PolygonType == unit.head.dataType) {
                    CGraphicsPolygonItem *item = new CGraphicsPolygonItem(unit.data.pPolygon, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPolygon) {
                        delete unit.data.pPolygon;
                        unit.data.pPolygon = nullptr;
                    }
                } else if (PolygonalStarType == unit.head.dataType) {
                    CGraphicsPolygonalStarItem *item = new CGraphicsPolygonalStarItem(unit.data.pPolygonStar, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPolygonStar) {
                        delete unit.data.pPolygonStar;
                        unit.data.pPolygonStar = nullptr;
                    }
                } else if (LineType == unit.head.dataType) {
                    CGraphicsLineItem *item = new CGraphicsLineItem(unit.data.pLine, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pLine) {
                        delete unit.data.pLine;
                        unit.data.pLine = nullptr;
                    }
                } else if (TextType == unit.head.dataType) {
                    emit signalAddTextItem(*unit.data.pText, unit.head);

                    if (unit.data.pText) {
                        delete unit.data.pText;
                        unit.data.pText = nullptr;
                    }
                } else if (PictureType == unit.head.dataType) {
                    CPictureItem *item = new CPictureItem(/*unit.data.pPic, unit.head*/);
                    item->loadGraphicsUnit(unit, true);
                    emit signalAddItem(item);

                    if (unit.data.pPic) {
                        delete unit.data.pPic;
                        unit.data.pPic = nullptr;
                    }
                } else if (PenType == unit.head.dataType) {
                    CGraphicsPenItem *item = new CGraphicsPenItem(unit.data.pPen, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pPen) {
                        delete unit.data.pPen;
                        unit.data.pPen = nullptr;
                    }

                } else if (BlurType == unit.head.dataType) {
                    CGraphicsMasicoItem *item = new CGraphicsMasicoItem(unit.data.pBlur, unit.head);
                    emit signalAddItem(item);

                    if (unit.data.pBlur) {
                        delete unit.data.pBlur;
                        unit.data.pBlur = nullptr;
                    }

                } else {
                    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!unknoewd type !!!!!!!!!!!! = " << unit.head.dataType;
                    break;
                }

                ///进度条处理
                count ++;
                process = int(qreal(count) / qreal(m_graphics.unitCount) * 100.0);
                emit signalUpdateProcessBar(process, false);
            }
            in >> m_graphics.version;
            qDebug() << "loadDDF m_graphics.version = " << m_graphics.version << endl;
            readFile.close();
            emit signalLoadDDFComplete(path, true);
        }
    });

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
        DDialog dia(dApp->topMainWindowWidget());
        dia.setFixedSize(404, 163);
        dia.setModal(true);
        QString shortenFileName = QFontMetrics(dia.font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia.width() / 2);
        dia.setMessage(tr("Unable to open the broken file \"%1\"").arg(shortenFileName));
        dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));
        dia.addButton(tr("OK"), false, DDialog::ButtonNormal);
        dia.exec();

        if (m_view != nullptr) {
            CCentralwidget *pCertralWidget = dynamic_cast<CCentralwidget *>(m_view->parentWidget());
            if (pCertralWidget != nullptr) {
                pCertralWidget->closeSceneView(m_view, true, true);
                m_view = nullptr;
            }
        }
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

void CDDFManager::writeMd5ToDdfFile(const QString &filePath)
{
    qDebug() << "write Md5 To DdfFile begin, file = " << filePath;
    bool result = false;
    QByteArray srcBinArry;
    QFile f(filePath);
    if (f.open(QFile::ReadWrite)) {

        srcBinArry = f.readAll();

        QDataStream stream(&f);

        //加密得到md5值
        QByteArray md5 = QCryptographicHash::hash(srcBinArry, QCryptographicHash::Md5);

        stream.device()->seek(srcBinArry.size());

        //防止 << 操作符写入长度，所以用这个writeRawData函数 (只写入md5值不写md5的长度，因为md5是固定16个字节的)
        stream.writeRawData(md5.data(), md5.size());

        f.close();

        result = true;

        qDebug() << "ddfFile file contex bin size = " << srcBinArry.size() << "result md5 = " << md5.toHex().toUpper();
    }
    qDebug() << "write Md5 To DdfFile end, file = " << filePath << " result = " << result;
}

void CDDFManager::slotSaveDDFComplete()
{
    CManageViewSigleton::GetInstance()->wacthFile(m_path);

    m_pSaveDialog->hide();

    m_view->getDrawParam()->setDdfSavePath(m_path);

    m_view->setModify(false);

    emit signalSaveFileFinished(m_path, getLastSaveStatus(), getSaveLastErrorString(), getSaveLastError(), m_finishedClose);
}
