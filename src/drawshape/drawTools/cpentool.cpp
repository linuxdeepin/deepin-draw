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

#include "cpentool.h"
#include "cdrawscene.h"
#include "cgraphicspenitem.h"
#include "frame/cgraphicsview.h"
#include "application.h"
#include "cattributeitemwidget.h"
#include "cattributemanagerwgt.h"
#include "seperatorline.h"
#include "application.h"
#include "cpictureitem.h"
#include "cviewmanagement.h"
#include "cgraphicslayer.h"
#include "cgraphicsitemevent.h"
#include "cundoredocommand.h"
#include "ccolorpickwidget.h"
#include "pickcolorwidget.h"

#include <QAbstractItemView>
#include <QUndoStack>
#include <QtMath>
#include <QPicture>
#include <DToolButton>

CPenTool::CPenTool()
    : IDrawTool(pen)
{

}

CPenTool::~CPenTool()
{

}

DrawAttribution::SAttrisList CPenTool::attributions()
{
    DrawAttribution::SAttrisList result;
    result << defaultAttriVar(DrawAttribution::EPenColor)
           << defaultAttriVar(DrawAttribution::EPenStyle)
           << defaultAttriVar(DrawAttribution::EPenWidth);
    return result;
}

QCursor CPenTool::cursor() const
{
    static QPixmap s_cur = QPixmap(":/cursorIcons/brush_mouse.svg");
    return QCursor(s_cur, 7, 26);
}

QAbstractButton *CPenTool::initToolButton()
{
    DToolButton *m_penBtn = new DToolButton;
    m_penBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_P)));
    drawApp->setWidgetAccesibleName(m_penBtn, "Pencil tool button");
    m_penBtn->setToolTip(tr("Pencil(P)"));
    m_penBtn->setIconSize(QSize(48, 48));
    m_penBtn->setFixedSize(QSize(37, 37));
    m_penBtn->setCheckable(true);

    connect(m_penBtn, &DToolButton::toggled, m_penBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_brush tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_brush tool_active");
        m_penBtn->setIcon(b ? activeIcon : icon);
    });
    m_penBtn->setIcon(QIcon::fromTheme("ddc_brush tool_normal"));
    return m_penBtn;
}

void CPenTool::registerAttributionWidgets()
{
    //8.线条开端样式设置控件
    auto streakBeginStyle = new CComBoxSettingWgt(tr("Start"));
    auto pStreakStartComboBox = new QComboBox;
    drawApp->setWidgetAccesibleName(pStreakStartComboBox, "Line start style combox");
    pStreakStartComboBox->setFixedSize(QSize(90, 36));
    pStreakStartComboBox->setIconSize(QSize(34, 20));
    pStreakStartComboBox->setFocusPolicy(Qt::NoFocus);

    pStreakStartComboBox->addItem(QIcon::fromTheme("ddc_none_arrow"), "");
    pStreakStartComboBox->addItem(QIcon::fromTheme("ddc_right_circle"), "");
    pStreakStartComboBox->addItem(QIcon::fromTheme("ddc_right_fill_circle"), "");
    pStreakStartComboBox->addItem(QIcon::fromTheme("ddc_right_arrow"), "");
    pStreakStartComboBox->addItem(QIcon::fromTheme("ddc_right_fill_arrow"), "");

    streakBeginStyle->setComboBox(pStreakStartComboBox);
    connect(pStreakStartComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), streakBeginStyle, [ = ](int index) {
        emit drawApp->attributionsWgt()->attributionChanged(EStreakBeginStyle, index);
    });
    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, streakBeginStyle, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == streakBeginStyle) {
            QSignalBlocker bloker(pStreakStartComboBox);
            pStreakStartComboBox->setCurrentIndex(var.toInt());
        }
    });

    streakBeginStyle->setAttribution(EStreakBeginStyle);
    CAttributeManagerWgt::installComAttributeWgt(EStreakBeginStyle, streakBeginStyle, 0);

    //9.线条末端样式设置控件
    auto streakEndStyle = new CComBoxSettingWgt(tr("End"));
    streakEndStyle->setAttribution(EStreakEndStyle);
    auto pStreakEndComboBox = new QComboBox;
    drawApp->setWidgetAccesibleName(pStreakEndComboBox, "Line end style combox");
    pStreakEndComboBox->setFixedSize(QSize(90, 36));
    pStreakEndComboBox->setIconSize(QSize(34, 20));
    pStreakEndComboBox->setFocusPolicy(Qt::NoFocus);

    pStreakEndComboBox->addItem(QIcon::fromTheme("ddc_none_arrow"), "");
    pStreakEndComboBox->addItem(QIcon::fromTheme("ddc_left_circle"), "");
    pStreakEndComboBox->addItem(QIcon::fromTheme("ddc_left_fill_circle"), "");
    pStreakEndComboBox->addItem(QIcon::fromTheme("ddc_left_arrow"), "");
    pStreakEndComboBox->addItem(QIcon::fromTheme("ddc_left_fill_arrow"), "");

    streakEndStyle->setComboBox(pStreakEndComboBox);
    connect(pStreakEndComboBox, QOverload<int>::of(&DComboBox::currentIndexChanged), streakEndStyle, [ = ](int index) {
        emit drawApp->attributionsWgt()->attributionChanged(EStreakEndStyle, index);
    });
    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, streakEndStyle, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == streakEndStyle) {
            QSignalBlocker bloker(pStreakEndComboBox);
            pStreakEndComboBox->setCurrentIndex(var.toInt());
        }
    });
    CAttributeManagerWgt::installComAttributeWgt(EStreakEndStyle, streakEndStyle, 0);

    //注册分隔符
    auto spl = new SeperatorLine();
    CAttributeManagerWgt::installComAttributeWgt(1775, spl);

    //10.画笔样式设置控件
    auto penStyleWgt = new CComBoxSettingWgt;
    penStyleWgt->setAttribution(EPenStyle);
    m_pPenStyleComboBox = new QComboBox;
    drawApp->setWidgetAccesibleName(m_pPenStyleComboBox, "Pen style combobox");

    m_pPenStyleComboBox->view()->installEventFilter(this);

    m_pPenStyleComboBox->setFixedSize(QSize(182, 36));
    m_pPenStyleComboBox->setIconSize(QSize(24, 20));
    m_pPenStyleComboBox->setFocusPolicy(Qt::NoFocus);

    m_pPenStyleComboBox->addItem(tr("Watercolor pen"));
