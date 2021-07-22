/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "cdrawparamsigleton.h"
#include <QGuiApplication>
#include <QFontDatabase>
#include <QImage>
#include <QPainter>
#include <QDesktopWidget>
#include <QTimer>

#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cdrawscene.h"
#include "application.h"
#include "cattributemanagerwgt.h"
#include "toptoolbar.h"
#include "cgraphicsitemselectedmgr.h"
#include "cattributemanagerwgt.h"
#include "cdrawtoolmanagersigleton.h"
#include "filehander.h"

static QString genericOneKey()
{
    static int s_pageCount = 0;
    return QString("%1").arg(++s_pageCount);
}
static void update_helper(PageContext *cxt)
{
    if (cxt->page() != nullptr) {
        auto pg = cxt->page();
        pg->setTitle(pg->title());

        if (pg->borad()->currentPage() == pg) {
            pg->borad()->attributionWidget()->setAttributions(pg->borad()->currentAttris());
        }
    }
}

PageContext::PageContext(const QString &file, QObject *parent): QObject(parent), _dirty(0)
{
    _key  = genericOneKey();

    //setFile(file);
    _file = FilePageHander::toLegalFile(file);

    if (file.isEmpty()){
        _name = tr("Unnamed");
        _key == "1" ? _name += "" : _name += _key;
    }
    else {
        QFileInfo info(file);
        _name = info.completeBaseName();
    }

    _scene = new PageScene(this);

    //设置scene大小为屏幕分辨率
    //获取屏幕分辨率
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    //需要乘以系统缩放系数才是最终的大小
    screenRect = QRect(0, 0, qRound(screenRect.width() * desktopWidget->devicePixelRatioF()),
                       qRound(screenRect.height() * desktopWidget->devicePixelRatioF()));
    this->setPageRect(screenRect);
}

QString PageContext::key() const
{
    return _key;
}

Page *PageContext::page() const
{
    return _scene->page();
}

QString PageContext::name() const
{
    return _name;
}

void PageContext::setName(const QString &name)
{
    _name = name;
}

QString PageContext::file() const
{
    return _file;
}

void PageContext::setFile(const QString &file)
{
    auto fileTemp = FilePageHander::toLegalFile(file);
    if (_file != fileTemp) {
        if (file.isEmpty()) {
            //_name = tr("Unnamed%1").arg(_key == "1" ? "" : _key);
        } else {
            QFileInfo info(file);
            _name = info.completeBaseName();
        }
        _file = file;
        setDirty(true);
    }
}

void PageContext::setDefaultAttri(int type, const QVariant &var)
{
    _attriValues[type] = var;
}

QVariant PageContext::defaultAttri(int type) const
{
    auto itf = _attriValues.find(type);
    if (itf == _attriValues.end()) {
        return QVariant();
    }
    return itf.value();
}

bool PageContext::isEmpty() const
{
    return _scene->getBzItems().isEmpty();
}

PageScene *PageContext::scene() const
{
    return _scene;
}

void PageContext::addSceneItem(const CGraphicsUnit &var, bool record, bool releaseUnit)
{
    if (QThread::currentThread() != qApp->thread()) {
        QMetaObject::invokeMethod(this, [ = ]() {
            auto item = CGraphicsItem::creatItemInstance(var.head.dataType, var);
            if (item != nullptr) {
                _scene->addCItem(item, true, record);
            }
            if (releaseUnit) {
                const_cast<CGraphicsUnit &>(var).release();
            }
        }, Qt::QueuedConnection);
    } else {
        auto item = CGraphicsItem::creatItemInstance(var.head.dataType, var);
        if (item != nullptr) {
            _scene->addCItem(item, true, record);
        }
        if (releaseUnit) {
            const_cast<CGraphicsUnit &>(var).release();
        }
    }
}

void PageContext::addImage(const QImage &img, bool record)
{
    CGraphicsUnit unit;
    unit.head.dataType = DyLayer;
    SDynamicLayerUnitData *p = new SDynamicLayerUnitData;
    p->baseImg = img;
    unit.data.pDyLayer = p;

    this->addSceneItem(unit, record);
}

QRectF PageContext::pageRect() const
{
    return _scene->sceneRect();
}

void PageContext::setPageRect(const QRectF &rect)
{
    _scene->setSceneRect(rect);
}

QImage PageContext::renderToImage() const
{
    return _scene->renderToImage();
}

void PageContext::update()
{
    update_helper(this);
}

bool PageContext::save(const QString &file, bool syn, int quility)
{
    if (file.isEmpty())
        return false;

    if (!isDirty())
        return true;

    if (isEmpty())
        return true;

    if (page() != nullptr && page()->borad() != nullptr) {
        page()->borad()->fileHander()->save(this, file.isEmpty() ? this->file() : file, syn, quility);
        return true;
    }
    return false;
}

bool PageContext::isDirty() const
{
    return _dirty;
}

void PageContext::setDirty(bool dirty)
{
    if (_dirty != dirty) {
        _dirty = dirty;
        update();
        emit dirtyChanged(dirty);

        if (page() != nullptr) {
            auto currentModified = page()->borad()->isAnyPageModified();
            if (currentModified != dirty)
                emit page()->borad()->modified(currentModified);
        }
    }
}

SAttrisList PageContext::currentAttris() const
{
    return _scene->currentAttris();
}

//QImage PageContext::toImage(const QSize &size) const
//{
//    QImage image(size.isValid() ? size : _scene->sceneRect().size().toSize(), QImage::Format_ARGB32);

//    QPainter painter(&image);
//    _scene->render(&painter, QRectF(0, 0, image.width(), image.height()), _scene->sceneRect());
//    painter.end();

//    return image;

//    return QImage();
//}

