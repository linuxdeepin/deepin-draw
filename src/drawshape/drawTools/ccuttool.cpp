// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ccuttool.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "qgraphicsview.h"
#include "globaldefine.h"

#include <DApplication>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <DToolButton>

#include "application.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "drawshape/globaldefine.h"
#include "drawshape/cdrawparamsigleton.h"

#include "application.h"
#include "toptoolbar.h"
#include "ccutwidget.h"
#include "cattributeitemwidget.h"
#include "ccutwidget.h"
#include "cattributemanagerwgt.h"
#include "cundoredocommand.h"
#include "ccutdialog.h"

DWIDGET_USE_NAMESPACE

CCutTool::CCutTool()
    : IDrawTool(cut)
    , m_pCutItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bModify(false)
{
    qDebug() << "Creating CCutTool";
    connect(this, &CCutTool::boardChanged, this, [ = ](DrawBoard * old, DrawBoard * cur) {
        Q_UNUSED(old)
        qDebug() << "Board changed, connecting to new board signals";
        connect(cur, &DrawBoard::pageAdded, this, [ = ](Page * added) {
            if (added->scene() != nullptr) {
                qDebug() << "Page added, storing original size for scene";
                m_originSizeMap.insert(added->scene(), added->pageRect());
            }
        });
        connect(cur, &DrawBoard::pageRemoved, this, [ = ](Page * removed) {
            qDebug() << "Page removed, cleaning up scene size map";
            m_originSizeMap.remove(removed->scene());
        });
    });
}

CCutTool::~CCutTool()
{
    qDebug() << "Destroying CCutTool";
    m_cutItems.clear();
}

DrawAttribution::SAttrisList CCutTool::attributions()
{
    CGraphicsCutItem *pItem = getCutItem(drawBoard()->currentPage()->scene());
    if (pItem != nullptr) {
        qDebug() << "Getting attributions for cut item";
        return pItem->attributions();
    }
    qDebug() << "No cut item found, returning default attributions";
    return QList<int>() << DrawAttribution::ECutToolAttri;
}

QAbstractButton *CCutTool::initToolButton()
{
    qDebug() << "Initializing cut tool button";
    DToolButton *m_cutBtn = new DToolButton;
    m_cutBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_C)));
    setWgtAccesibleName(m_cutBtn, "Crop tool button");
    m_cutBtn->setToolTip(tr("Crop (C)"));
    m_cutBtn->setIconSize(QSize(48, 48));
    m_cutBtn->setFixedSize(QSize(37, 37));
    m_cutBtn->setCheckable(true);
    connect(m_cutBtn, &DToolButton::toggled, m_cutBtn, [ = ](bool b) {
        qDebug() << "Cut tool button toggled:" << b;
        QIcon icon       = QIcon::fromTheme("ddc_screenshot tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_screenshot tool_active");
        m_cutBtn->setIcon(b ? activeIcon : icon);
    });
    m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_normal"));
    return m_cutBtn;
}

void CCutTool::setAttributionVar(int attri, const QVariant &var, int phase, bool autoCmdStack)
{
    if (attri == DrawAttribution::ECutToolAttri) {
        QList<QVariant> vars = var.toList();
        if (vars.count() != 2) {
            qDebug() << "Invalid attribution variables count:" << vars.count();
            return;
        }
        ECutType cuttp = ECutType(vars.first().toInt());
        QSize sz = vars.at(1).toSize();
        qDebug() << "Setting cut tool attribution - type:" << cuttp << "size:" << sz;

        auto scene = drawBoard()->currentPage()->scene();

        this->changeCutType(cuttp, scene);
        this->changeCutSize(scene, sz);
    }
}

