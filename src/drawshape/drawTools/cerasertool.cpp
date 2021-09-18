/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Wang Yicun <wangyicun@uniontech.com>
 *
 * Maintainer: Ji Xianglong <jixianglong@uniontech.com>
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
#include "cerasertool.h"
#include "cundoredocommand.h"

CEraserTool::CEraserTool()
    : IDrawTool(eraser)
{
    connect(this, &CEraserTool::boardChanged, this, [ = ](DrawBoard * old, DrawBoard * cur) {
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

CEraserTool::~CEraserTool()
{
}

SAttrisList CEraserTool::attributions()
{
    DrawAttribution::SAttrisList result;
    result << defaultAttriVar(DrawAttribution::EEraserWidth);
    return result;
}

QCursor CEraserTool::cursor() const
{
//    static QPixmap s_cur;
//    QString curStr(":/cursorIcons/eraser_mouse.svg");
//    QSvgRenderer render;
//    render.load(curStr);

//    qreal curScale = this->drawBoard()->currentPage()->view()->getScale();

//    QPixmap pix(qRound(m_width * curScale * 3), qRound(m_width * curScale * 3));
//    pix.fill(Qt::transparent);

//    QPainter painter(&pix);
//    render.render(&painter, QRect(QPoint(0, 0), pix.size()));
//    s_cur = pix;

    // 将鼠标光标设置为透明图片
    qreal curScale = this->drawBoard()->currentPage()->view()->getScale();
    qreal deviceRadio = curScale;

    auto width = this->drawBoard()->currentPage()->defaultAttriVar(EEraserWidth).toInt();
    // 图片随橡皮擦粗细大小变化
    QPixmap s_cur(qRound(width * deviceRadio), qRound(width * deviceRadio));
    s_cur.fill(Qt::transparent);

    QPainter painter(&s_cur);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    return QCursor(s_cur);
}

QAbstractButton *CEraserTool::initToolButton()
{
    DToolButton *m_eraserBtn = new DToolButton;
    m_eraserBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_E)));
    setWgtAccesibleName(m_eraserBtn, "Eraser tool button");
    m_eraserBtn->setToolTip(tr("Eraser (E)"));
    m_eraserBtn->setIconSize(QSize(48, 48));
    m_eraserBtn->setFixedSize(QSize(37, 37));
    m_eraserBtn->setCheckable(true);
    connect(m_eraserBtn, &DToolButton::toggled, m_eraserBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_eraser tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_eraser tool_active");
        m_eraserBtn->setIcon(b ? activeIcon : icon);
    });
    m_eraserBtn->setIcon(QIcon::fromTheme("ddc_eraser tool_normal"));
    return m_eraserBtn;
}

void CEraserTool::registerAttributionWidgets()
{
    auto eraserWidget = new CSpinBoxSettingWgt(tr("Width"));
    setWgtAccesibleName(eraserWidget->spinBox(), "Eraser inner spinbox");
    eraserWidget->setAttribution(EEraserWidth);
    eraserWidget->spinBox()->setSpinRange(1, 500);
    eraserWidget->spinBox()->setSuffix("px");

    drawBoard()->attributionWidget()->installComAttributeWgt(EEraserWidth, eraserWidget, 20);
}

CGraphicsItem *CEraserTool::creatItem(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)

    return nullptr;
}

void CEraserTool::toolStart(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    if (isFirstEvent()) {
        // 擦除时置顶
        saveZ(event->scene());
        auto selectItems = event->scene()->selectGroup()->items();
        event->scene()->moveBzItemsLayer(selectItems, EUpLayer, -1);
    }
    toolDoUpdate(event);

    event->scene()->drawView()->viewport()->update();
}

int CEraserTool::decideUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    int ret = drawBoard()->currentPage()->defaultAttriVar(EEraserWidth).toInt();

    if (_layers.find(event->scene()) == _layers.end()) {
        auto scene = event->scene();
        JDynamicLayer *layer = nullptr;
        if (scene->selectGroup()->items().count() == 1) {
            auto pSelected = dynamic_cast<JDynamicLayer *>(scene->selectGroup()->items().first());
            if (pSelected != nullptr) {
                layer = pSelected;
            }
        }
        if (layer == nullptr) {
            return 0;
        }
        _layers.insert(scene, layer);
    }

    if (ret != 0) {
        _activePictures[event->uuid()].beginSubPicture();
    }
    return ret;
}

void CEraserTool::toolUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    QPicture picture = paintTempErasePen(event, pInfo);

    auto pLayer = dynamic_cast<JDynamicLayer *>(_layers[event->scene()]);
    if (pLayer != nullptr) {
        bool blockStatus = pLayer->isBlocked();
        pLayer->setBlocked(false);
        pLayer->addPicture(picture, false, false);
        pLayer->setBlocked(blockStatus);
    }
    _activePictures[event->uuid()].addSubPicture(picture);

    // 获取到当前顶层图元
    auto currenItem = event->scene()->topBzItem(event->pos(), true,
                                                event->eventType() == CDrawToolEvent::ETouchEvent ? drawBoard()->touchFeelingEnhanceValue() : 0);

    // 判断顶层图元是否发生变化
    bool eraserActive = (pLayer == currenItem) ? true : false;

    // 更新鼠标光标
    this->drawBoard()->currentPage()->setDrawCursor(eraserActive ? cursor() : QCursor(Qt::ForbiddenCursor));

    event->view()->viewport()->update();
}