//    m_pPenStyleComboBox->addItem(tr("Calligraphy pen"));
//    m_pPenStyleComboBox->addItem(tr("Crayon"));

    penStyleWgt->setComboBox(m_pPenStyleComboBox);

    connect(m_pPenStyleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), penStyleWgt, [ = ](int index) {
        emit drawApp->attributionsWgt()->attributionChanged(EPenStyle, index);
    });
    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, penStyleWgt, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == penStyleWgt) {
            QSignalBlocker bloker(m_pPenStyleComboBox);
            pStreakEndComboBox->setCurrentIndex(var.toInt());
        }
    });
    CAttributeManagerWgt::installComAttributeWgt(EPenStyle, penStyleWgt, 0);
}

QPixmap CPenTool::pictureColorChanged(const QImage &image, const QColor &color)
{
    QImage _image = image.scaled(24, 24).convertToFormat(QImage::Format_ARGB32);
    for (int i = 0; i < _image.width(); ++i) {
        for (int j = 0; j < _image.height(); ++j) {
            if (_image.pixelColor(i, j).alpha() != 0) {
                _image.setPixelColor(i, j, color);
            }
        }
    }
    QPixmap pixmap;
    pixmap.convertFromImage(_image);
    return pixmap;
}

bool CPenTool::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_pPenStyleComboBox->view() && drawBoard()->currentPage() != nullptr){
        if(e->type() == QEvent::Show){
            QColor color = /*drawApp->currenDefaultAttriVar(EPenColor).value<QColor>();*/
                    drawBoard()->defaultAttriVar(drawBoard()->currentPage(),EPenColor).value<QColor>();
            qWarning() << "asdasdasdad" << color;
            QImage image = QImage(":/icons/deepin/builtin/texts/icon_marker_24px.svg");
            QPixmap pixmap = pictureColorChanged(image, color);
            m_pPenStyleComboBox->setItemIcon(0, QIcon(pixmap));

//            image = QImage(":/icons/deepin/builtin/texts/icon_calligraphy_24px.svg");
//            pixmap = pictureColorChanged(image, color);
//            m_pPenStyleComboBox->setItemIcon(1, QIcon(pixmap));

//            image = QImage(":/icons/deepin/builtin/texts/icon_crayon_24px.svg");
//            pixmap = pictureColorChanged(image, color);
//            m_pPenStyleComboBox->setItemIcon(2, QIcon(pixmap));
        } else if (e->type() == QEvent::Hide){
            m_pPenStyleComboBox->setItemIcon(0, QIcon());
//            m_pPenStyleComboBox->setItemIcon(1, QIcon());
//            m_pPenStyleComboBox->setItemIcon(2, QIcon());
        }
    }
    return IDrawTool::eventFilter(o, e);
}

