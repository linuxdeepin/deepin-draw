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

#include "cmasicotool.h"
//#include "cgraphicsmasicoitem.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cgraphicsitemselectedmgr.h"
#include "cpictureitem.h"
#include "global.h"
#include "application.h"
#include "cundoredocommand.h"
#include "cattributeitemwidget.h"
#include "cattributemanagerwgt.h"
#include "blurwidget.h"
#include "cdrawscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QApplication>
#include <DToolButton>
#include <QDebug>

IBlurTool::IBlurTool(QObject *parent): IDrawTool(blur)
{
    setParent(parent);

    connect(this, &IBlurTool::boardChanged, this, [ = ](DrawBoard * old, DrawBoard * cur) {
        Q_UNUSED(old)
        for (int i = 0; i < cur->count(); ++i) {
            auto page = cur->page(i);
            connect(page->scene(), &PageScene::selectionChanged, this, [ = ](const QList<CGraphicsItem * > &children) {
                Q_UNUSED(children)
                setEnable(isEnable(page->view()));
            });
        }
        connect(cur, &DrawBoard::pageAdded, this, [ = ](Page * added) {
            setEnable(isEnable(added->view()));
            connect(added->scene(), &PageScene::selectionChanged, this, [ = ](const QList<CGraphicsItem * > &children) {
                Q_UNUSED(children)
                setEnable(isEnable(added->view()));
            });
        });
        connect(cur, QOverload<Page *>::of(&DrawBoard::currentPageChanged), this, [ = ](Page * cur) {
            setEnable(isEnable(cur->view()));
        });
    });
}

SAttrisList IBlurTool::attributions()
{
    DrawAttribution::SAttrisList result;
    result << defaultAttriVar(DrawAttribution::EBlurAttri);
    return result;
}

JDynamicLayer *IBlurTool::desLayer(PageScene *pScene)
{
    if (_layers[pScene] == nullptr) {
        _layers[pScene] = sceneCurrentLayer(pScene);
    }
    return _layers[pScene];
}

QCursor IBlurTool::cursor() const
{
    static QPixmap s_cur = QPixmap(":/cursorIcons/smudge_mouse.svg");
    return QCursor(s_cur);
}

QAbstractButton *IBlurTool::initToolButton()
{
    DToolButton *m_blurBtn = new DToolButton;
    m_blurBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_B)));
    setWgtAccesibleName(m_blurBtn, "Blur tool button");
    m_blurBtn->setToolTip(tr("Blur (B)"));
    m_blurBtn->setIconSize(QSize(48, 48));
    m_blurBtn->setFixedSize(QSize(37, 37));
    m_blurBtn->setCheckable(true);
    m_blurBtn->setEnabled(false);

    connect(m_blurBtn, &DToolButton::toggled, m_blurBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_smudge tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_smudge tool_active");
        m_blurBtn->setIcon(b ? activeIcon : icon);
    });
    m_blurBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_normal"));

    return m_blurBtn;
}

void IBlurTool::registerAttributionWidgets()
{
    auto pBlurWidget = new BlurWidget;
    connect(pBlurWidget, &BlurWidget::blurWidthChanged, this, [ = ](int width) {

        drawBoard()->setDrawAttribution(DrawAttribution::BlurPenWidth, width, EChanged, false);
    });

    connect(pBlurWidget, &BlurWidget::blurTypeChanged, this, [ = ](EBlurEffect type) {
        drawBoard()->setDrawAttribution(DrawAttribution::BlurPenEffect, int(type), EChanged, false);
    });

    drawBoard()->attributionWidget()->setDefaultAttributionVar(DrawAttribution::BlurPenWidth, 20);
    drawBoard()->attributionWidget()->setDefaultAttributionVar(DrawAttribution::BlurPenEffect, int(MasicoEffect));
    drawBoard()->attributionWidget()->installComAttributeWgt(DrawAttribution::EBlurAttri, pBlurWidget);
}

void IBlurTool::toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    if (isFirstEvent()) {
        auto layer = desLayer(event->scene());
        _saveZs.insert(layer, LayerSaveInfo(layer->zValue(), layer->isBlocked()));
        layer->setZValue(INT32_MAX - 1);
//        layer->setBlocked(false);
    }
}

int IBlurTool::decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    if (desLayer(event->scene()) != nullptr) {
        desLayer(event->scene())->blurBegin(desLayer(event->scene())->mapFromScene(event->pos()));
        return 1;
    }

    return 0;
}

void IBlurTool::toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    desLayer(event->scene())->blurUpdate(desLayer(event->scene())->mapFromScene(event->pos()));
}

void IBlurTool::toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    auto layer = desLayer(event->scene());
    layer->blurEnd();
    if (isFinalEvent()) {
        auto stackData = _saveZs[layer];
        layer->setZValue(stackData.z);
//        layer->setBlocked(stackData.blocked);
        _layers.remove(event->scene());
        _saveZs.remove(layer);
    }
}

bool IBlurTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    return false;
}

bool IBlurTool::isEnable(PageView *pView)
{
    if (pView == nullptr)
        return false;
    return (sceneCurrentLayer(pView->drawScene()) != nullptr);
}

int IBlurTool::allowedMaxTouchPointCount()
{
    return 1;
}

void IBlurTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
    Q_UNUSED(oldStatus)
    Q_UNUSED(nowStatus)

    if (oldStatus == EIdle && nowStatus == EReady) {
        qApp->installEventFilter(this);
    } else if (oldStatus == EReady && nowStatus == EIdle) {
        qApp->removeEventFilter(this);
    }
}

JDynamicLayer *IBlurTool::sceneCurrentLayer(PageScene *scene)
{
    if (scene == nullptr)
        return nullptr;

    JDynamicLayer *layer = nullptr;

    auto items = scene->selectGroup()->items();
    if (items.count() == 1) {
        CGraphicsItem *pItem = items[0];

        if (pItem->isBzGroup()) {
            QList<CGraphicsItem *> lists = static_cast<CGraphicsItemGroup *>(pItem)->getBzItems(true);
//            foreach (CGraphicsItem *p, lists) {
//                if (p->isBlurEnable() && dynamic_cast<JDynamicLayer *>(p) != nullptr) {
//                    layer = dynamic_cast<JDynamicLayer *>(p);
//                    break;
//                }
//            }
            if (lists.count() == 1) {
                CGraphicsItem *p = lists.first();
                if (p->isBlurEnable() && dynamic_cast<JDynamicLayer *>(p) != nullptr) {
                    layer = dynamic_cast<JDynamicLayer *>(p);
                }
            }

        } else {
            if (pItem->isBlurEnable()) {
                if (pItem->isBlurEnable() && dynamic_cast<JDynamicLayer *>(pItem) != nullptr) {
                    layer = dynamic_cast<JDynamicLayer *>(pItem);
                }
            }
        }
    }

    return layer;
}

bool IBlurTool::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::ShortcutOverride) {
        bool b = IDrawTool::eventFilter(o, e);
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if (keyEvent->matches(QKeySequence::Redo) || keyEvent->matches(QKeySequence::Undo)) {
            QMetaObject::invokeMethod(this, [ = ]() {
                if (drawBoard()->currentPage() != nullptr) {
                    auto scene = drawBoard()->currentPage()->scene();
                    auto ImageLayer = sceneCurrentLayer(scene);
                    if (ImageLayer == nullptr) {
                        drawBoard()->setCurrentTool(selection);
                        scene->drawView()->viewport()->setCursor(Qt::ArrowCursor);
                    }
                }
            }, Qt::QueuedConnection);
        }
        return b;
    }
    return IDrawTool::eventFilter(o, e);
}