void CEraserTool::toolFinish(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    auto picture = _activePictures.take(event->uuid());
    picture.endSubPicture();
    restoreZ();

    auto pLayer = dynamic_cast<JDynamicLayer *>(_layers[event->scene()]);
    if (pLayer != nullptr) {
        CCmdBlock blocker(pLayer);
        bool blockStatus = pLayer->isBlocked();
        pLayer->setBlocked(false);
        pLayer->addPicture(picture.picture(), true, false, false);
        pLayer->setBlocked(blockStatus);
    }
    if (_allITERecordInfo.count() == 1) {
        _layers.remove(event->scene());
    }

    this->drawBoard()->currentPage()->setDrawCursor(cursor());

    event->view()->viewport()->update();
}

bool CEraserTool::returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)

    return false;
}

bool CEraserTool::isEnable(PageView *pView)
{
    if (pView == nullptr) {
        return false;
    }
    auto items = pView->drawScene()->selectGroup()->items();
    bool isEraser = false;
    if (items.count() == 1) {
        CGraphicsItem *pItem = items[0];
        if (pItem->isEraserEnable())
            isEraser = true;
    }
    return isEraser;
}

void CEraserTool::onStatusChanged(IDrawTool::EStatus oldStatus, IDrawTool::EStatus nowStatus)
{
    auto scene = currentPage() != nullptr ? currentPage()->scene() : nullptr;

    if (scene == nullptr)
        return;

    if (oldStatus == EIdle && nowStatus == EReady) {
        qApp->installEventFilter(this);
        scene->update();
        m_leaved = false;
    } else if (oldStatus == EReady && nowStatus == EIdle) {
        if (drawBoard()->currentPage() != nullptr)
            drawBoard()->currentPage()->view()->viewport()->update();
        qApp->removeEventFilter(this);
        m_leaved = true;
    }
}

void CEraserTool::mouseHoverEvent(CDrawToolEvent *event)
{
    event->scene()->update();
}

void CEraserTool::enterEvent(CDrawToolEvent *event)
{
    m_leaved = false;
    event->view()->viewport()->update();
}

void CEraserTool::leaveEvent(CDrawToolEvent *event)
{
    m_leaved = true;
    event->view()->viewport()->update();
}

void CEraserTool::drawMore(QPainter *painter, const QRectF &rect, PageScene *scene)
{
    Q_UNUSED(rect)

    if (m_leaved)
        return;

    // 自绘鼠标光标
    auto view = scene->drawView();

    // 如果鼠标选中的位置不是图片就返回
    if (view->viewport()->cursor().shape() != Qt::BitmapCursor) {
        return;
    }

    auto posInViewport = view->viewport()->mapFromGlobal(QCursor::pos());

    if (!view->viewport()->rect().contains(posInViewport)) {
        return;
    }

    auto width = scene->drawView()->page()->defaultAttriVar(EEraserWidth).toInt();
    painter->save();
    painter->setClipping(false);
    auto pos =  view->mapToScene(posInViewport);

    qreal half = qMax(width / 2, 1);
    auto rectEllipse = QRectF(pos - QPointF(half, half), pos + QPointF(half, half));
    QPen pen(QColor(0, 0, 0, 200));
    pen.setWidthF(2 / view->getScale());
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(rectEllipse);

    pen.setColor(QColor(255, 255, 255, 100));
    pen.setWidthF(1 / view->getScale());
    painter->setPen(pen);
    auto rectEllipse2 = QRectF(pos - QPointF(half + 2 / view->getScale(), half + 2 / view->getScale()),
                               pos + QPointF(half + 2 / view->getScale(), half + 2 / view->getScale()));
    painter->drawEllipse(rectEllipse2);
    painter->restore();
}

bool CEraserTool::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::ShortcutOverride) {
        bool b = IDrawTool::eventFilter(o, e);
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if (keyEvent->matches(QKeySequence::Redo) || keyEvent->matches(QKeySequence::Undo)) {
            QMetaObject::invokeMethod(this, [ = ]() {
                if (drawBoard()->currentPage() != nullptr) {
                    auto scene = drawBoard()->currentPage()->scene();
                    if (!isEnable(drawBoard()->currentPage()->view())) {
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

QPicture CEraserTool::paintTempErasePen(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    PageView *pView = event->scene()->drawView();
    QPicture picture;
    QPainter painter(&picture);

    QPointF  prePos = _layers[event->scene()]->mapFromScene(pInfo->_prePos) ;
    QPointF  pos = _layers[event->scene()]->mapFromScene((event->pos())) ;
    QLineF line(prePos, pos);
    QPen pen;
    pen.setWidthF(pView->page()->defaultAttriVar(EEraserWidth).value<qreal>());
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(pen);
    painter.drawLine(line);
    painter.end();

    return picture;
}

void CEraserTool::saveZ(PageScene *scene)
{
    _tempZs.clear();
    auto invokedItems = scene->getRootItems(PageScene::EAesSort);
    for (int i = 0; i < invokedItems.size(); ++i) {
        auto pItem = invokedItems.at(i);
        saveItemZValue(pItem);
    }
}

void CEraserTool::saveItemZValue(CGraphicsItem *pItem)
{
    if (pItem->isBzItem()) {
        _tempZs.insert(pItem, pItem->drawZValue());
    } else if (pItem->isBzGroup()) {
        auto items = static_cast<CGraphicsItemGroup *>(pItem)->items();
        for (auto p : items) {
            saveItemZValue(p);
            _tempZs.insert(pItem, pItem->drawZValue());
        }
    }
}

void CEraserTool::restoreZ()
{
    for (auto it = _tempZs.begin(); it != _tempZs.end(); ++it) {
        it.key()->setZValue(it.value());
    }
    _tempZs.clear();
}