//void CPenTool::toolCreatItemUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
//{
//    if (pInfo != nullptr) {
//        CGraphicsPenItem *pPenIem = dynamic_cast<CGraphicsPenItem *>(pInfo->businessItem);
//        if (nullptr != pPenIem) {
//            QPointF pointMouse = event->pos();
//            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
//            pPenIem->updatePenPath(pPenIem->mapFromScene(pointMouse), shiftKeyPress);
//            event->setAccepted(true);

//            QPixmap &pix = event->view()->cachedPixmap();
//            QPainter painter(&pix);
//            painter.setRenderHint(QPainter::Antialiasing);
//            QPen p = pPenIem->pen();
//            qreal penW = p.widthF() * event->view()->getScale();
//            p.setWidthF(penW);
//            //修复调节画笔粗细较粗,不透明度较小时绘制画笔图元,绘制过程中画笔中有很多点
//            QColor updateQColor = p.color();
//            updateQColor.setAlpha(255);
//            p.setColor(updateQColor);

//            painter.setPen(p);
//            if (event->keyboardModifiers() != Qt::ShiftModifier) {
//                auto activeLineIt = _activePaintedLines.find(event->uuid());
//                if (activeLineIt != _activePaintedLines.end()) {
//                    QLineF activeLine = activeLineIt.value();
//                    if (!activeLine.isNull()) {
//                        painter.drawLine(activeLine);
//                        _activePaintedLines.erase(activeLineIt);
//                    }
//                }
//                painter.drawLine(event->view()->mapFromScene(pInfo->_prePos), event->view()->mapFromScene(event->pos()));
//            } else {
//                auto activeLineIt = _activePaintedLines.find(event->uuid());
//                if (activeLineIt == _activePaintedLines.end()) {
//                    QLineF activeLine;
//                    activeLine.setP1(event->view()->mapFromScene(pInfo->_prePos));
//                    activeLine.setP2(event->view()->mapFromScene(event->pos()));
//                    _activePaintedLines.insert(event->uuid(), activeLine);
//                } else {
//                    QLineF &activeLine = activeLineIt.value();
//                    activeLine.setP2(event->view()->mapFromScene(event->pos()));
//                }
//            }
//            event->view()->update();
//            event->view()->viewport()->update();
//        }
//    }
//}

//void CPenTool::toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo)
//{
//    if (pInfo != nullptr) {
//        CGraphicsPenItem *pPenIem = dynamic_cast<CGraphicsPenItem *>(pInfo->businessItem);
//        if (nullptr != pPenIem) {
//            if (!pInfo->hasMoved()) {
//                event->scene()->removeCItem(pPenIem);
//                delete pPenIem;
//                pInfo->businessItem = nullptr;
//            } else {
//                pPenIem->drawComplete();
//                if (pPenIem->scene() == nullptr) {
//                    pPenIem->drawScene()->addCItem(pPenIem);
//                }
//                // [BUG 28087] 所绘制的画笔未默认呈现选中状态
//                //pPenIem->setSelected(true);
//                pPenIem->setDrawFlag(false);


//                //_tempEditableItem->loadItem(pPenIem);
//                _layer->addPenPath(pPenIem->mapToItem(_layer, pPenIem->getPath()), pPenIem->pen());
//                pPenIem->drawScene()->removeCItem(pPenIem);
//            }
//        }
//        //1.取消缓存，恢复到正常绘制
//        event->view()->setCacheEnable(false);
//    }

//    //清除缓存数据，避免绘制画笔图案过程中会多出一条线
//    _activePaintedLines.clear();

//    IDrawTool::toolCreatItemFinish(event, pInfo);
//}

CGraphicsItem *CPenTool::creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    return nullptr;
}

void CPenTool::toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
}