void CCutTool::registerAttributionWidgets()
{
    qDebug() << "Registering cut tool attribution widgets";
    CCutWidget *pCutWidget = new CCutWidget;
    setWgtAccesibleName(pCutWidget, "scene cut attribution widget");
    _pCutWidget = pCutWidget;
    pCutWidget->setAutoCalSizeIfRadioChanged(false);
    pCutWidget->setAttribute(Qt::WA_NoMousePropagation, true);
    connect(pCutWidget, &CCutWidget::cutSizeChanged, this, [ = ](const QSize & sz) {
        qDebug() << "Cut size changed:" << sz;
        QList<QVariant> vars;
        vars << pCutWidget->cutType() << sz;
        drawBoard()->setDrawAttribution(DrawAttribution::ECutToolAttri, vars);
    });
    connect(pCutWidget, &CCutWidget::cutTypeChanged, this, [ = ](ECutType tp) {
        qDebug() << "Cut type changed:" << tp;
        if (drawBoard() != nullptr && drawBoard()->currentPage() != nullptr) {
            CCutTool *pTool = dynamic_cast<CCutTool *>(drawBoard()->currentPage()->currentTool_p());
            if (pTool != nullptr) {
                QSizeF resultSz = this->changeCutType(tp, drawBoard()->currentPage()->scene());
                pCutWidget->setCutSize(resultSz.toSize(), false);
                QList<QVariant> vars;
                vars << tp << pCutWidget->cutSize();
                drawBoard()->setDrawAttribution(DrawAttribution::ECutToolAttri, vars);
            }
        }
    });
    connect(pCutWidget, &CCutWidget::finshed, this, [ = ](bool accept) {
        qDebug() << "Cut widget finished, accept:" << accept;
        CCutTool *pTool = dynamic_cast<CCutTool *>(drawBoard()->currentPage()->currentTool_p());
        if (pTool != nullptr) {
            pTool->doFinished(accept, true);
        }
        pCutWidget->hideExpWindow();
    });
    connect(drawBoard()->attributionWidget(), &CAttributeManagerWgt::updateWgt, this, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == pCutWidget) {
            qDebug() << "Updating cut widget with new values";
            QSignalBlocker bloker(pCutWidget);
            QList<QVariant> vars = var.toList();
            if (vars.count() == 2) {
                ECutType cuttp = ECutType(vars.first().toInt());
                pCutWidget->setCutType(cuttp, false, false);
                QSize sz = vars.at(1).toSize();
                pCutWidget->setCutSize(sz, false);
            }
        }
    });
    qWarning() << "CCutTool::registerAttributionWidgets()CCutTool::registerAttributionWidgets()-------";
    drawBoard()->attributionWidget()->installComAttributeWgt(DrawAttribution::ECutToolAttri, pCutWidget);
}

void CCutTool::toolStart(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    qDebug() << "Cut tool started";
    m_pCutItem = getCurCutItem();

    QGraphicsItem *pFirstItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    if (pFirstItem != nullptr) {
        qDebug() << "Setting closed hand cursor for drag operation";
        event->view()->page()->setDrawCursor(Qt::ClosedHandCursor);
    }
}

int CCutTool::decideUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    qDebug() << "Deciding update type for cut tool";
    EOperateType ret = ENothing;

    QGraphicsItem *pStartPosTopQtItem = event->scene()->firstItem(pInfo->_startPos,
                                                                  pInfo->startPosItems, true, true);
    if (event->scene()->isBussizeHandleNodeItem(pStartPosTopQtItem)) {
        qDebug() << "Resize operation detected";
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pStartPosTopQtItem);
        pInfo->_etcopeTpUpdate = pHandle->dir();
        pInfo->etcItems.clear();

        pInfo->etcItems.append(m_pCutItem);

        ret = EResizeMove;
    } else if (pStartPosTopQtItem == m_pCutItem) {
        qDebug() << "Drag move operation detected";
        ret = EDragMove;
    }

    pInfo->_opeTpUpdate = ret;

    return pInfo->_opeTpUpdate;
}

void CCutTool::toolUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo->_opeTpUpdate == EDragMove) {
        qDebug() << "Updating drag move operation";
        m_pCutItem->move(pInfo->_prePos, event->pos());
    } else if (pInfo->_opeTpUpdate == EResizeMove) {
        qDebug() << "Updating resize operation";
        CSizeHandleRect::EDirection direction = CSizeHandleRect::EDirection(pInfo->_etcopeTpUpdate);
        m_pCutItem->resizeCutSize(direction, pInfo->_prePos, event->pos(), &pInfo->_prePos);
        _pCutWidget->setCutSize(m_pCutItem->rect().size().toSize(), false);
    }
    event->view()->viewport()->update();
}

void CCutTool::toolFinish(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    qDebug() << "Cut tool finished";
    mouseHoverEvent(event);
}

void CCutTool::mouseHoverEvent(CDrawToolEvent *event)
{
    QPointF scenePos = event->pos();

    QList<QGraphicsItem *> items = event->scene()->items(scenePos);

    QGraphicsItem *pItem = event->scene()->firstItem(scenePos, items, true, true, false, false);

    if (event->scene()->isBussizeHandleNodeItem(pItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pItem);
        event->view()->page()->setDrawCursor(pHandle->getCursor());
    } else if (pItem != nullptr && pItem->type() == CutType) {
        event->view()->page()->setDrawCursor(Qt::OpenHandCursor);
    } else {
        event->view()->page()->setDrawCursor(Qt::ArrowCursor);
    }
}

void CCutTool::createCutItem(PageScene *scene)
{
    if (!m_cutItems.contains(scene)) {
        qDebug() << "Creating new cut item for scene";
        deleteCutItem(scene);

        scene->clearSelectGroup();

        m_pCutItem = new CGraphicsCutItem(scene->sceneRect());
        scene->addCItem(m_pCutItem);

        if (m_originSizeMap.find(scene) != m_originSizeMap.end()) {
            qDebug() << "Setting original init rect from map";
            m_pCutItem->setOriginalInitRect(m_originSizeMap[scene]);
        }

        m_dragHandle = CSizeHandleRect::None;

        m_pCutItem->setIsFreeMode(true);
        m_pCutItem->setSelected(true);
        m_bModify = false;

        m_cutItems.insert(scene, m_pCutItem);

        if (_pCutWidget != nullptr) {
            qDebug() << "Updating cut widget size";
            _pCutWidget->setCutSize(m_pCutItem->rect().size().toSize(), false);
        }
    }
}

