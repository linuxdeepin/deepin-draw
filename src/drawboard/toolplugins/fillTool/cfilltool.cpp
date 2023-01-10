/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: ZhangYong <zhangyong@uniontech.com>
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
#include "cfilltool.h"

#include "pagescene.h"
#include "pageview.h"
#include "pagecontext.h"
#include "attributemanager.h"
#include "cundoredocommand.h"

#include <QToolButton>
#include <QStack>
#include <QtMath>

static void fillColorInPos(QImage &img, const QPoint &posInImage, const QColor &fillColor)
{
    if (img.isNull())
        return;

    if (!img.rect().contains(posInImage)) {
        return;
    }
    QColor borderIfColor = img.pixelColor(posInImage);

    QStack<QPoint> waitToFillPos;
    waitToFillPos << posInImage;
    while (!waitToFillPos.isEmpty()) {
        auto pos   = waitToFillPos.pop();
        auto color = img.pixelColor(pos);
        if (color != fillColor) {
            img.setPixelColor(pos, fillColor);

            for (int di = -1; di < 2; ++di) {
                int x = pos.x() + di;
                for (int dj = -1; dj < 2; ++dj) {
                    int y = pos.y() + dj;
                    QPoint neberPos(x, y);
                    if (img.rect().contains(neberPos)) {
                        auto colorN = img.pixelColor(neberPos);
                        if (colorN == borderIfColor) {
                            waitToFillPos.push(neberPos);
                        }
                    }
                }
            }
        }
    }
}

class JFillCommand: public RasterCommand
{
public:
    JFillCommand(const QColor &color, const QPoint pos): RasterCommand(),
        _color(color), _posInImage(pos)
    {
    }
    int  cmdType() override {return 4;}
    void doCommandFor(RasterItem *p) override
    {
        if (p != nullptr) {
            fillColorInPos(p->rImage(), _posInImage, _color);
        }
    }
    void serialization(QDataStream &out) override
    {
        out << _color;
        out << _posInImage;
    }
    void deserialization(QDataStream &in) override
    {
        in >> _color;
        in >> _posInImage;
    }
    QColor     _color;
    QPoint     _posInImage;
};

FillTool::FillTool(QObject *parent): DrawFunctionTool(parent)
{
    auto btn = toolButton();
    setWgtAccesibleName(btn, "Paint bucket tool button");
    btn->setToolTip(tr("Paint bucket"));
    btn->setIconSize(QSize(48, 48));
    btn->setFixedSize(QSize(37, 37));
    btn->setCheckable(true);
    //btn->setVisible(true);

    connect(btn, &QToolButton::toggled, btn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_brush tool_normal", QIcon(RCC_DRAWBASEPATH + "ddc_brush tool_normal_48px.svg"));
        QIcon activeIcon = QIcon::fromTheme("ddc_brush tool_active", QIcon(RCC_DRAWBASEPATH + "ddc_brush tool_active_48px.svg"));
        btn->setIcon(b ? activeIcon : icon);
    });
    btn->setIcon(QIcon::fromTheme("ddc_brush tool_normal", QIcon(RCC_DRAWBASEPATH + "ddc_brush tool_normal_48px.svg")));

    connect(this, &FillTool::toolManagerChanged, this, [ = ](DrawBoardToolMgr * _t1, DrawBoardToolMgr * _t2) {
        Q_UNUSED(_t1)
        if (_t2 != nullptr) {
            auto board = _t2->drawBoard();
            connect(board, &DrawBoard::pageAdded, this, [ = ](Page * page) {
                page->context()->setDefaultAttri(EUserAttri + 1, QColor(0, 0, 0));
            });
        }
    });
}

int FillTool::toolType() const
{
    return MoreTool + 1;
}


SAttrisList FillTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EUserAttri + 1);
    return result;
}

int FillTool::minMoveUpdateDistance()
{
    return 0;
}

void FillTool::funcStart(ToolSceneEvent *event)
{
    auto item = currentLayer(event);
    if (item == nullptr) {
        return;
    }

    QColor fillColor = event->view()->page()->defaultAttriVar(EUserAttri + 1).value<QColor>();

    if (!fillColor.isValid())
        return;

    if (item->type() == RasterItemType) {
        auto layer = static_cast<RasterItem *>(item);
        QImage &img = layer->rImage();
        QPoint  posInImage = layer->mapScenePosToImage(event->pos()).toPoint();
        fillColorInPos(img, posInImage, fillColor);
        auto cmd = new JFillCommand(fillColor, posInImage);
        layer->appendComand(cmd, false);
    } else {
        auto vectorItem = dynamic_cast<VectorItem *>(item);
        if (vectorItem != nullptr) {
            UndoRecorder cmd(vectorItem, EChanged);
            vectorItem->setBrush(fillColor);
        }
    }

    event->scene()->update();
}

void FillTool::funcUpdate(ToolSceneEvent *event, int decided)
{

}

void FillTool::funcFinished(ToolSceneEvent *event, int decided)
{

}

void FillTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
    Q_UNUSED(oldStatus)
    Q_UNUSED(nowStatus)
}

PageItem *FillTool::currentLayer(ToolSceneEvent *event)
{
//    auto items = event->selectGroup()->items();

//    if (items.count() == 1) {
//        return dynamic_cast<RasterItem *>(items.first());
//    }
//    return nullptr;

    return event->topPageItemUnderPressedPos();
}

TOOLINTERFACE(FillTool)