int CPenTool::decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    int ret = ENormalPen;
    if (event->keyboardModifiers() & Qt::ControlModifier) {
        ret = ECalligraphyPen;
    } else if (event->keyboardModifiers() & Qt::ShiftModifier) {
        ret = ETempErase;
    }

    if (ret != ETempErase) {
        event->view()->setCacheEnable(true);
    }
    if (ret != 0) {
        _activePictures[event->uuid()].beginSubPicture();
        _activePictures[event->uuid()].setPenForPath(getViewDefualtPen(event->view()));
        _activePictures[event->uuid()].setBrushForPath(getViewDefualtBrush(event->view()));
    }
    return ret;
}

void CPenTool::toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    QPicture picture;
    switch (pInfo->_opeTpUpdate) {
    case ENormalPen: {
        picture = paintNormalPen(event, pInfo);
        break;
    }
    case ECalligraphyPen: {
        picture = paintCalligraphyPen(event, pInfo);
        break;
    }
    case ETempErase: {
        picture = paintTempErasePen(event, pInfo);
        break;
    }
    default:
        break;
    }
    _activePictures[event->uuid()].addSubPicture(picture);
    _activePictures[event->uuid()].addPoint(_layer->mapFromScene((event->pos())));

    //实时显示,绘制到临时显示的缓冲图上
    paintPictureToView(picture, event->view());
}

void CPenTool::toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    auto picture = _activePictures.take(event->uuid());
    picture.endSubPicture();

    if (pInfo->_opeTpUpdate == ENormalPen || pInfo->_opeTpUpdate == ECalligraphyPen) {
        _layer->addPicture(picture.picture(), true, true);
    } else if (pInfo->_opeTpUpdate == ETempErase) {
        _layer->addPicture(picture.picture(), true);
    }

    if (_allITERecordInfo.count() == 1) {
        event->view()->setCacheEnable(false);
    }
}

//void CPenTool::toolCreatItemStart(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
//{
//    if (event->keyboardModifiers() & Qt::ShiftModifier) {
//        QPointF pos = event->view()->mapFromScene(event->pos());
//        QLineF line(pos, pos);
//        _activePaintedLines.insert(event->uuid(), line);
//    }
//    return IDrawTool::toolCreatItemStart(event, pInfo);
//}

int CPenTool::allowedMaxTouchPointCount()
{
    return 10;
}

bool CPenTool::returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    //return !pInfo->hasMoved();
    return false;
}

int CPenTool::minMoveUpdateDistance()
{
    return 1;
}

void CPenTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
    auto scene = currentPage() != nullptr?currentPage()->scene():nullptr;

    if(scene == nullptr)
        return;

    if (oldStatus == EIdle && nowStatus == EReady) {
        if (scene->selectGroup()->items().count() == 1) {
            auto pSelected = dynamic_cast<JDynamicLayer *>(scene->selectGroup()->items().first());
            if (pSelected != nullptr) {
                _layer = pSelected;
            }
        }

        if (_layer == nullptr) {
            _layer = new  JDynamicLayer;
            scene->addCItem(_layer);
        }
    }

    if (oldStatus == EReady && nowStatus == EIdle) {
        _layer = nullptr;
    }
}

QPen CPenTool::getViewDefualtPen(PageView *view) const
{
    PageView *pView = view;
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(pView->page()->defaultAttriVar(EPenColor).value<QColor>());
    pen.setWidthF(pView->page()->defaultAttriVar(EPenWidth).value<qreal>());
    return pen;
}

QBrush CPenTool::getViewDefualtBrush(PageView *view) const
{
    PageView *pView = view;
    QBrush brush;
    brush.setColor(pView->page()->defaultAttriVar(EPenColor).value<QColor>());
    return brush;
}