void CCutTool::deleteCutItem(PageScene *scene)
{
    if (m_cutItems.contains(scene)) {
        qDebug() << "Deleting cut item for scene";
        auto itf = m_cutItems[scene];
        CGraphicsCutItem *pCutItem = itf;
        scene->removeCItem(pCutItem);

        if (pCutItem == m_pCutItem) {
            m_pCutItem = nullptr;
        }

        delete pCutItem;
        m_cutItems.remove(scene);
    }
}

QSizeF CCutTool::changeCutType(int type, PageScene *scene)
{
    qDebug() << "Changing cut type to:" << type;
    CGraphicsCutItem *pItem = getCutItem(scene);
    if (pItem != nullptr) {
        if (cut_free == type) {
            qDebug() << "Setting free mode";
            pItem->setIsFreeMode(true);
            pItem->setSelected(true);
        } else {
            qDebug() << "Setting ratio type:" << type;
            pItem->setSelected(false);
            pItem->setRatioType(ECutType(type));
        }
        m_bModify = true;
        return pItem->rect().size();
    }
    return QSizeF(0, 0);
}

void CCutTool::changeCutSize(const PageScene *scene, const QSize &size)
{
    qDebug() << "Changing cut size to:" << size;
    CGraphicsCutItem *pItem = getCutItem(const_cast<PageScene *>(scene));
    if (pItem != nullptr) {
        pItem->setSize(size.width(), size.height());
        m_bModify = true;
    }
}

void CCutTool::doFinished(bool accept, bool cmd)
{
    qDebug() << "Finishing cut operation, accept:" << accept << "cmd:" << cmd;
    CGraphicsCutItem *pCutItem = getCurCutItem();

    QRectF wantedRect = pCutItem->mapRectToScene(pCutItem->rect());
    bool changed = wantedRect != pCutItem->originalRect();
    if (accept && changed) {
        qDebug() << "Applying cut changes to scene";
        CCmdBlock block(cmd ? drawBoard()->currentPage()->scene() : nullptr);
        PageView *pView = drawBoard()->currentPage()->view();
        pView->drawScene()->setSceneRect(wantedRect);
    }
    drawBoard()->setCurrentTool(selection);
}

bool CCutTool::getCutStatus()
{
    qDebug() << "Getting cut status";
    CGraphicsCutItem *pCutItem = getCurCutItem();
    PageView *pView = drawBoard()->currentPage()->view();

    if (pCutItem == nullptr || pView == nullptr) {
        qDebug() << "pCutItem or pView is nullptr";
        return false;
    }

    QRectF sceneItemBd = pCutItem->mapRectToScene(pCutItem->rect());
    bool status = sceneItemBd != pView->sceneRect();
    qDebug() << "Cut status:" << status;
    return status;
}

CGraphicsCutItem *CCutTool::getCurCutItem()
{
    PageView *pView = drawBoard()->currentPage()->view();
    if (pView != nullptr) {
        return getCutItem(pView->drawScene());
    }
    return nullptr;
}

CGraphicsCutItem *CCutTool::getCutItem(PageScene *scene)
{
    if (scene != nullptr) {
        if (m_cutItems.contains(scene)) {
            return m_cutItems[scene];
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

void CCutTool::onStatusChanged(EStatus oldStatus, EStatus newStatus)
{
    qDebug() << "Cut tool status changed from" << oldStatus << "to" << newStatus;
    if (currentPage() == nullptr)
        return;

    auto page = currentPage();
    if (oldStatus == EIdle && newStatus == EReady) {
        qDebug() << "Installing event filter and creating cut item";
        page->installEventFilter(this);
        createCutItem(page->scene());
    } else if (newStatus == EIdle) {
        qDebug() << "Removing event filter and deleting cut item";
        page->removeEventFilter(this);
        deleteCutItem(page->scene());
    }
}

bool CCutTool::blockPageBeforeOutput(Page *page)
{
    if (page->currentTool_p() == this) {
        qDebug() << "Blocking page before output";
        drawBoard()->setCurrentPage(page);
        CCutDialog dialog(drawBoard());
        int ret = dialog.exec();
        if (CCutDialog::Save == ret) {
            qDebug() << "Save selected, finishing cut operation";
            doFinished(true, true);
        }  else if (CCutDialog::Discard == ret) {
            qDebug() << "Discard selected, returning to selection tool";
            drawBoard()->setCurrentTool(selection);
        }
    }
    return false;
}

bool CCutTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return false;
}

bool CCutTool::eventFilter(QObject *o, QEvent *e)
{
    if (o == drawBoard()->currentPage()) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvnt = static_cast<QKeyEvent *>(e);
            if (keyEvnt->key() == Qt::Key_Return || keyEvnt->key() == Qt::Key_Enter) {
                qDebug() << "Enter/Return key pressed, finishing cut operation";
                doFinished(true, true);
            }
        }
    }
    return IDrawTool::eventFilter(o, e);
}