QPicture CPenTool::paintNormalPen(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
//    QPointF pointMouse = event->pos();
//    bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
//    pPenIem->updatePenPath(pPenIem->mapFromScene(pointMouse), shiftKeyPress);
//    event->setAccepted(true);

//    QPixmap &pix = event->view()->cachedPixmap();
//    QPainter painter(&pix);
//    painter.setRenderHint(QPainter::Antialiasing);
//    QPen p = pPenIem->pen();
//    qreal penW = p.widthF() * event->view()->getScale();
//    p.setWidthF(penW);
//    //修复调节画笔粗细较粗,不透明度较小时绘制画笔图元,绘制过程中画笔中有很多点
//    QColor updateQColor = p.color();
//    updateQColor.setAlpha(255);
//    p.setColor(updateQColor);

//    painter.setPen(p);
//    if (event->keyboardModifiers() != Qt::ShiftModifier) {
//        auto activeLineIt = _activePaintedLines.find(event->uuid());
//        if (activeLineIt != _activePaintedLines.end()) {
//            QLineF activeLine = activeLineIt.value();
//            if (!activeLine.isNull()) {
//                painter.drawLine(activeLine);
//                _activePaintedLines.erase(activeLineIt);
//            }
//        }
//        painter.drawLine(event->view()->mapFromScene(pInfo->_prePos), event->view()->mapFromScene(event->pos()));
//    } else {
//        auto activeLineIt = _activePaintedLines.find(event->uuid());
//        if (activeLineIt == _activePaintedLines.end()) {
//            QLineF activeLine;
//            activeLine.setP1(event->view()->mapFromScene(pInfo->_prePos));
//            activeLine.setP2(event->view()->mapFromScene(event->pos()));
//            _activePaintedLines.insert(event->uuid(), activeLine);
//        } else {
//            QLineF &activeLine = activeLineIt.value();
//            activeLine.setP2(event->view()->mapFromScene(event->pos()));
//        }
//    }
//    event->view()->update();
//    event->view()->viewport()->update();

    QPicture picture;
    QPainter painter(&picture);

    QPointF  prePos = _layer->mapFromScene(pInfo->_prePos);
    QPointF  pos = _layer->mapFromScene((event->pos())) ;

    QLineF l(prePos, pos);

    PageView *pView = event->scene()->drawView();
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(pView->page()->defaultAttriVar(EPenColor).value<QColor>());
    pen.setWidthF(pView->page()->defaultAttriVar(EPenWidth).value<qreal>());
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setCompositionMode(QPainter::CompositionMode_Source);

    painter.drawLine(l);

    painter.end();

    return picture;
}

QPicture CPenTool::paintCalligraphyPen(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    PageView *pView = event->scene()->drawView();

    QPicture picture;
    QPainter painter(&picture);

    QPointF  prePos = _layer->mapFromScene(pInfo->_prePos);
    QPointF  pos = _layer->mapFromScene((event->pos())) ;

    QLineF l(prePos, pos);
    const qreal angleDegress = 30;
    const qreal cW = pView->page()->defaultAttriVar(EPenWidth).value<qreal>() + 10;
    qreal offXLen = qCos(qDegreesToRadians(angleDegress)) * (cW / 2.0);
    qreal offYLen = qSin(qDegreesToRadians(angleDegress)) * (cW / 2.0);
    QPointF point1(prePos.x() - offXLen, prePos.y() - offYLen);
    QPointF point2(prePos.x() + offXLen, prePos.y() + offYLen);

    QPointF point3(pos.x() - offXLen, pos.y() - offYLen);
    QPointF point4(pos.x() + offXLen, pos.y() + offYLen);

    QPainterPath path;
    path.moveTo(point1);

    if (l.length() > 20) {
        path.quadTo(QLineF(point1, point2).center(), point2);
        path.quadTo(QLineF(point2, point4).center(), point4);
        path.quadTo(QLineF(point4, point3).center(), point3);
        path.quadTo(QLineF(point3, point1).center(), point1);
    } else {
        path.lineTo(point2);
        path.lineTo(point4);
        path.lineTo(point3);
        path.lineTo(point1);
    }
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(pView->page()->defaultAttriVar(EPenColor).value<QColor>());

    QPen p; p.setColor(pView->page()->defaultAttriVar(EPenColor).value<QColor>());
    painter.setPen(p);

    painter.setCompositionMode(QPainter::CompositionMode_Source);

    painter.drawPath(path);

    painter.end();

    return picture;
}

QPicture CPenTool::paintTempErasePen(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    PageView *pView = event->scene()->drawView();
    QPicture picture;
    QPainter painter(&picture);

    QPointF  prePos = _layer->mapFromScene(pInfo->_prePos) ;
    QPointF  pos = _layer->mapFromScene((event->pos())) ;
    QLineF line(prePos, pos);
    QPen pen;
    pen.setWidthF(10 + pView->page()->defaultAttriVar(EPenWidth).value<qreal>());
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(pen);
    painter.drawLine(line);
    painter.end();

    _layer->addPicture(picture, false, false);

    return picture;
}

void CPenTool::paintPictureToView(const QPicture &picture, PageView *view)
{
    QPainter painter(&view->cachedPixmap());

    auto trans = _layer->sceneTransform() * view->viewportTransform();

    painter.setTransform(trans);

    painter.drawPicture(QPoint(0, 0), picture);

    view->viewport()->update();
}
